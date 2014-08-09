/*
  @fn  comm_open_connection_once
  @des Attempts to open a socket connection to the address specified once
  @par address -- the string denoting the address to connect to
  @ret The socket file descriptor of the opened socket or < 0 if unsuccessful

  @fn  comm_close_connection
  @des Closes the socket associated with the node with ID n
  @par n -- the ID of the node whose connection is to be closed
  @ret < 0 if unsuccessful close
 */

#include "communication_server.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <algorithm>

#include "str_manip.h"
#include "globals.h"
#include "hr_time.h"
#include "thread_constructs.h"
#include "distributed_file.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::map;
using std::vector;
using std::list;

// The threads for the communication server
listen_thread_t     comm_tcp_listen_thread;
recv_thread_list_t  comm_tcp_recv_threads;

listen_thread_t     comm_udp_recv_thread;

thread_t            comm_recv_parse_thread;

thread_t            comm_send_thread;

// Queues for message passing between threads
msg_queue_t comm_send_q;
msg_queue_t comm_recv_q;

// Port and contacts to communicate with
int               comm_port = -1;
contact_list_t    comm_contacts;

// The distributed file
distributed_file comm_file;

// Node IDs currently reachable from this node
set<unsigned int> comm_reachability_list;
vn_list_t comm_vns;
distinguished_t comm_distinguished;

// For barriers
barrier_list_t comm_barriers;
NaiveAtomicNumber<unsigned int> comm_barrier_count;

// For votes
barrier_t comm_votes;
barrier_t comm_catchup;

// For send/recv status
bool comm_up = true;

// Hidden functions from the user!
int comm_open_tcp_connection_once( const node_contact_t& contact );
int comm_open_tcp_connection_until( const node_contact_t& contact );
int comm_open_udp_connection_once( const node_contact_t& contact );
int comm_open_udp_connection_until( const node_contact_t& contact );
int comm_bind_once( int port, int type );

void* comm_tcp_listen_thread_fun( void* data );
void* comm_tcp_recv_thread_fun( void* data );
void* comm_udp_recv_thread_fun( void* data );
void* comm_recv_parse_thread_fun( void* data );
void* comm_send_thread_fun( void* data );

// Handling of incoming messages
void comm_handle_write( const msg_t& incoming );
void comm_handle_read_req( const msg_t& incoming );
void comm_handle_read_rep( const msg_t& incoming );
void comm_handle_node_down( const msg_t& incoming );
void comm_handle_node_up( const msg_t& incoming );
void comm_handle_halt( const msg_t& incoming );
void comm_handle_abort( const msg_t& incoming );
void comm_handle_commit( const msg_t& incoming );
void comm_handle_vote_req( const msg_t& incoming );
void comm_handle_vote_rep( const msg_t& incoming );
void comm_handle_catchup_req( const msg_t& incoming );
void comm_handle_catchup_rep( const msg_t& incoming );
void comm_handle_barrier( const msg_t& incoming );
void comm_handle_unreachable( const msg_t& incoming );
void comm_handle_reachable( const msg_t& incoming );

// For the voting algorithm
int comm_collect_votes();
bool is_distinguished();
int comm_catch_up();
int comm_do_update();
unsigned int comm_get_max_vn();
unsigned int comm_compute_max_vn();

// Packing for msg sending/recving
void comm_pack_distributed_file_entry( const distributed_file_entry_t& entry, msg_t& msg );
void comm_unpack_distributed_file_entry ( const msg_t& msg, distributed_file_entry_t& entry );

// some prints
void comm_print_vns();
void comm_print_reachability_list();

// Communication server setup
int comm_start( const char* config_file_path ) {
  int error = 0;

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

    // Read coordinator ID
    std::getline( config_file, line );
    NODE_NETWORK_MASTER = str_to_uint( line );

    // Contact to add after each read from file
    node_contact_t contact;

    // All new contacts have no open sockets, so -1
    contact.sockfd = -1;

    // No nodes in the network yet
    NODE_NETWORK_SIZE = 0;

    // Read <node ID, node address>
    while ( config_file.good() ) {
      // Read node ID
      std::getline( config_file, line );
      contact.id = str_to_uint( line );

      // Read node address
      std::getline( config_file, line );
      contact.address = line;

      // Use TCP to talk to master, use UDP elsewise
      contact.type = ( contact.id == NODE_NETWORK_MASTER ? SOCK_STREAM : SOCK_DGRAM );

      error = comm_add_contact( contact );
      if ( error != 0 ) {
        cout << "comm_start Couldn't add contact "
          << contact.id << "@" << contact.address << endl;
        return error;
      }

      NODE_NETWORK_SIZE++;
    }
    
    // print_contact_list( comm_contacts );

    // The subnet of udp servers is all of th eservers minus the one master
    NODE_SUBNET_SIZE = NODE_NETWORK_SIZE - 1;

    // Finished reading from file
    config_file.close();

    // Initialize this node's vn info
    comm_vns[NODE_INFO.id].vn = 0;
    comm_vns[NODE_INFO.id].ru = NODE_SUBNET_SIZE;
    strncpy( comm_vns[NODE_INFO.id].text, "", sizeof("") );

    comm_print_vns();
    
    // Initialize the distributed file
    comm_file.path = "replica_" + uint_to_str( NODE_INFO.id ) + ".txt";
    comm_file.delimiters = "|";

    comm_file.print();
    
    // Initialize the distinguished object
    comm_distinguished.m = 0;
    comm_distinguished.n = 0;

    // Initialize the reachability list to all UDP nodes
    for ( contact_list_t::iterator it = comm_contacts.begin();
          it != comm_contacts.end();
          it++ ) {
      if ( it->first != NODE_NETWORK_MASTER ) {
        comm_reachability_list.insert( it->first );
      }
    }
    
    comm_print_reachability_list();

    // all other attrs default to OK values

    // Spawn tcp listener thread
    error = pthread_create( &(comm_tcp_listen_thread.id),
                            NULL,
                            comm_tcp_listen_thread_fun,
                            NULL );
    if ( error != 0 ) {
      cout << "comm_start Couldn't create TCP listen thread" << endl;
      return error;
    }
    comm_tcp_listen_thread.state = T_STATE_ACTIVE;

    // Spawn UDP receiver thread
    error = pthread_create( &(comm_udp_recv_thread.id),
                            NULL, 
                            comm_udp_recv_thread_fun,
                            NULL );
    if ( error != 0 ) {
      cout << "comm_start Couldn't create UDP recveive thread" << endl;
      return error;
    }
    comm_udp_recv_thread.state = T_STATE_ACTIVE;                        
    
    // Spawn receive parser thread
    error = pthread_create( &(comm_recv_parse_thread.id),
                            NULL,
                            comm_recv_parse_thread_fun,
                            NULL );
    if ( error != 0 ) {
      cout << "comm_start Couldn't create receive parse thread" << endl;
      return error;
    }
    comm_recv_parse_thread.state = T_STATE_ACTIVE;

    // Open a connection to all remote addresses previously added using an
    // comm_add_contact() call
    error = comm_initiate_contacts();
    if ( error != 0 ) {
      cout << "comm_start Couldn't connect to all remote addresses" << endl;
      return error;
    }
    
    // print_contact_list( comm_contacts );    

    // Spawn sender thread
    error = pthread_create( &(comm_send_thread.id),
                            NULL,
                            comm_send_thread_fun,
                            NULL );
    if ( error != 0 ) {
      cout << "comm_start Couldn't create send thread" << endl;
      return error;
    }
    comm_send_thread.state = T_STATE_ACTIVE;

    error = comm_barrier();
    if ( error != 0 ) {
      cout << "comm_start Couldn't join the network" << endl;
    }
  }

  else {
    error = -1;
    cout << "comm_start Couldn't open config file" << endl;
  }

  cout << "Initial node configuration: " << endl;
  print_contact_list( comm_contacts ); 
  
  // msg_t test = create_msg( MSG_T_FINAL, 0, 0, 0, "" );
  msg_t test;
  test.header = MSG_T_FINAL;
  comm_broadcast( test );

  return error;
}

int comm_add_contact( const node_contact_t& contact ) {
  int error = 0;
  
  comm_contacts[contact.id] = contact;

  return error;
}

int comm_delete_contact( unsigned int id ) {
  int error = 0;

  comm_close_connection( comm_contacts[id] );
  
  error = comm_contacts.erase( id );

  return error;
}

int comm_open_connection_once( const node_contact_t& contact ) {
  if ( contact.type == SOCK_STREAM ) {
    return comm_open_tcp_connection_once( contact );
  }
  
  else if ( contact.type == SOCK_DGRAM ) {
    return comm_open_udp_connection_once( contact );
  }
  
  else {
    cout << "comm_open_connection_once Unknown protocol " << contact.type << endl;
    return -1;
  }
}

int comm_open_connection_until( const node_contact_t& contact ) {
  if ( contact.type == SOCK_STREAM ) {
    return comm_open_tcp_connection_until( contact );
  }
  
  else if ( contact.type == SOCK_DGRAM ) {
    return comm_open_udp_connection_until( contact );
  }
  
  else {
    cout << "comm_open_connection_until Unknown protocol " << contact.type << endl;
    return -1;
  }
}

int comm_open_tcp_connection_once( const node_contact_t& contact ) {
  int error = 0;
  contact_list_t::iterator node = comm_contacts.find( contact.id );

  if ( node == comm_contacts.end() ) {
    cout << "comm_open_tcp_connection_once No known id " << contact.id << endl;
    error = -1;
  }

  else if ( node->second.address.compare( contact.address ) != 0 ) {
    cout << "comm_open_tcp_connection_once Mismatched address for supplied contact and stored contact" << endl;
    error = -2;
  }

  else {
    struct addrinfo hints, *server_info, *p;
    string comm_port_str = int_to_str( comm_port );

    memset( &hints, 0, sizeof(hints) );
    hints.ai_family   = AF_UNSPEC;         // IPv4 or IPv6 don't care
    hints.ai_socktype = SOCK_STREAM;       // TCP

    // Get address info of remote address
    error = getaddrinfo( node->second.address.c_str(), comm_port_str.c_str(), &hints, &server_info );
    if ( error != 0 ) {
      ::freeaddrinfo( server_info );
      cout << "comm_open_tcp_connection_once Couldn't get info for remote address "
        << node->second.address << endl;
      return error;
    }

    // loop through all the results and connect to the first we can
    for( p = server_info; p != NULL; p = p->ai_next ) {
      node->second.sockfd = ::socket( p->ai_family, p->ai_socktype, p->ai_protocol );
      if ( node->second.sockfd == -1 ) {
        continue;
      }

      if ( ::connect( node->second.sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) {
        continue;
      }

      break;
    }

    ::freeaddrinfo( server_info );

    if ( p == NULL ) {
      error = -1;
      cout << "comm_open_tcp_connection_once Couldn't connect to remote address "
        << node->second.address << endl;
      return error;
    }
  }

  return error;
}

int comm_open_tcp_connection_until( const node_contact_t& contact ) {
  int error = 0;
  contact_list_t::iterator node = comm_contacts.find( contact.id );

  if ( node == comm_contacts.end() ) {
    cout << "comm_open_tcp_connection_until No known id " << contact.id << endl;
    error = -1;
  }

  else if ( node->second.address.compare( contact.address ) != 0 ) {
    cout << "comm_open_tcp_connection_until Mismatched address for supplied contact and stored contact" << endl;
    error = -2;
  }

  else {
    struct addrinfo hints, *server_info, *p;
    string comm_port_str = int_to_str( comm_port );

    memset( &hints, 0, sizeof(hints) );
    hints.ai_family   = AF_UNSPEC;         // IPv4 or IPv6 don't care
    hints.ai_socktype = SOCK_STREAM;       // TCP 

    do {
      error = 0;

      // Get address info of remote address
      error = getaddrinfo( node->second.address.c_str(), comm_port_str.c_str(), &hints, &server_info );
      if ( error != 0 ) {
        cout << "comm_open_tcp_connection_until Couldn't get info for remote address "
          << node->second.address << endl;
          wait_s_duration( 1 );
      }
    } while ( error != 0 );

    do {
      error = 0;

      // loop through all the results and connect to the first we can
      for( p = server_info; p != NULL; p = p->ai_next ) {
        node->second.sockfd = ::socket( p->ai_family, p->ai_socktype, p->ai_protocol );
        if ( node->second.sockfd == -1 ) {
          continue;
        }

        if ( ::connect( node->second.sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) {
          continue;
        }

        break;
      }

      if ( p == NULL ) {
        error = -1;
        cout << "comm_open_tcp_connection_until Couldn't connect to remote address "
          << node->second.address << endl;
        wait_s_duration( 1 );
      }
    } while ( error != 0 );

    ::freeaddrinfo( server_info );
  }

  return error;
}

int comm_open_udp_connection_once( const node_contact_t& contact ) {
  int error = 0;
  contact_list_t::iterator node = comm_contacts.find( contact.id );

  if ( node == comm_contacts.end() ) {
    cout << "comm_open_udp_connection_once No known id " << contact.id << endl;
    error = -1;
  }

  else if ( node->second.address.compare( contact.address ) != 0 ) {
    cout << "comm_open_udp_connection_once Mismatched address for supplied contact and stored contact" << endl;
    error = -2;
  }

  else {
    struct addrinfo hints, *p;
    string comm_port_str = int_to_str( comm_port );

    memset( &hints, 0, sizeof(hints) );
    hints.ai_family   = AF_UNSPEC;         // IPv4 or IPv6 don't care
    hints.ai_socktype = SOCK_DGRAM;        // UDP 

    // Get address info of remote address
    error = getaddrinfo( node->second.address.c_str(), comm_port_str.c_str(), &hints, &(node->second.server_info) );
    if ( error != 0 ) {
      cout << "comm_open_udp_connection_once Couldn't get info for remote address "
        << node->second.address << endl;
      return error;
    }

    // loop through all the results and connect to the first we can
    for( p = node->second.server_info; p != NULL; p = p->ai_next ) {
      node->second.sockfd = ::socket( p->ai_family, p->ai_socktype, p->ai_protocol );
      if ( node->second.sockfd != -1 ) {
        break;
      }
    }

    if ( p == NULL ) {
      error = -1;
      cout << "comm_open_udp_connection_once Couldn't connect to remote address "
        << node->second.address << endl;
    }
  }

  return error;
}

int comm_open_udp_connection_until( const node_contact_t& contact ) {
  int error = 0;
  contact_list_t::iterator node = comm_contacts.find( contact.id );

  if ( node == comm_contacts.end() ) {
    cout << "comm_open_udp_connection_until No known id " << contact.id << endl;
    error = -1;
  }

  else if ( node->second.address.compare( contact.address ) != 0 ) {
    cout << "comm_open_udp_connection_until Mismatched address for supplied contact and stored contact" << endl;
    error = -2;
  }

  else {
    struct addrinfo hints, *p;
    string comm_port_str = int_to_str( comm_port );

    memset( &hints, 0, sizeof(hints) );
    hints.ai_family   = AF_UNSPEC;         // IPv4 or IPv6 don't care
    hints.ai_socktype = SOCK_DGRAM;        // UDP 

    do {
      error = 0;

      // Get address info of remote address
      error = getaddrinfo( node->second.address.c_str(), comm_port_str.c_str(), &hints, &(node->second.server_info) );
      if ( error != 0 ) {
        cout << "comm_open_udp_connection_until Couldn't get info for remote address "
          << node->second.address << endl;
          wait_s_duration( 1 );
      }
    } while ( error != 0 );

    do {
      error = 0;

      // loop through all the results and connect to the first we can
      for( p = node->second.server_info; p != NULL; p = p->ai_next ) {
        node->second.sockfd = ::socket( p->ai_family, p->ai_socktype, p->ai_protocol );
        if ( node->second.sockfd != -1 ) {
          break;
        }
      }

      if ( p == NULL ) {
        error = -1;
        cout << "comm_open_udp_connection_until Couldn't connect to remote address "
          << node->second.address << endl;
        wait_s_duration( 1 );
      }
    } while ( error != 0 );
  }

  return error;
}

int comm_close_connection( const node_contact_t& contact ) {
  int error = 0;
  contact_list_t::iterator node = comm_contacts.find( contact.id );

  if ( node == comm_contacts.end() ) {
    cout << "comm_close_connection No known id " << contact.id << endl;
    error = -1;
  }

  else if ( node->second.address.compare( contact.address ) != 0 ) {
    cout << "comm_close_connection Mismatched address for supplied contact and stored contact" << endl;
    error = -2;
  }

  else {
    ::close( node->second.sockfd );
    node->second.sockfd = -1;
  }

  return error;
}

int comm_initiate_contacts() {
  int error = 0;

  for ( contact_list_t::iterator node = comm_contacts.begin();
        node != comm_contacts.end();
        node++ ) {
    // cout << "attempting to open connection to" << endl;
    // print_contact( node->second );
    
    switch ( node->second.type ) {
      case SOCK_STREAM:
        error = comm_open_tcp_connection_until( node->second );
        break;
      
      case SOCK_DGRAM:
        error = comm_open_udp_connection_until( node->second );
        break;
      
      default:
        cout << "Unknown socket type for contact ";
        print_contact( node->second );
    }
    
    if ( error != 0 ) {
      cout << "comm_initiate_contacts Couldn't connect to contact";
      print_contact( node->second );
      break;
    }
  }

  return error;
}

int comm_bind_once( int port, int type ) {
  int sockfd = -1, yes = 1, error = 0;
  struct addrinfo hints, *server_info, *p;
  string comm_port_str = int_to_str( comm_port );

  memset( &hints, 0, sizeof(hints) );
  hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6 don't care
  hints.ai_socktype = type;        // use specified type TCP/UDP
  hints.ai_flags    = AI_PASSIVE;  // Use this IP

  error = getaddrinfo( NULL, comm_port_str.c_str(), &hints, &server_info );
  if ( error != 0 ) {
    ::freeaddrinfo( server_info );
    cout << "comm_bind_once Couldn't get info for local address" << endl;
    return error;
  }

  // loop through all the results and connect to the first we can
  for( p = server_info; p != NULL; p = p->ai_next ) {
    sockfd = ::socket( p->ai_family, p->ai_socktype, p->ai_protocol );
    if ( sockfd == -1 ) {
      continue;
    }

    if ( ::setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes) ) == -1 ) {
      // ?
    }

    if ( ::bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) {
      continue;
    }

    break;
  }

  ::freeaddrinfo( server_info );

  if ( p == NULL ) {
    error = -1;
    cout << "comm_bind_once Couldn't bind to port " << port << endl;
  }

  return sockfd;
}

// Thread entry functions
void* comm_tcp_listen_thread_fun( void* data ) {
  int newfd = -1, error = 0;
  struct sockaddr_storage remote_addr;
  socklen_t sin_size = sizeof(remote_addr);

  comm_tcp_listen_thread.sockfd = comm_bind_once( comm_port, SOCK_STREAM ); 
  if ( comm_tcp_listen_thread.sockfd < 0 ) {
    cout << "comm_tcp_listen_thread_fun Couldn't bind to port " << comm_port << endl;
    return NULL;
  }
  
  error = ::listen( comm_tcp_listen_thread.sockfd, 15 );
  if ( error == -1 ) {
    cout << "comm_tcp_listen_thread_fun Couldn't listen to port " << comm_port << endl;
    return NULL;
  }

  // Accept loop
  // Communication hasn't been stopped
  while ( comm_tcp_listen_thread.state == T_STATE_ACTIVE ) {
    newfd = ::accept( comm_tcp_listen_thread.sockfd, (struct sockaddr*)&remote_addr, &sin_size );
    if ( newfd == -1 ) {
      cout << "comm_tcp_listen_thread_fun Couldn't accept new connection" << endl;
      continue;
    }
    
    if ( !comm_up ) {
      cout << "comm_tcp_listen_thread_fun Comms are down, new connection terminated" << endl;
      ::close( newfd );
      continue;
    }

    // Spawn receiver thread and store sockfd and pthread_t id for it in the
    // comm_recv_thread_ids list
    recv_thread_t new_thread;
    new_thread.sockfd = newfd;
    new_thread.state = T_STATE_ACTIVE;

    error = pthread_create( &(new_thread.id),
                            NULL,
                            comm_tcp_recv_thread_fun,
                            NULL
                          );

    if ( error != 0 ) {
      ::close( newfd );
      cout << "comm_tcp_listen_thread_fun Couldn't create receive thread" << endl;
    }

    comm_tcp_recv_threads.mutex.lock();
    comm_tcp_recv_threads.thread_list.push_back( new_thread );
    comm_tcp_recv_threads.mutex.unlock();
  }

  ::close( comm_tcp_listen_thread.sockfd );

  return NULL;
}

void* comm_tcp_recv_thread_fun( void* data ) {
  list<recv_thread_t>::iterator my_info;
  char      incoming_buf[512];
  msg_t*    incoming;

  wait_ms_duration( 100 );

  // Locate this recv thread's info in the list
  bool found = false;
  do {
    comm_tcp_recv_threads.mutex.lock();

    for ( my_info = comm_tcp_recv_threads.thread_list.begin();
          my_info != comm_tcp_recv_threads.thread_list.end();
          my_info++ ) {
      if ( pthread_equal( my_info->id, pthread_self() ) ) {
        found = true;
        break;
      }
    }

    comm_tcp_recv_threads.mutex.unlock();
  } while ( !found );

  // Receive loop
  // Communication hasn't been stopped
  while ( my_info->state == T_STATE_ACTIVE ) {
    int bytes = ::recv( my_info->sockfd, incoming_buf, sizeof(incoming_buf), 0 );
    if ( bytes != sizeof(msg_t) ) {
      cout << "comm_tcp_recv_thread_fun DID NOT RECEIVE ENTIRE MESSAGE" << endl;
    }

    // Normal receive
    if ( bytes > 0 ) {
      incoming = reinterpret_cast<msg_t*>(incoming_buf);
      comm_recv_q.push( *incoming );
    }

    // Connection closed from remote end
    else {
      cout << "comm_tcp_recv_thread_fun Terminating receive thread" << endl;
      ::close( my_info->sockfd );
      comm_close_connection( comm_contacts[incoming->send_id] );
      my_info->state = T_STATE_STOP;
    }
  }

  // Remove this thread from the recv_thread_ids
  comm_tcp_recv_threads.mutex.lock();
  comm_tcp_recv_threads.thread_list.erase( my_info );
  comm_tcp_recv_threads.mutex.unlock();

  return NULL;
}

void* comm_udp_recv_thread_fun( void* data ) {
  int newfd = -1, error = 0;
  struct sockaddr remote_addr;
  socklen_t sin_size = sizeof(remote_addr);
  char      incoming_buf[512];
  msg_t*    incoming;

  comm_udp_recv_thread.sockfd = comm_bind_once( comm_port, SOCK_DGRAM ); 
  if ( comm_udp_recv_thread.sockfd < 0 ) {
    cout << "comm_udp_recv_thread_fun Couldn't bind to port " << comm_port << endl;
    return NULL;
  }

  // Recvfrom loop
  // Communication hasn't been stopped
  while ( comm_udp_recv_thread.state == T_STATE_ACTIVE ) {
    int bytes = ::recvfrom( comm_udp_recv_thread.sockfd, incoming_buf, sizeof(incoming_buf), 0,
                            &remote_addr, &sin_size );
    if ( bytes != sizeof(msg_t) ) {
      cout << "comm_udp_recv_thread_fun DID NOT RECEIVE ENTIRE MESSAGE" << endl;
    }

    // Normal receive
    if ( bytes > 0 ) {
      incoming = reinterpret_cast<msg_t*>(incoming_buf);
      comm_recv_q.push( *incoming );
    }

    // Connection closed from remote end
    else {
      cout << "comm_udp_recv_thread_fun Terminating receive thread" << endl;
      ::close( comm_udp_recv_thread.sockfd );
      comm_udp_recv_thread.state = T_STATE_STOP;
    }
  }

  return NULL;
}

void* comm_recv_parse_thread_fun( void* data ) {
  msg_t incoming;

  // Parse loop
  // Communication hasn't been stopped
  while ( comm_recv_parse_thread.state == T_STATE_ACTIVE ) {
    incoming = comm_recv();
    comm_print_recv_msg( incoming );
    if ( !comm_up ) {
      cout << "comm_recv_parse_thread Comms are down, message discarded" << endl;    
      continue;
    }

    switch ( incoming.header ) {
      case MSG_T_WRITE:
        comm_handle_write( incoming );
        break;

      case MSG_T_READ_REQ:
        comm_handle_read_req( incoming );
        break;

      case MSG_T_READ_REP:
        comm_handle_read_rep( incoming );
        break;

      case MSG_T_NODE_DOWN:
        comm_handle_node_down( incoming );
        break;

      case MSG_T_NODE_UP:
        comm_handle_node_up( incoming );
        break;

      case MSG_T_HALT:
        // halt other threads
        comm_handle_halt( incoming );

        // halt self
        return NULL;

      case MSG_T_ABORT:
        comm_handle_abort( incoming );
        break;

      case MSG_T_COMMIT:
        comm_handle_commit( incoming );
        break;

      case MSG_T_VOTE_REQ:
        comm_handle_vote_req( incoming );
        break;

      case MSG_T_VOTE_REP:
        comm_handle_vote_rep( incoming );
        break;
        
      case MSG_T_CATCHUP_REQ:
        comm_handle_catchup_req( incoming );
        break;
      
      case MSG_T_CATCHUP_REP:
        comm_handle_catchup_rep( incoming );
        break;

      case MSG_T_BARRIER:
        comm_handle_barrier( incoming );
        break;
        
      case MSG_T_UNREACHABLE:
        comm_handle_unreachable( incoming );
        break;
        
      case MSG_T_REACHABLE:
        comm_handle_reachable( incoming );
        break;

      case MSG_T_FINAL:
        break;
        
      default:
        cout << "comm_recv_parse_thread Unknown message header " << incoming.header << endl;
        break;
    }
  }

  return NULL;
}

void* comm_send_thread_fun( void* data ) {
  msg_t outgoing;
  int error = 0;

  // Send loop
  // Communication hasn't been stopped
  while ( comm_send_thread.state == T_STATE_ACTIVE ) {
    comm_send_q.wait_and_pop( outgoing );

    if ( outgoing.header == MSG_T_HALT ) {
      break;
    }
    
    if ( !comm_up && (outgoing.header != MSG_T_UNREACHABLE) ) {
      cout << "comm_send_thread_fun Comms are down, message not sent" << endl;
      continue;
    }

    // Check to make sure outgoing destination is a known location and
    // a socket is open for sending
    contact_list_t::iterator contact = comm_contacts.find( outgoing.dest_id );
    if ( contact == comm_contacts.end() ) {
      cout << "comm_send_thread_fun Unknown destination ID " << outgoing.dest_id << endl;
      continue;
    }

    else if ( contact->second.sockfd < 0 ) {
      cout << "comm_send_thread_fun No open connection to destination ID " << outgoing.dest_id << endl;
      continue;
    }

    comm_print_send_msg( outgoing );

    if ( contact->second.type == SOCK_STREAM ) {
      error = ::send( contact->second.sockfd, &outgoing, sizeof(outgoing), 0 );
      if ( error == -1 ) {
        cout << "comm_send_thread_fun Couldn't send tcp message" << endl;
      }
    }
    
    else if ( contact->second.type == SOCK_DGRAM ) {
      error = ::sendto( contact->second.sockfd, &outgoing, sizeof(outgoing), 0, 
        contact->second.server_info->ai_addr, contact->second.server_info->ai_addrlen );
      if ( error == -1 ) {
        cout << "comm_send_thread_fun Couldn't send udp message" << endl;
      }
    }
    
    else {
      cout << "comm_send_thread_fun Unknown socket type " << contact->second.type << endl;
    }
  }

  return NULL;
}

void comm_handle_write( const msg_t& incoming ) {
  int error = 0;
  // msg_t outgoing = create_msg( MSG_T_FINAL, 0, 0, 0, "" );
  msg_t outgoing;
  
  // comm_file.lock();

  error = comm_collect_votes();
  if ( error != 0 ) {
    cout << "comm_handle_write Failed to collect votes, aborting" << endl;
    // comm_file.unlock();
    outgoing.header = MSG_T_ABORT;
    outgoing.dest_id = NODE_NETWORK_MASTER;
    comm_send( outgoing );
    return;
  }

  if ( is_distinguished() && (comm_file.get_vn() < comm_get_max_vn()) ) {
    error = comm_catch_up();
    if ( error != 0 ) {
      cout << "comm_handle_write Failed to catch up, aborting" << endl;
      // comm_file.unlock();
      outgoing.header = MSG_T_ABORT;
      outgoing.dest_id = NODE_NETWORK_MASTER;
      comm_send( outgoing );
      return;
    }
  }

  else {
    // comm_file.unlock();
    outgoing.header = MSG_T_ABORT;
    outgoing.dest_id = NODE_NETWORK_MASTER;
    comm_send( outgoing );
    return;
  }

  error = comm_do_update();
  if ( error != 0 ) {
    cout << "comm_handle_write Failed to do update, aborting" << endl;
    // comm_file.unlock();
    outgoing.header = MSG_T_ABORT;
    outgoing.dest_id = NODE_NETWORK_MASTER;
    comm_send( outgoing );
    return;
  }

  comm_file.append( comm_vns[NODE_INFO.id] );

  outgoing.header = MSG_T_COMMIT;
  comm_pack_distributed_file_entry( comm_vns[NODE_INFO.id], outgoing );

  for ( set<unsigned int>::iterator it = comm_reachability_list.begin();
        it != comm_reachability_list.end();
        it++ ) {
    if ( *it == NODE_INFO.id ) {
      continue;
    }
    outgoing.dest_id = *it;
    comm_send( outgoing );
  }

  // comm_file.unlock();
}

void comm_handle_read_req( const msg_t& incoming ) {
  int error = 0;
  // msg_t outgoing = create_msg( MSG_T_FINAL, 0, 0, 0, "" );
  msg_t outgoing;

  // comm_file.lock();

  error = comm_collect_votes();
  if ( error != 0 ) {
    cout << "comm_handle_read_req Failed to collect votes, aborting" << endl;
    // comm_file.unlock();
    outgoing.header = MSG_T_ABORT;
    outgoing.dest_id = NODE_NETWORK_MASTER;
    comm_send( outgoing );
    return;
  }

  if ( is_distinguished() && (comm_file.get_vn() < comm_get_max_vn()) ) {
    error = comm_catch_up();
    if ( error != 0 ) {
      cout << "comm_handle_read_req Failed to catch up, aborting" << endl;
      // comm_file.unlock();
      outgoing.header = MSG_T_ABORT;
      outgoing.dest_id = NODE_NETWORK_MASTER;
      comm_send( outgoing );
      return;
    }
  }

  else {
    // comm_file.unlock();
    outgoing.header = MSG_T_ABORT;
    outgoing.dest_id = NODE_NETWORK_MASTER;
    comm_send( outgoing );
    return;
  }

  outgoing.header = MSG_T_READ_REP;
  outgoing.dest_id = incoming.send_id;
  comm_pack_distributed_file_entry( comm_file.get_last_entry(), outgoing );
  comm_send( outgoing );

  // comm_file.unlock();
}

void comm_handle_read_rep( const msg_t& incoming ) {
  // unpack the message and output it to the screen
  distributed_file_entry_t unpacked;
  
  comm_unpack_distributed_file_entry( incoming, unpacked );
  
  cout << "Read:" << endl;
  print_distributed_file_entry( unpacked );
}

void comm_handle_node_down( const msg_t& incoming ) {
  comm_up = false;

  // msg_t outgoing;
  // outgoing.header = MSG_T_UNREACHABLE;
  // comm_broadcast( outgoing );
}

void comm_handle_node_up( const msg_t& incoming ) {
  comm_up = true;
  
  // msg_t outgoing = create_msg( MSG_T_FINAL, 0, 0, 0, "" );;
  msg_t outgoing;
  outgoing.header = MSG_T_REACHABLE;
  comm_broadcast( outgoing );

  // Get up to date if need be
  if ( is_distinguished() && comm_file.get_vn() < comm_get_max_vn() ) {
    // comm_file.lock();
    int error = comm_catch_up();
    if ( error != 0 ) {
      cout << "comm_handle_node_up Failed to catch up" << endl;
    }
    // comm_file.unlock();
  }
}

void comm_handle_halt( const msg_t& incoming ) {
  comm_stop();

  exit(0);
}

void comm_handle_abort( const msg_t& incoming ) {
  cout << "Aborting request. Abort caused by " << incoming.send_id << endl;
}

void comm_handle_commit( const msg_t& incoming ) {
  comm_unpack_distributed_file_entry( incoming, comm_vns[NODE_INFO.id] );
  
  cout << "Committing: " << endl;
  print_distributed_file_entry( comm_vns[NODE_INFO.id] );

  // comm_file.lock();

  comm_file.append( comm_vns[NODE_INFO.id] );

  // comm_file.unlock();
}

void comm_handle_vote_req( const msg_t& incoming ) {
  msg_t outgoing;

  outgoing.header = MSG_T_VOTE_REP;
  outgoing.dest_id = incoming.send_id;
  comm_pack_distributed_file_entry( comm_vns[NODE_INFO.id], outgoing );
  // TODO does this need to be comm_file.get_last_entry()?
  
  comm_send( outgoing );
}

void comm_handle_vote_rep( const msg_t& incoming ) {
  comm_unpack_distributed_file_entry( incoming, comm_vns[incoming.send_id] );
  
  cout << "Vote reply: " << endl;
  print_distributed_file_entry( comm_vns[incoming.send_id] );

  // don't count votes from network master!
  if ( comm_votes.count.get() == comm_votes.expected.get() ) {
    comm_votes.cv.broadcast();
  }
}

void comm_handle_catchup_req( const msg_t& incoming ) {
  int error = 0;

  // msg_t outgoing = create_msg( MSG_T_FINAL, 0, 0, 0, "" );;
  msg_t outgoing;
  outgoing.header = MSG_T_CATCHUP_REP;
  outgoing.dest_id = incoming.send_id;
  
  // get entries from the file using fun stuff
  // comm_file.lock();
  
  vector<distributed_file_entry_t> replies = comm_file.get_entries( incoming.data, comm_file.get_vn() );
  
  for ( unsigned int i = 0; i < replies.size(); i++ ) {
    comm_pack_distributed_file_entry( replies[i], outgoing );
    error = comm_send( outgoing );
    if ( error != 0 ) {
      cout << "comm_handle_catchup_req Could not send message" << endl;
    }
  } 
  
  // comm_file.unlock();
}

void comm_handle_catchup_rep( const msg_t& incoming ) {
  int error = 0;
  
  distributed_file_entry_t entry;
  comm_unpack_distributed_file_entry( incoming, entry );
  
  cout << "Catching up with: " << endl;
  print_distributed_file_entry( entry );
  
  error = comm_file.append( entry );
  if ( error != 0 ) {
    cout << "comm_handle_catchup_rep Could not append" << endl;
  }
  
  if ( entry.vn == comm_get_max_vn() ) {
    cout << "comm_handle_catchup_rep Caught up!" << endl;
    comm_catchup.cv.broadcast();
  }  
}

void comm_handle_barrier( const msg_t& incoming ) {
  /*comm_barriers[incoming.data].count.inc();

  if ( comm_barriers[incoming.data].count.get() == comm_barriers[incoming.data].expected.get() ) {
    comm_barriers[incoming.data].cv.broadcast();
  }*/
}

void comm_handle_unreachable( const msg_t& incoming ) {
  // remove node from reachability list
  comm_reachability_list.erase( incoming.send_id );
}

void comm_handle_reachable( const msg_t& incoming ) {
  // add node to reachability list
  comm_reachability_list.insert( incoming.send_id );
}

// Server interaction functions
int comm_send( msg_t msg ) {
  msg.send_id = NODE_INFO.id;
  comm_send_q.push( msg );

  return 0;
}

int comm_broadcast( msg_t msg ) {
  int error = 0;

  for ( contact_list_t::iterator it = comm_contacts.begin();
        it != comm_contacts.end();
        it++ ) {
    msg.dest_id = it->first;
    error = comm_send( msg );
    if ( error < 0 ) {
      cout << "comm_broadcast Couldn't send to id " << msg.dest_id << endl;
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
  /*int error = 0;

  // Mark the next barrier as started and add to ongoing barriers list
  unsigned int barrier_number = comm_barrier_count.inc();
  barrier_t barrier;
  comm_barriers[barrier_number] = barrier;

  // Broadcast to other nodes that this node hit a barrier
  msg_t barrier_msg;
  barrier_msg.data = barrier_number;
  barrier_msg.header = MSG_T_BARRIER;

  error = comm_broadcast( barrier_msg );
  if ( error != 0 ) {
    cout << "comm_barrier Couldn't broadcast barrier number " << barrier_number << endl;
    return error;
  }

  else {
    comm_barriers[barrier_number].expected.set( NODE_NETWORK_SIZE );
  }

  if ( comm_barriers[barrier_number].count.get() < comm_barriers[barrier_number].expected.get() ) {
    comm_barriers[barrier_number].cv.wait( comm_barriers[barrier_number].mutex );
  }

  comm_barriers.erase( barrier_number );

  return error;*/
  cout << "BARRIER" << endl;
  getchar();
  return 0;
}

int comm_stop() {
  comm_tcp_recv_threads.mutex.lock();

  // Mark all threads STOP
  comm_tcp_listen_thread.state = T_STATE_STOP;
  comm_udp_recv_thread.state = T_STATE_STOP;
  comm_recv_parse_thread.state = T_STATE_STOP;
  comm_send_thread.state = T_STATE_STOP;
  for ( list<recv_thread_t>::iterator it = comm_tcp_recv_threads.thread_list.begin();
        it != comm_tcp_recv_threads.thread_list.end();
        it++ ) {
    it->state = T_STATE_STOP;
  }

  // Close sockets threads are working with.
  // This gets the receiver threads and the listener thread off the blocking
  // accept() and recv() calls
  ::close( comm_tcp_listen_thread.sockfd );
  ::close( comm_udp_recv_thread.sockfd );
  for ( contact_list_t::iterator it = comm_contacts.begin();
        it != comm_contacts.end();
        it++ ) {
    comm_close_connection( it->second );
  }
  for ( list<recv_thread_t>::iterator it = comm_tcp_recv_threads.thread_list.begin();
        it != comm_tcp_recv_threads.thread_list.end();
        it++ ) {
    ::close( it->sockfd );
  }
  
  comm_tcp_recv_threads.mutex.unlock();

  // Push an execution STOP message into the send and recv queues
  // This gets the receiver parser thread and the sending thread off the blocked
  // wait_and_pop() calls
  // msg_t terminate = create_msg( MSG_T_FINAL, 0, 0, 0, "" );
  msg_t terminate;
  terminate.header = MSG_T_HALT;
  comm_recv_q.push( terminate );
  comm_send_q.push( terminate );

  return 0;
}

void comm_print_recv_msg( const msg_t& incoming ) {
  cout << incoming.dest_id << " <- " << incoming.send_id << " "
    << get_msg_header_string( incoming ) << " / " << incoming.data 
    << " / " << incoming.msg << endl;
}

void comm_print_send_msg( const msg_t& outgoing ) {
  cout << outgoing.send_id << " -> " << outgoing.dest_id << " "
    << get_msg_header_string( outgoing ) << " / " << outgoing.data 
    << " / " << outgoing.msg << endl;
}

int comm_collect_votes() {
  int error = 0;
  comm_votes.expected.set(0);
  comm_votes.count.set(0);

  // msg_t outgoing = create_msg( MSG_T_FINAL, 0, 0, 0, "" );
  msg_t outgoing;
  outgoing.header = MSG_T_VOTE_REQ;

  // bcast MSG_T_VOTE_REQ to all nodes in reachable (include self, doesn't matter)
  for ( set<unsigned int>::iterator it = comm_reachability_list.begin();
        it != comm_reachability_list.end();
        it++ ) {
    outgoing.dest_id = *it;
    error = comm_send( outgoing );
    if ( error != 0 ) {
      cout << "comm_collect_votes Couldn't send vote request to " << *it << endl;
    }
    else {
      comm_votes.expected.inc();
    }
  }

  if ( comm_votes.count.get() < comm_votes.expected.get() ) {
    comm_votes.cv.wait( comm_votes.mutex );
  }

  return 0;
}

bool is_distinguished() {
  // set m = max vn from reachables
  comm_distinguished.m = comm_compute_max_vn();

  // set i = set of reachable nodes with vn = m
  comm_distinguished.i.clear();
  for ( set<unsigned int>::iterator it = comm_reachability_list.begin();
        it != comm_reachability_list.end();
        it++ ) {
    if ( comm_vns[*it].vn == comm_distinguished.m ) {
      comm_distinguished.i.insert( *it );
    }
  }

  // n = ru from any node in i (take the first node)
  // TODO could cause problems if i is empty
  comm_distinguished.n = comm_vns[*(comm_distinguished.i.begin())].ru;

  // if i.size > n/2 and all members of ds are in i ret true, else return false
  if ( comm_distinguished.i.size() > (comm_distinguished.n / 2) ) {
    for ( set<unsigned int>::iterator it = comm_vns[NODE_INFO.id].ds.begin();
          it != comm_vns[NODE_INFO.id].ds.end();
          it++ ) {
      if ( comm_distinguished.i.find( *it ) == comm_distinguished.i.end() ) {
        return false;
      }
    }
  }

  return true;
}

int comm_catch_up() {
  // msg_t outgoing = create_msg( MSG_T_FINAL, 0, 0, 0, "" );;
  msg_t outgoing;
  outgoing.header  = MSG_T_CATCHUP_REQ;
  // outgoing.data    = comm_vns[NODE_INFO.id].vn;
  outgoing.data    = comm_file.get_vn();
  outgoing.dest_id = -1;

  // Find a node in reachables with vn comm_get_max_vn() so that we receive
  // updates from a node which is up to date
  for ( set<unsigned int>::iterator it = comm_reachability_list.begin();
        it != comm_reachability_list.end();
        it++ ) {
    // found a node that is up to date and recahable
    if ( comm_vns[*it].vn == comm_get_max_vn() ) {
      outgoing.dest_id = *it;
      break;
    }
  }
  
  comm_send( outgoing );
  
  comm_catchup.cv.wait( comm_catchup.mutex );
  
  return 0;
}

int comm_do_update() {
  int error = 0;

  // Update our version number
  comm_vns[NODE_INFO.id].vn = comm_distinguished.m + 1;

  // update our reachability list
  comm_vns[NODE_INFO.id].ru = comm_reachability_list.size();

  // update our distinguished set
  comm_vns[NODE_INFO.id].ds.clear();
  
  // if only 3 nodes, insert all 3
  if ( comm_reachability_list.size() == 3 ) {
    comm_vns[NODE_INFO.id].ds = comm_reachability_list;
  }
  
  // else if even sized list of size > 3, grab highest ID in list
  else if ( !(comm_reachability_list.size() & 1) && (comm_reachability_list.size() > 3) ) {
    // Find max id in reachability list
    comm_vns[NODE_INFO.id].ds.insert( 
      *max_element(comm_reachability_list.begin(), comm_reachability_list.end()) );
  }

  return error;
}

unsigned int comm_get_max_vn() {
  return comm_distinguished.m;
}

unsigned int comm_compute_max_vn() {
  unsigned int max_vn = 0;

  for ( map<unsigned int, distributed_file_entry_t>::iterator it = comm_vns.begin();
        it != comm_vns.end();
        it++ ) {
    if ( it->second.vn > max_vn ) {
      max_vn = it->second.vn;
    }
  }

  return max_vn;
}

void comm_pack_distributed_file_entry( const distributed_file_entry_t& entry, msg_t& msg ) {
  // pack a vn, ru, ds, text entry into msg
  // msg.header = msg.header
  // msg.send_id = msg.send_id
  // msg.dest_id = msg.dest_id
  // msg.data = text.length
  // msg.msg = [(sizeof vn) vn]
  //           [(sizeof ru) ru]
  //           [(size_t) ds length]
  //           [(sizeof ds[i])*n ds entries]
  //           [(64 bytes) text]

  // void * memcpy ( void * destination, const void * source, size_t num );

  // Create an iterator for place in msg.msg to write data
  size_t itr = 0;

  // pack vn
  memcpy( &(msg.msg[itr]), &(entry.vn), sizeof(entry.vn) );
  itr += sizeof(entry.vn);

  // pack ru
  memcpy( &(msg.msg[itr]), &(entry.ru), sizeof(entry.ru) );
  itr += sizeof(entry.ru);

  // pack ds
  // memcpy( &(msg.msg[itr]), &(entry.ds.size()), sizeof(size_t) );
  size_t entry_ds_size = entry.ds.size();
  memcpy( &(msg.msg[itr]), &(entry_ds_size), sizeof(entry_ds_size) );
  itr += sizeof(entry.ds.size());
  for ( set<unsigned int>::iterator it = entry.ds.begin(); it != entry.ds.end(); it++ ) {
    memcpy( &(msg.msg[itr]), &(*it), sizeof(unsigned int) );
    itr += sizeof(unsigned int);
  }

  // pack text
  strncpy( &(msg.msg[itr]), entry.text, strlen(entry.text) );
  itr += strlen(entry.text);
  msg.msg[itr+1] = '\0';
}

void comm_unpack_distributed_file_entry ( const msg_t& msg, distributed_file_entry_t& entry ) {
  // unpack a msg_t in the format below into a vn, ru, ds, text entry
  // pack a vn, ru, ds, text entry into msg
  // msg.header = msg.header
  // msg.send_id = msg.send_id
  // msg.dest_id = msg.dest_id
  // msg.data = text.length
  // msg.msg = [(sizeof vn) vn]
  //           [(sizeof ru) ru]
  //           [(size_t) ds length]
  //           [(sizeof ds[i])*n ds entries]
  //           [(size_t) text length]
  //           [text]

  // void * memcpy ( void * destination, const void * source, size_t num );
  size_t itr = 0;

  // unpack vn
  memcpy( &(entry.vn), &(msg.msg[itr]), sizeof(entry.vn) );
  itr += sizeof(entry.vn);

  // unpack ru
  memcpy( &(entry.ru), &(msg.msg[itr]), sizeof(entry.ru) );
  itr += sizeof(entry.ru);

  // unpack ds list
  size_t ds_list_size = 0;
  memcpy( &(ds_list_size), &(msg.msg[itr]), sizeof(ds_list_size) );
  itr += sizeof(size_t);

  entry.ds.clear();
  unsigned int node_id = 0;
  for ( size_t i = 0; i < ds_list_size; i++ ) {
    memcpy( &(node_id), &(msg.msg[itr]), sizeof(unsigned int) );
    itr += sizeof(unsigned int);
    entry.ds.insert( node_id );
  }

  // unpack text
  // strncpy( entry.text, &(msg.msg + itr), strlen(msg.msg[itr]) );
  // memcpy( entry.text, &(msg.msg[itr]), sizeof(entry.text) );
  strcpy( entry.text, &(msg.msg[itr]) );
  entry.text[63] = '\0';
}

void comm_print_vns() {
  cout << "Version Numbers:" << endl;
  for ( map<unsigned int, distributed_file_entry_t>::iterator it = comm_vns.begin();
        it != comm_vns.end();
        it++ ) {
    cout << it->first << ": ";
    print_distributed_file_entry( it->second );    
  }
}

void comm_print_reachability_list() {
  cout << "Reachability list:" << endl;
  for ( set<unsigned int>::iterator it = comm_reachability_list.begin();
        it != comm_reachability_list.end();
        it++ ) {
    cout << *it << " ";     
  }
  cout << endl;
}