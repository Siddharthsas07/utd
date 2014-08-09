#include "communication_server.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "str_manip.h"
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include "globals.h"
#include "hr_time.h"
#include "NaiveAtomicNumber.hpp"
#include "CriticalSectionTracker.h"
#include <pthread.h>
#include "ConditionVariable.h"
#include "Mutex.h"
#include "thread_constructs.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;

// The threads for the communication server
recv_thread_t          comm_listen_thread_id;
thread_t               comm_recv_parse_thread_id;
vector<recv_thread_t>  comm_recv_thread_ids;
thread_t               comm_send_thread_id;

// Queues for message passing between threads
concurrent_queue<msg_t> comm_send_q;
concurrent_queue<msg_t> comm_recv_q;

// Critical section handling
CriticalSectionTracker            comm_cs;
concurrent_queue<cs_stat_entry_t> comm_cs_log;

// Network barrier handling
barrier_t              comm_barrier_obj;

// Port and contacts to communicate with
int                    comm_port = -1;
vector<node_contact_t> comm_contacts;

// Communication server setup
int comm_start( char* config_file_path ) {
  int error  = 0;

  if ( config_file_path == NULL ) {
    cout << "comm_start Invalid config file path " << config_file_path << endl;
    error = -1;
    return error;
  }

  // Open configuration file, read only
  string line;
  ifstream config_file( config_file_path, ios::in );

  if ( config_file.is_open() ) {
    // Read comms port
    std::getline( config_file, line );
    comm_port = str_to_uint( line );

    // Read network master
    std::getline( config_file, line );
    NODE_NETWORK_MASTER = str_to_uint( line );

    node_contact_t curr_node;
    curr_node.sockfd = -1;

    // Read <node ID, address> pairs
    while ( config_file.good() ) {
      std::getline( config_file, line );
      curr_node.id = str_to_uint( line );

      std::getline( config_file, line );
      curr_node.address = line;

      error = comm_add_contact( curr_node );
      if ( error == -1 ) {
        cout << "comm_start Couldn't add contact "
          << curr_node.id << "@" << curr_node.address << endl;
      }
    }

    cout << "Config file read complete" << endl;
    
    // Finished reading from file
    config_file.close();

    NODE_NETWORK_SIZE = comm_contacts.size();

    // Initialize the critical section stuff
    comm_cs.rep_deferred.resize( NODE_NETWORK_SIZE, false );
    comm_cs.mutex_tokens.resize( NODE_NETWORK_SIZE, false );
    comm_cs.mutex_tokens[NODE_ID] = true;

    // Spawn listener thread
    error = pthread_create( &(comm_listen_thread_id.id),
                            NULL,
                            comm_listen_thread_start,
                            &comm_listen_thread_id );
    if ( error != 0 ) {
      comm_listen_thread_id.state = T_STATE_STOP;
      cout << "comm_start Couldn't create listen thread" << endl;
      return error;
    }
    comm_listen_thread_id.state = T_STATE_ACTIVE;
    
    cout << "Created listen thread" << endl;

    // Spawn receive parser thread
    error = pthread_create( &(comm_recv_parse_thread_id.id),
                            NULL,
                            comm_recv_parse_thread_start,
                            &comm_recv_parse_thread_id );
    if ( error != 0 ) {
      comm_recv_parse_thread_id.state = T_STATE_STOP;
      cout << "comm_start Couldn't create receive parse thread" << endl;
      return error;
    }
    comm_recv_parse_thread_id.state = T_STATE_ACTIVE;
    
    cout << "Created receive parse thread" << endl;

    // Open a connection to all remote addresses (previously read from file)
    error = comm_open_connections();
    if ( error != 0 ) {
      cout << "comm_start Couldn't connect to all remote addresses" << endl;
      return error;
    }
    
    cout << "Connected to all remote hosts" << endl;
    
    // Spawn sender thread
    error = pthread_create( &(comm_send_thread_id.id),
                            NULL,
                            comm_send_thread_start,
                            &comm_send_thread_id );
    if ( error != 0 ) {
      comm_send_thread_id.state = T_STATE_STOP;
      cout << "comm_start Couldn't create send thread" << endl;
      return error;
    }
    comm_send_thread_id.state = T_STATE_ACTIVE;
    
    cout << "Created send thread" << endl;

    error = comm_barrier();
    if ( error != 0 ) {
      cout << "comm_start Couldn't join the network" << endl;
      return error;
    }
  }

  else {
    error = -1;
  }

  return error;
}

int comm_add_contact( const node_contact_t& contact ) {
  comm_contacts.push_back( contact );

  return 0;
}

int comm_open_connection_once( string address ) {
  int sockfd = -1;
  struct addrinfo hints, *server_info, *p;

  memset( &hints, 0, sizeof(hints) );
  hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6 don't care
  hints.ai_socktype = SOCK_STREAM; // TCP

  // Get address info of remote address
  if ( getaddrinfo( address.c_str(), int_to_str(comm_port).c_str(), &hints, &server_info ) != 0 ) {
    ::freeaddrinfo( server_info );
    cout << "comm_open_connection_once Couldn't get info for remote address "
      << address << endl;
    return -1;
  }

  // loop through all the results and connect to the first we can
  for( p = server_info; p != NULL; p = p->ai_next ) {
    sockfd = ::socket( p->ai_family, p->ai_socktype, p->ai_protocol );
    if ( sockfd == -1 ) {
      continue;
    }

    if ( ::connect( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) {
      continue;
    }

    break;
  }

  if ( p == NULL ) {
    cout << "comm_open_connection_once Couldn't connect to remote address "
      << address << endl;
    ::freeaddrinfo( server_info );
    return -1;
  }

  ::freeaddrinfo( server_info );

  return sockfd;
}

int comm_close_connection( unsigned int n ) {
  if ( n >= comm_contacts.size() ) {
    cout << "communication_server: Couldn't close connection to node " << n << endl;
  }

  ::close( comm_contacts[n].sockfd );
  comm_contacts[n].sockfd = -1;

  return 0;
}

int comm_open_connection_until( string address ) {
  int sockfd = -1;
  struct addrinfo hints, *server_info, *p;

  memset( &hints, 0, sizeof(hints) );
  hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6 don't care
  hints.ai_socktype = SOCK_STREAM; // TCP

  int error = 0;

  do {
    // Get address info of remote address
    error = ::getaddrinfo( address.c_str(), int_to_str(comm_port).c_str(), &hints, &server_info );

    // If the get failed, wait one second and try again
    if ( error != 0 ) {
      cout << "comm_open_connection_until Couldn't get info for remote address "
        << address << endl;
      wait_s_duration( 1 );
    }
  } while ( error != 0 );

  do {
    // loop through all the results and connect to the first we can
    for( p = server_info; p != NULL; p = p->ai_next ) {
      sockfd = ::socket( p->ai_family, p->ai_socktype, p->ai_protocol );
      if ( sockfd == -1 ) {
        continue;
      }

      if ( ::connect( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) {
        continue;
      }

      break;
    }

    // If the parsing failed, wait one second and try again
    if ( p == NULL ) {
      cout << "comm_open_connection_until Couldn't connect to remote address "
        << address << endl;
      wait_s_duration( 1 );
    }
  } while ( p == NULL );

  ::freeaddrinfo( server_info );

  return sockfd;
}

int comm_open_connections() {
  for ( unsigned int i = 0; i < comm_contacts.size(); i++ ) {
    if ( comm_contacts[i].id != NODE_ID ) {
      comm_contacts[i].sockfd = comm_open_connection_until( comm_contacts[i].address );
    }
  }

  return 0;
}

int comm_bind_once( int port ) {
  int sockfd = -1, yes = 1;
  struct addrinfo hints, *server_info, *p;

  memset( &hints, 0, sizeof(hints) );
  hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6 don't care
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags    = AI_PASSIVE;  // Use this IP

  int error =  0;

  if ( ::getaddrinfo( NULL, int_to_str(comm_port).c_str(), &hints, &server_info ) != 0 ) {
    freeaddrinfo( server_info );
    cout << "comm_bind_once Couldn't get info for this address" << endl;
    return -1;
  }

  // loop through all the results and connect to the first we can
  for( p = server_info; p != NULL; p = p->ai_next ) {
    sockfd = ::socket( p->ai_family, p->ai_socktype, p->ai_protocol );
    if ( sockfd == -1 ) {
      continue;
    }

    if ( ::setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) ) == -1 ) {
      // ?
    }

    if ( ::bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) {
      continue;
    }

    break;
  }

  ::freeaddrinfo( server_info );
  
  if ( p == NULL ) {
    cout << "comm_bind_once Couldn't bind to port "
        << comm_port << endl;
    return -1;
  }

  if ( ::listen( sockfd, 15 ) == -1 ) {
    cout << "comm_bind_once Couldn't listen on port " 
      << comm_port << endl;
    return -1;
  }

  return sockfd;
}

// Thread entry functions
void* comm_listen_thread_start( void* data ) {
  recv_thread_t* my_info = reinterpret_cast<recv_thread_t*>(data);
  int newfd = -1, error = 0;
  struct sockaddr_storage remote_addr;
  socklen_t sin_size = sizeof(remote_addr);

  my_info->sockfd = comm_bind_once( comm_port );
  if ( my_info->sockfd < 0 ) {
    cout << "comm_listen_thread_start Couldn't bind to port " << comm_port << endl;
    return NULL;
  }

  // Accept loop
  // Communication hasn't been stopped
  while ( my_info->state == T_STATE_ACTIVE ) {
    newfd = ::accept( my_info->sockfd, (struct sockaddr*)&remote_addr, &sin_size );
    if ( newfd == -1 ) {
      cout << "comm_listen_thread_start Couldn't accept new connection" << endl;
      continue;
    }

    // Spawn receiver thread
    recv_thread_t new_thread;
    comm_recv_thread_ids.push_back( new_thread );

    comm_recv_thread_ids[comm_recv_thread_ids.size()-1].sockfd = newfd;
    error = pthread_create( &(comm_recv_thread_ids[comm_recv_thread_ids.size()-1].id),
                            NULL,
                            comm_recv_thread_start,
                            &comm_recv_thread_ids[comm_recv_thread_ids.size()-1] );

    if ( error != 0 ) {
      comm_recv_thread_ids[comm_recv_thread_ids.size()-1].state = T_STATE_STOP;
      ::close( newfd );
      cout << "comm_listen_thread_start Couldn't create receive thread" << endl;
    }

    else {
      comm_recv_thread_ids[comm_recv_thread_ids.size()-1].state = T_STATE_ACTIVE;
    }
  }

  ::close( my_info->sockfd );

  return NULL;
}

void* comm_recv_thread_start( void* data ) {
  recv_thread_t* my_info = reinterpret_cast<recv_thread_t*>(data);
  char   incoming_buf[1500];
  msg_t* incoming;
  unsigned int remote_ID;

  // Receive loop
  // Communication hasn't been stopped
  while ( my_info->state == T_STATE_ACTIVE ) {
    int bytes = ::recv( my_info->sockfd, incoming_buf, sizeof(incoming_buf), 0 );

    // Normal receive
    if ( bytes > 0 ) {
      incoming = reinterpret_cast<msg_t*>(incoming_buf);
      remote_ID = incoming->send_ID;
      comm_recv_q.push( *incoming );
    }

    // Connection closed
    else if ( bytes <= 0 ) {
      cout << "comm_recv_thread_start Terminating receive thread" << endl;
      comm_close_connection( remote_ID );
      my_info->state = T_STATE_STOP;
      break;
    }
  }

  ::close( my_info->sockfd );

  return NULL;
}

void* comm_recv_parse_thread_start( void* data ) {
  thread_t* my_info = reinterpret_cast<thread_t*>(data);
  //int network_join_count = 1;
  int computation_complete_count = 0;
  int token_count = 0;
  msg_t incoming;
  int error = 0;
  bool defer_it;

  // Parse loop
  // Communication hasn't been stopped
  while ( my_info->state == T_STATE_ACTIVE ) {
    incoming = comm_recv();
    print_recv_msg( incoming );

    switch ( incoming.msg_type ) {
      case MSG_T_REQUEST_CS:
        comm_cs.shared_vars.lock();
        // Calculate our new sequence number
        comm_cs.max_seq_num = fun::max( comm_cs.max_seq_num, incoming.ts );

        // Determine if a reply should be deferred
        defer_it = (comm_cs.requesting_cs
                   && ( incoming.ts > comm_cs.seq_num
                        || ( (incoming.ts == comm_cs.seq_num) && (incoming.send_ID > NODE_ID) ) ) );

        // Defer the reply
        if ( defer_it ) {
          comm_cs.rep_deferred[incoming.send_ID] = true;
        }

        // Send the reply
        else {
          comm_cs.mutex_tokens[incoming.send_ID] = false;
          msg_t outgoing;
          outgoing.dest_ID = incoming.send_ID;
          outgoing.msg_type = MSG_T_REPLY_CS;
          outgoing.ts = comm_cs.seq_num;
          comm_send( outgoing );
        }
        comm_cs.shared_vars.unlock();
        break;

      case MSG_T_REPLY_CS:
        comm_cs.shared_vars.lock();
        comm_cs.mutex_tokens[incoming.send_ID] = true;
        token_count = 0;

        // Determine if this node has permission from all nodes to enter CS
        for ( unsigned int i = 0; i < comm_cs.mutex_tokens.size(); i++ ) {
          if ( comm_cs.mutex_tokens[i] ) {
            token_count++;
          }
        }
        comm_cs.shared_vars.unlock();

        if ( token_count == NODE_NETWORK_SIZE ) {
          comm_cs.entry_ok.broadcast();
        }
        break;

      case MSG_T_RESTART:
        // Nothing...
        break;

      case MSG_T_BARRIER:
        comm_barrier_obj.count++;
        if ( comm_barrier_obj.count.get() == NODE_NETWORK_SIZE ) {
          comm_barrier_obj.cv.broadcast();
        }
        break;

      case MSG_T_END:
        // Nothing...
        break;

      case MSG_T_TERMINATE:
        my_info->state = T_STATE_STOP;
        break;

      default:
        cout << "comm_recv_parse_thread_start Unknown message type" << endl;
        break;
    }
  }

  return NULL;
}

void* comm_send_thread_start( void* data ) {
  thread_t* my_info = reinterpret_cast<thread_t*>(data);
  msg_t outgoing;
  int error = 0;

  // Send loop
  // Communication hasn't been stopped
  while ( my_info->state == T_STATE_ACTIVE ) {
    comm_send_q.wait_and_pop( outgoing );
    
    if ( outgoing.msg_type == MSG_T_TERMINATE ) {
      break;
    }
    
    if ( outgoing.dest_ID >= comm_contacts.size() ) {
      cout << "comm_send_thread_start Unknown destination ID " << outgoing.dest_ID << endl;
      continue;
    }
    
    if ( comm_contacts[outgoing.dest_ID].sockfd < 0 ) {
      cout << "comm_send_thread_start No connection to " << comm_contacts[outgoing.dest_ID].address << endl;
    }

    print_send_msg( outgoing );
    // Send the message
    error = ::send( comm_contacts[outgoing.dest_ID].sockfd, &outgoing, sizeof(outgoing), 0 );
    if ( error == -1 ) {
      cout << "comm_send_thread_start Couldn't send message " << endl;
    }
  }

  return NULL;
}

// Server interaction functions
int comm_send( msg_t msg ) {
  msg.send_ID = NODE_ID;
  comm_send_q.push( msg );

  return 0;
}

int comm_broadcast( msg_t msg ) {
  int error = 0;

  msg_t outgoing = msg;
  outgoing.send_ID = NODE_ID;

  // Loop over contacts and send msg to each one
  for ( unsigned int i = 0; i < comm_contacts.size(); i++ ) {
    if ( comm_contacts[i].id != NODE_ID ) {
      outgoing.dest_ID = comm_contacts[i].id;
      error = comm_send( outgoing );
    }
  }

  return error;
}

msg_t comm_recv() {
  msg_t temp;

  comm_recv_q.wait_and_pop( temp );

  return temp;
}

int comm_barrier() {
  cout << "BARRIER HIT!" << endl;
  
  msg_t barrier_msg;
  barrier_msg.msg_type = MSG_T_BARRIER;
  barrier_msg.ts = comm_cs.seq_num;
  
  comm_barrier_obj.count++;
  
  // Broadcast that this node has hit the barrier
  if ( comm_broadcast( barrier_msg ) != 0 ) {
    cout << "comm_barrier Couldn't broadcast barrier hit" << endl;
    return -1;
  }
  
  if ( comm_barrier_obj.count.get() < NODE_NETWORK_SIZE ) {
    // wait until all other nodes have hit the barrier
    comm_barrier_obj.cv.wait( comm_barrier_obj.mutex );
  }
  
  wait_s_duration(1);
  
  cout << "BARRIER PASSED" << endl;
  
  comm_barrier_obj.count.set(0);
  
  return 0;
}

int comm_stop() {
  // Mark all threads STOP
  comm_listen_thread_id.state = T_STATE_STOP;
  comm_recv_parse_thread_id.state = T_STATE_STOP;
  comm_send_thread_id.state = T_STATE_STOP;
  for ( unsigned int i = 0; i < comm_recv_thread_ids.size(); i++ ) {
    comm_recv_thread_ids[i].state = T_STATE_STOP;
  }

  // Close sockets threads are working with.
  // This gets the receiver threads and the listener thread off the blocking
  // accept() and recv() calls
  ::close( comm_listen_thread_id.sockfd );
  for ( unsigned int i = 0; i < comm_recv_thread_ids.size(); i++ ) {
    ::close( comm_recv_thread_ids[i].sockfd );
  }
  for ( unsigned int i = 0; i < comm_contacts.size(); i++ ) {
    comm_close_connection( i );
  }

  // Push an execution STOP message into the send and recv queues
  // This gets the receiver parser thread and the sending thread off the blocked
  // wait_and_pop() calls
  msg_t terminate;
  terminate.msg_type = MSG_T_TERMINATE;
  comm_recv_q.push( terminate );
  comm_send_q.push( terminate );

  // Join all threads
  pthread_join( comm_listen_thread_id.id, NULL );
  pthread_join( comm_recv_parse_thread_id.id, NULL );
  pthread_join( comm_send_thread_id.id, NULL );
  for ( unsigned int i = 0; i < comm_recv_thread_ids.size(); i++ ) {
    pthread_join( comm_recv_thread_ids[i].id, NULL );
  }

  return 0;
}

int comm_request_cs() {
  // Record start time of request
  cs_stat_entry_t curr_request;
  /*timeval start;
  timeval end;
  gettimeofday(&start,NULL);*/
  CStopWatch timer;
  timer.startTimer();

  comm_cs.shared_vars.lock();

  int req_cnt = 0;
  comm_cs.requesting_cs = true;
  comm_cs.seq_num = comm_cs.max_seq_num + 1;
  msg_t outgoing;
  outgoing.msg_type = MSG_T_REQUEST_CS;
  outgoing.ts = comm_cs.seq_num;

  // Send request messages to all nodes required
  for ( unsigned int i = 0; i < comm_cs.mutex_tokens.size(); i++ ) {
    if ( !comm_cs.mutex_tokens[i] ) {
      req_cnt++;
      outgoing.dest_ID = i;
      comm_send( outgoing );
    }
  }

  curr_request.msg_count = req_cnt * 2;   // total exchanged = requests + replies (should receive 1 rep for each req)
  curr_request.seq_num = comm_cs.seq_num; // sequence number of request made

  comm_cs.shared_vars.unlock();

  // If no requests were made, don't wait on any replies
  if ( req_cnt > 0 ) {
    comm_cs.entry_ok.wait( comm_cs.entry_ok_mutex );
  }

  // Record grant time of request
  /*gettimeofday(&end,NULL);

  timeval res;
  timersub(&end, &start, &res);*/
  timer.stopTimer();

  // Record the request blocked/waiting time in seconds
  //curr_request.wait_time = ( res.tv_sec + res.tv_usec / 1000000.0 );
  curr_request.wait_time = timer.getElapsedTime();

  cout << "cs log entry: " << "seq_num " << curr_request.seq_num
       << " wait_time = " << curr_request.wait_time
       << " msg_count = " << curr_request.msg_count << endl;

  comm_cs_log.push( curr_request );

  return 0;
}

int comm_release_cs() {
  comm_cs.shared_vars.lock();
  comm_cs.requesting_cs = false;

  msg_t outgoing;
  outgoing.msg_type = MSG_T_REPLY_CS;
  outgoing.ts       = comm_cs.seq_num;

  // Send deferred replies
  for ( unsigned int i = 0; i < comm_cs.rep_deferred.size(); i++ ) {
    if ( comm_cs.rep_deferred[i] ) {
      outgoing.dest_ID = i;
      comm_cs.mutex_tokens[i] = false;
      comm_send( outgoing );
    }
  }

  comm_cs.shared_vars.unlock();

  return 0;
}