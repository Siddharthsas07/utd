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
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <algorithm>

#include "str_manip.h"
#include "globals.h"
#include "hr_time.h"
#include "thread_constructs.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::map;

// The threads for the communication server
recv_thread_t          comm_listen_thread_id;
thread_t               comm_recv_parse_thread_id;
vector<recv_thread_t>  comm_recv_thread_ids;
thread_t               comm_send_thread_id;

// Queues for message passing between threads
msg_queue_t comm_send_q;
msg_queue_t comm_recv_q;

// Critical section handling
concurrent_queue<cs_entry_t> comm_cs_log;

concurrent_queue<msg_t> comm_app_msgs;

// Port and contacts to communicate with
int            comm_port = -1;
contact_list_t comm_contacts;
group_map_t    comm_groups;
seq_num_t      comm_seq_num;

// the debuggins
Mutex SHARED_VARS;
timeval TIMER;
NaiveAtomicNumber<unsigned int> comm_write_num;

// Hidden functions from the user!
int comm_open_connection_once( const string& address );
int comm_close_connection( unsigned int id );
int comm_open_connection_until( const string& address );
int comm_open_connections();
int comm_bind_once( int port );

void* comm_listen_thread_start( void* data );
void* comm_recv_thread_start( void* data );
void* comm_recv_parse_thread_start( void* data );
void* comm_send_thread_start( void* data );
void  comm_handle_cs_request( const msg_t& incoming );
void  comm_handle_cs_reply( const msg_t& incoming );
void  comm_handle_ping( const msg_t& incoming );
void  comm_handle_barrier( const msg_t& incoming );
void  comm_handle_write( const msg_t& incoming );
bool  comm_group_exists( unsigned int group );
bool  comm_node_exists( unsigned int group, unsigned int id );

// Communication server setup
int comm_start( const char* config_file_path ) {
  comm_seq_num.num = 0;
  comm_seq_num.max_num = 0;
  comm_write_num.set(0);

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
    
    // Contact to add after each read from file 
    node_contact_t contact;
    
    // All new contacts have no open sockets, so -1
    contact.sockfd = -1;

    // No nodes in the network yet
    NODE_NETWORK_SIZE = 0;
    
    // Read <node ID, node groups, node address>
    while ( config_file.good() ) {
    
      // Read node ID
      std::getline( config_file, line );
      contact.id = str_to_uint( line );
      
      // Read node groups and convert to unsigned ints
      std::getline( config_file, line );
      vector<string> temp_ids = str_breakup( line, "," );
      vector<unsigned int> group_ids;
      //cin.get();
      for ( unsigned int i = 0; i < temp_ids.size(); i++ ) {
        if ( temp_ids[i] != "" ) {
          // cout << "adding a number to group_ids " << str_to_uint( temp_ids[i] ) << endl;
          group_ids.push_back( str_to_uint( temp_ids[i] ) );
        }
      }
      cout << endl;
      
      // Read node address
      std::getline( config_file, line );
      contact.address = line;

      error = comm_add_contact( contact, group_ids );
      if ( error == -1 ) {
        cout << "comm_start Couldn't add contact "
          << contact.id << "@" << contact.address << endl;
        return error;
      }
      
      NODE_NETWORK_SIZE++;
    }

    // cout << "Config file read complete" << endl;
    
    // Finished reading from file
    config_file.close();
    
    // Initialize all the variables before we use them    
    // Not requesting cs of any groups
    for ( group_map_t::iterator it = comm_groups.begin(); it != comm_groups.end(); it++ ) {
      it->second.cs.requesting = false;
    }
    
    
    // comm_print_contacts();
    
    // comm_print_groups();      

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
    
    // cout << "Created listen thread" << endl;

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
    
    // cout << "Created receive parse thread" << endl;

    // Open a connection to all remote addresses (previously read from file)
    error = comm_open_connections();
    if ( error != 0 ) {
      cout << "comm_start Couldn't connect to all remote addresses" << endl;
      return error;
    } 
    
    // cout << "Connected to all remote hosts" << endl;
    
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
    
    // cout << "Created send thread" << endl;
    
    // comm_print_contacts();
    
    error = comm_barrier( BASE_GROUP );
    if ( error != 0 ) {
      cout << "comm_start Couldn't join the network" << endl;
      return error;
    }
  }

  else {
    error = -1;
  }
  
  cout << "Initial node configuration: " << endl;
  comm_print_contacts();
  comm_print_groups();

  return error;
}

int comm_add_contact( const node_contact_t& contact, unsigned int group ) {
  comm_contacts[contact.id] = contact;
  
  // Always add to BASE_GROUP
  comm_groups[BASE_GROUP].cs.cs_map[contact.id].mutex_token = (contact.id == NODE_INFO.id ? true : false);
  comm_groups[BASE_GROUP].cs.cs_map[contact.id].rep_deferred = false;
  
  // Add to specified group
  if ( group != 0 ) {
    comm_groups[group].cs.cs_map[contact.id].mutex_token = (contact.id == NODE_INFO.id ? true : false);
    comm_groups[group].cs.cs_map[contact.id].rep_deferred = false;
  } 
  
  if ( contact.id == NODE_INFO.id ) {
    NODE_INFO.address = contact.address;
  }

  return 0;
}

int comm_add_contact( const node_contact_t& contact, const vector<unsigned int>& groups ) {
  comm_contacts[contact.id] = contact;
  
  // Always add to BASE_GROUP
  comm_groups[BASE_GROUP].cs.cs_map[contact.id].mutex_token = (contact.id == NODE_INFO.id ? true : false);
  comm_groups[BASE_GROUP].cs.cs_map[contact.id].rep_deferred = false;
  
  for ( unsigned int i = 0; i < groups.size(); i++ ) {
    // Add to specified group
    if ( groups[i] != BASE_GROUP ) {
      comm_groups[groups[i]].cs.cs_map[contact.id].mutex_token = (contact.id == NODE_INFO.id ? true : false);
      comm_groups[groups[i]].cs.cs_map[contact.id].rep_deferred = false;
    }
  }
  
  if ( contact.id == NODE_INFO.id ) {
    NODE_INFO.address = contact.address;
  }

  return 0;
}

int comm_remove_contact( unsigned int id, unsigned int group ) {
  if ( group == BASE_GROUP ) {
    cout << "comm_remove_contact Disallowed removal from group id " << BASE_GROUP << endl;
    return -1;
  }
  
  group_map_t::iterator group_it = comm_groups.find( group );
  
  if ( group_it == comm_groups.end() ) {
    cout << "comm_remove_contact Unknown group id " << group << endl;
    return -1;
  }
  
  group_it->second.cs.cs_map.erase( id );
  
  return 0;
}

int comm_remove_contact( unsigned int id, const vector<unsigned int>& groups ) {
  // Iterate over groups and remove any instances of id
  for ( unsigned int i = 0; i < groups.size(); i++ ) {
    comm_remove_contact( id, groups[i] );
  }
  
  return 0;
}

int comm_delete_contact( unsigned int id ) {
  // Close connection
  comm_close_connection( id );
  
  // Remove from all groups except BASE_GROUP
  for ( group_map_t::iterator it = comm_groups.begin(); it != comm_groups.end(); it++ ) {
    comm_remove_contact( id, it->first );
  }
  
  // Remove from BASE_GROUP
  comm_groups[BASE_GROUP].cs.cs_map.erase( id );
  
  // Remove from contacts
  comm_contacts.erase( id );
  
  return 0;
}

unsigned int comm_find_group( unsigned int id ) {
  // Loop over the groups
  for ( group_map_t::iterator group_it = comm_groups.begin(); group_it != comm_groups.end(); group_it++ ) {
    // Ignore base group for now
    if ( group_it->first == BASE_GROUP ) {
      continue;
    }
    
    // Try to find group elsewise
    map<unsigned int, critical_section_t>::iterator map_it = group_it->second.cs.cs_map.find( id );
    if ( map_it != group_it->second.cs.cs_map.end() ) {
      return group_it->first;
    }
  }
  
  return BASE_GROUP;
}

int comm_open_connection_once( const string& address ) {
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

int comm_close_connection( unsigned int id ) {
  contact_list_t::iterator node = comm_contacts.find( id );
  
  if ( node == comm_contacts.end() ) {
    cout << "comm_close_connection No known id " << id << endl;
    return -1;
  }
  
  ::close( node->second.sockfd );
  node->second.sockfd = -1;

  return 0;
}

int comm_open_connection_until( const string& address ) {
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
  for ( contact_list_t::iterator node = comm_contacts.begin();
        node != comm_contacts.end();
        node++ ) {
    if ( node->first == NODE_INFO.id ) {
      continue;
    }
    
    node->second.sockfd = comm_open_connection_until( node->second.address );
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

  // Receive loop
  // Communication hasn't been stopped
  while ( my_info->state == T_STATE_ACTIVE ) {
    int bytes = ::recv( my_info->sockfd, incoming_buf, sizeof(incoming_buf), 0 );
    if ( bytes != sizeof(msg_t) ) {
      cout << "DIDNOT RECEIVE ENTIRE MESSAGE" << endl;
    }

    // Normal receive
    if ( bytes > 0 ) {
      incoming = reinterpret_cast<msg_t*>(incoming_buf);
      comm_recv_q.push( *incoming );
    }

    // Connection closed
    else if ( bytes <= 0 ) {
      cout << "comm_recv_thread_start Terminating receive thread" << endl;
      comm_close_connection( incoming->send_ID );
      my_info->state = T_STATE_STOP;
      break;
    }
  }

  ::close( my_info->sockfd );

  return NULL;
}

void* comm_recv_parse_thread_start( void* data ) {
  thread_t* my_info = reinterpret_cast<thread_t*>(data);
  msg_t incoming;

  // Parse loop
  // Communication hasn't been stopped
  while ( my_info->state == T_STATE_ACTIVE ) {
    incoming = comm_recv();
    comm_print_recv_msg( incoming );

    switch ( incoming.msg_type ) {
      case MSG_T_REQUEST_CS:
        comm_handle_cs_request( incoming );   
        comm_print_groups();
        break;

      case MSG_T_REPLY_CS:
        comm_handle_cs_reply( incoming );
        comm_print_groups();
        break;

      case MSG_T_RESTART:
        // Nothing...yet
        break;

      case MSG_T_BARRIER:
        comm_handle_barrier( incoming );
        break;

      case MSG_T_END:
        // Nothing...
        break;

      case MSG_T_TERMINATE:
        my_info->state = T_STATE_STOP;
        break;
        
      case MSG_T_WRITE:
        comm_handle_write( incoming );
        break;
        
      case MSG_T_PING:
        comm_handle_ping( incoming );
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
    
    contact_list_t::iterator contact = comm_contacts.find( outgoing.dest_ID );
    if ( contact == comm_contacts.end() ) {
      cout << "comm_send_thread_start Unknown destination ID " << outgoing.dest_ID << endl;
      continue;
    }
    
    comm_print_send_msg( outgoing );
    
    error = ::send( contact->second.sockfd, &outgoing, sizeof(outgoing), 0 );
    if ( error == -1 ) {
      cout << "comm_send_thread_start Couldn't send message:\n\t";
      comm_print_send_msg( outgoing );
    }
  }

  return NULL;
}

void comm_handle_cs_request( const msg_t& incoming ) {
  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( incoming.group_ID );
  if ( group_it == comm_groups.end() ) {
    cout << "comm_handle_cs_request Unknown group_ID " << incoming.group_ID << endl;
    return;
  }

  // Check to see if the sender is actually part of the group if not skip
  /*set<unsigned int>::iterator node_exists = group->second.ids.find( incoming.send_ID );
  if ( node_exists == group->second.ids.end() ) {
    return;
  }  */

  // Lock the group and update variables
  // comm_seq_num.mutex.lock();
  // group_it->second.cs.mutex.lock(); 
  SHARED_VARS.lock();
  
  comm_seq_num.max_num = fun::max( comm_seq_num.max_num, incoming.ts );  

  // Determine if a reply to this request should be deferred
  bool defer_it = group_it->second.cs.requesting
             && ( (incoming.ts > comm_seq_num.num)
                  || ((incoming.ts == comm_seq_num.num) && (incoming.send_ID > NODE_INFO.id)) ) ;

  // Defer the reply
  if ( defer_it ) {
    group_it->second.cs.cs_map[incoming.send_ID].rep_deferred = true;
  }

  // Send the reply
  else {
    group_it->second.cs.cs_map[incoming.send_ID].mutex_token = false;
    //group_it->second.cs.cs_map[incoming.send_ID].rep_deferred = false;
    msg_t outgoing;
    outgoing.send_ID  = NODE_INFO.id;
    outgoing.dest_ID  = incoming.send_ID;  
    outgoing.ts       = comm_seq_num.num; // max seq num??
    outgoing.group_ID = incoming.group_ID;
    outgoing.msg_type = MSG_T_REPLY_CS;    
    
    comm_send( outgoing );
  }
  
  // group_it->second.cs.mutex.unlock();
  // comm_seq_num.mutex.unlock();
  SHARED_VARS.unlock();
  
  return;
}

void comm_handle_cs_reply( const msg_t& incoming ) {
  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( incoming.group_ID );
  if ( group_it == comm_groups.end() ) {
    cout << "comm_handle_cs_reply Unknown group_ID " << incoming.group_ID << endl;
    return;
  }

  // Check to see if the sender is actually part of the group if not skip
  /*set<unsigned int>::iterator node_exists = group->second.ids.find( incoming.send_ID );
  if ( node_exists == group->second.ids.end() ) {
    return;
  }  */

  // Lock the group and update variables
  //group_it->second.cs.mutex.lock();
  SHARED_VARS.lock();

  group_it->second.cs.cs_map[incoming.send_ID].mutex_token = true;
  // unsigned int token_count = 0;
  bool permission = true;

  // Determine if this node has permission from all nodes to enter CS
  for ( map<unsigned int, critical_section_t>::iterator it = group_it->second.cs.cs_map.begin();
        it != group_it->second.cs.cs_map.end();
        it++ ) {
    cout << "checking permissions for cs entry..." << endl;
    if ( !(it->second.mutex_token) ) {
      permission = false;
      break;
    }
  }
  
  // group_it->second.cs.mutex.unlock();
  SHARED_VARS.unlock();

  if ( permission ) {
    group_it->second.cs.entry_ok.broadcast();
  }
}

void comm_handle_ping( const msg_t& incoming ) {
  msg_t outgoing;
  outgoing.send_ID = NODE_INFO.id;
  outgoing.dest_ID = incoming.send_ID;
  outgoing.ts = comm_seq_num.num;
  outgoing.group_ID = incoming.group_ID;
  outgoing.msg_type = MSG_T_PING;
  
  comm_send( outgoing );
}

void comm_handle_barrier( const msg_t& incoming ) {
  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( incoming.group_ID );
  if ( group_it == comm_groups.end() ) {
    cout << "comm_handle_barrier Unknown group_ID " << incoming.group_ID << endl;
    return;
  }

  // Check to see if the sender is actually part of the group if not skip
  /*set<unsigned int>::iterator node_exists = group->second.ids.find( incoming.send_ID );
  if ( node_exists == group->second.ids.end() ) {
    return;
  }*/
  
  group_it->second.barrier.count++;
  if ( group_it->second.barrier.count.get() == group_it->second.cs.cs_map.size() ) {
    group_it->second.barrier.cv.broadcast();
  }
}

void comm_handle_write( const msg_t& incoming ) {
  // Stringstream to hold contents to be written to file
  stringstream contents;

  contact_list_t::iterator contact = comm_contacts.find( incoming.send_ID );
  string hostname = ( contact == comm_contacts.end() ? "" : contact->second.address );
  
  contents << "<" << incoming.send_ID << ", " << incoming.ts << ", "
    << hostname << ">" << endl;

  // open up file of "replica_<nodeID>.txt"
  string output_path = "replica_" + int_to_str( NODE_INFO.id ) + ".txt";
  ofstream out_file( output_path, ios::out | ios::app );
  //ofstream out_file( output_path, ios::out );
  
  // write string and close file.
  if ( out_file.is_open() ) {
    out_file << contents.str();
    out_file.close();
  }
  
  else {
    cout << contents.str();
  }
}

bool comm_group_exists( unsigned int group ) {
  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( group );
  if ( group_it == comm_groups.end() ) {
    return false;
  }
  
  return true;
}

bool comm_node_exists( unsigned int group, unsigned int id ) {
  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( group );
  if ( group_it == comm_groups.end() ) {
    return false;
  }

  // Check to see if the sender is actually part of the group if not skip
  map<unsigned int, critical_section_t>::iterator node_exists = group_it->second.cs.cs_map.find( id );
  if ( node_exists == group_it->second.cs.cs_map.end() ) {
    return false;
  }
  
  return true;
}

// Server interaction functions
int comm_send( msg_t msg ) {
  msg.send_ID = NODE_INFO.id;
  comm_send_q.push( msg );

  return 0;
}

int comm_broadcast( const msg_t& msg, unsigned int group ) {
  int error = 0;
  
  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( group );
  if ( group_it == comm_groups.end() ) {
    cout << "comm_broadcast Cannot broadcast to group " << group << endl;
    error = -1;
    return error;
  }

  msg_t outgoing = msg;
  outgoing.group_ID = group;
  
  // For each node ID in the group, send a message to that node
  for ( map<unsigned int, critical_section_t>::iterator it = group_it->second.cs.cs_map.begin();
    it != group_it->second.cs.cs_map.end();
    it++ ) {    
    // Don't broadcast message to self
    if ( it->first == NODE_INFO.id ) {
      continue;
    }
    
    outgoing.dest_ID = it->first;
    
    error = comm_send( outgoing );
    if ( error < 0 ) {
      break;
    }
  }
  
  return error;
}

int comm_write() {

  // 1 acquire cs
  comm_request_cs( CLIENT_GROUP );
  
  // 2 bcast write msg to server group
  msg_t write_msg;
  write_msg.send_ID  = NODE_INFO.id;
  write_msg.dest_ID  = 0;
  write_msg.ts       = comm_seq_num.num;
  write_msg.group_ID = 0;
  write_msg.msg_type = MSG_T_WRITE;
  comm_broadcast( write_msg, SERVER_GROUP );
  
  // 3 wait for servers to finish writing and send response
  wait_ms_duration( 500 );
  
  // 4 release cs
  comm_release_cs( CLIENT_GROUP );
  
  // 5 return
/*
  comm_request_cs( BASE_GROUP );
  
  wait_ms_duration( 30 );
  
  comm_release_cs( BASE_GROUP );
*/
}

msg_t comm_recv() {
  msg_t temp;

  comm_recv_q.wait_and_pop( temp );

  return temp;
}

int comm_barrier( unsigned int group ) {  
  int error = 0;
  
  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( group );
  if ( group_it == comm_groups.end() ) {
    cout << "comm_barrier Unknown group_ID " << group << endl;
    error = -1;
    return error;
  }

  msg_t barrier_msg;
  barrier_msg.send_ID  = NODE_INFO.id;
  barrier_msg.dest_ID  = 0;
  barrier_msg.ts       = comm_seq_num.num;
  barrier_msg.msg_type = MSG_T_BARRIER;
  barrier_msg.group_ID = group;
  
  group_it->second.barrier.count++;
  
  // Broadcast that this node has hit the barrier
  if ( comm_broadcast( barrier_msg, group ) != 0 ) {
    cout << "comm_barrier Couldn't broadcast barrier hit" << endl;
    return -1;
  }
  
  // Not not all nodes have hit the barrier, wait
  if ( group_it->second.barrier.count.get() < group_it->second.cs.cs_map.size() ) {
    group_it->second.barrier.cv.wait( group_it->second.barrier.mutex );
  }

  // Reset number of nodes having hit this barrier 
  group_it->second.barrier.count.set(0);
  return 0;
}

int comm_ping( unsigned int group ) {
  int error = 0;
  
  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( group );
  if ( group_it == comm_groups.end() ) {
    cout << "comm_barrier Unknown group_ID " << group << endl;
    error = -1;
    return error;
  }

  msg_t ping_msg;
  ping_msg.send_ID  = NODE_INFO.id;
  ping_msg.dest_ID  = 0;
  ping_msg.ts       = comm_seq_num.num;
  ping_msg.msg_type = MSG_T_PING;
  ping_msg.group_ID = group;
  
  // Broadcast the ping
  if ( comm_broadcast( ping_msg, group ) != 0 ) {
    cout << "comm_barrier Couldn't broadcast barrier hit" << endl;
    return -1;
  }
  
  // Not not all nodes have hit the barrier, wait
  if ( group_it->second.ping.count.get() < group_it->second.cs.cs_map.size() ) {
    group_it->second.ping.cv.wait( group_it->second.ping.mutex );
  }

  // Reset number of nodes having hit this barrier 
  group_it->second.ping.count.set(0);
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
  for ( contact_list_t::iterator it = comm_contacts.begin();
        it != comm_contacts.end();
        it++ ) {
    comm_close_connection( it->first );     
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

int comm_request_cs( unsigned int group ) {
  int error = 0;

  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( group );
  if ( group_it == comm_groups.end() ) {
  cout << "comm_request_cs Cannot request critical section for group " << group << endl;
    error = -1;
    return error;
  }
  
  // Request critical section for specified group. Record stats for waiting time
  cs_entry_t req;
  req.seq_num = 0;
  req.wait_time = 0;
  req.msg_count = 0;
  CStopWatch timer;
  msg_t      outgoing;
  outgoing.send_ID  = NODE_INFO.id;
  outgoing.dest_ID  = 0;
  outgoing.ts       = comm_seq_num.num;
  outgoing.group_ID = group;
  outgoing.msg_type = MSG_T_REQUEST_CS;
  
  timer.startTimer();
  
  // Lock cs entries so they don't change while reading
  // comm_seq_num.mutex.lock();
  // group_it->second.cs.mutex.lock();
  SHARED_VARS.lock();
  
  group_it->second.cs.requesting = true;
  
  // Update and record sequence numbers
  comm_seq_num.num = comm_seq_num.max_num + 1;
  outgoing.ts = comm_seq_num.num;
  req.seq_num = comm_seq_num.num;  
  
  for ( map<unsigned int, critical_section_t>::iterator it = group_it->second.cs.cs_map.begin();
        it != group_it->second.cs.cs_map.end();
        it++ ) {
    if ( !(it->second.mutex_token) ) {
      req.msg_count++;
      outgoing.dest_ID = it->first;
      comm_send( outgoing );
    }
  }
  
  // Total messages exchanged is always 1 sent + 1 recv = 2*sent
  req.msg_count *= 2;
  
  // group_it->second.cs.mutex.unlock();  
  // comm_seq_num.mutex.unlock();
  SHARED_VARS.unlock();
 
  // If no requests were made, do not wait on any replies
  if ( req.msg_count > 0 ) {
    group_it->second.cs.entry_ok.wait( group_it->second.cs.entry_ok_mutex );
  }
 
  // Record wait time
  timer.stopTimer();
  req.wait_time = timer.getElapsedTime();

  cout << "cs log entry: " << "seq_num " << req.seq_num
       << " wait_time = " << req.wait_time
       << " msg_count = " << req.msg_count << endl;

  comm_cs_log.push( req );
  
  return 0;
}

int comm_release_cs( unsigned int group ) {
  int error = 0;

  // Check to see if the reply is for a group that exists if not skip
  group_map_t::iterator group_it = comm_groups.find( group );
  if ( group_it == comm_groups.end() ) {
    cout << "comm_release_cs Cannot request critical section for group " << group << endl;
    error = -1;
    return error;
  }
  
  msg_t outgoing;
  outgoing.send_ID  = NODE_INFO.id;
  outgoing.dest_ID  = 0;
  outgoing.ts       = comm_seq_num.num;
  outgoing.group_ID = group;
  outgoing.msg_type = MSG_T_REPLY_CS;  
  
  //group_it->second.cs.mutex.lock();
  SHARED_VARS.lock();
  group_it->second.cs.requesting = false;
  outgoing.ts = comm_seq_num.num;
  
  // Send deferred replies
  for ( map<unsigned int, critical_section_t>::iterator it = group_it->second.cs.cs_map.begin();
          it != group_it->second.cs.cs_map.end();
          it++ ) {
    if ( it->second.rep_deferred ) {
      outgoing.dest_ID = it->first;
      //it->second.rep_deferred = false;
      it->second.mutex_token = false;
      comm_send( outgoing );
    }    
  }
  
  //group_it->second.cs.mutex.unlock();
  SHARED_VARS.unlock();

  return 0;
}

void comm_print_recv_msg( const msg_t& incoming ) {
  gettimeofday(&(TIMER), NULL);
  cout << TIMER.tv_sec << "." << TIMER.tv_usec << " (" << comm_seq_num.num << "): " << incoming.dest_ID 
    << " <- " << incoming.send_ID << " " << comm_print_msg_name( incoming ) 
    << " in " << incoming.group_ID << endl;
}

void comm_print_send_msg( const msg_t& outgoing ) {
  gettimeofday(&(TIMER), NULL);
  cout << TIMER.tv_sec << "." << TIMER.tv_usec << " (" << comm_seq_num.num << "): " << outgoing.send_ID 
    << " -> " << outgoing.dest_ID << " " << comm_print_msg_name( outgoing ) 
    << " in " << outgoing.group_ID << endl;
}

char* comm_print_msg_name( const msg_t& msg ) { 
  switch ( msg.msg_type ) {
    case MSG_T_REQUEST_CS:
      return "MSG_T_REQUEST_CS";
    case MSG_T_REPLY_CS:
      return "MSG_T_REPLY_CS";
    case MSG_T_RESTART:
      return "MSG_T_RESTART";
    case MSG_T_BARRIER:
      return "MSG_T_BARRIER";
    case MSG_T_END:
      return "MSG_T_END";
    case MSG_T_TERMINATE:
      return "MSG_T_TERMINATE";
    case MSG_T_WRITE:
      return "MSG_T_WRITE";
    case MSG_T_PING:
      return "MSG_T_PING";
    case MSG_T_FINAL:
      return "MSG_T_FINAL";
    default:
      return "MSG_T_UNKNOWN_BAD";     
  }
}

void comm_print_contacts() {
  gettimeofday(&(TIMER), NULL);
  cout << TIMER.tv_sec << "." << TIMER.tv_usec << " (" << comm_seq_num.num << "): " << "Contacts:" << endl;
  for ( contact_list_t::iterator it = comm_contacts.begin(); it != comm_contacts.end(); it++ ) {
    cout << "\t" << it->first << " @ " << it->second.address << " : " << it->second.sockfd << endl;
  }
}

void comm_print_groups() {
  for ( group_map_t::iterator it = comm_groups.begin(); it != comm_groups.end(); it++ ) {
    gettimeofday(&(TIMER), NULL);
    cout << TIMER.tv_sec << "." << TIMER.tv_usec << " (" << comm_seq_num.num << "): " << "Group " << it->first << ":" << endl;
    
    // Group member IDs
    cout << "\t{";    
    for ( map<unsigned int, critical_section_t>::iterator it2 = it->second.cs.cs_map.begin();
          it2 != it->second.cs.cs_map.end();
          it2++ ) {
      cout << it2->first << " ";
    }
    cout << "}" << endl;
    
    // Mutex tokens
    cout << "\t{";
    for ( map<unsigned int, critical_section_t>::iterator it2 = it->second.cs.cs_map.begin();
          it2 != it->second.cs.cs_map.end();
          it2++ ) {
      cout << it2->second.mutex_token << " ";
    }
    cout << "}" << endl;
    
    // Replies deferred
    cout << "\t{";
    for ( map<unsigned int, critical_section_t>::iterator it2 = it->second.cs.cs_map.begin();
          it2 != it->second.cs.cs_map.end();
          it2++ ) {
      cout << it2->second.rep_deferred << " ";
    }
    cout << "}" << endl;
    
    // Requesting CS
    cout << "\t" << it->second.cs.requesting << endl;
  }
}
