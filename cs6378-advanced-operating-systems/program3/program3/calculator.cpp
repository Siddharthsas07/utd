#include "calculator.h"

#include "communication_server.h"
#include "str_manip.h"
#include <iostream>
#include <string>
#include <fstream>
#include "globals.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;

thread_t calc_thread_id;
concurrent_queue<msg_t> calc_msg_q;

string command_file_path = "commands.txt";

void calc_handle_command( const string& cmd );

// Calculator setup
int calc_start() {
  int error = 0;

  // Spawn calculation thread
  error = pthread_create( &(calc_thread_id.id),
                          NULL,
                          calc_thread_start,
                          NULL );
  if ( error != 0 ) {
    calc_thread_id.state = T_STATE_STOP;
    cout << "calc_start Couldn't create calculator thread" << endl;
    return error;
  }
  calc_thread_id.state = T_STATE_ACTIVE;

  return error;
}

// Thread entry functions
void* calc_thread_start( void* data ) {
  // Only do something if it's the network master (manager, command sender)
  if ( NODE_INFO.id == NODE_NETWORK_MASTER ) {
    int error  = 0;

    // Open configuration file, read only
    string line;
    ifstream cmd_file( command_file_path, ios::in );

    if ( cmd_file.is_open() ) {
      while ( cmd_file.good() ) {
        std::getline( cmd_file, line );
        calc_handle_command( line );
      }

      // Finished reading from file
      cmd_file.close();
    }

    else {
      error = -1;
      cout << "calc_thread_start Couldn't open command file" << endl;
    }
  }

  else {
    // don't do anything if not the network master. Just block until network
    // execution completes (fall through to calc_to_completion)
  }
}

void calc_to_completion() {
  pthread_join ( calc_thread_id.id, NULL );

  // Wait for all other threads to complete calculations
  
  wait_s_duration( 3 );
  
  comm_barrier();
}

// Calculator interaction functions
int calc_stop() {
  calc_thread_id.state = T_STATE_STOP;

  pthread_join( calc_thread_id.id, NULL );

  return 0;
}

void calc_handle_command( const string& cmd ) {
  // Do not accept empty commands
  if ( cmd == "" || cmd == "\n") {
    return;
  }

  msg_t cmd_msg = create_msg( MSG_T_FINAL, 0, 0, 0, "" );
  
  // Get first token
  size_t lhs = 0;
  size_t rhs = cmd.find_first_of( " ", lhs );
  
  string cmd_type = cmd.substr( lhs, rhs-lhs );
  
  lhs = rhs+1;
  rhs = cmd.find_first_of( " ", lhs );
  
  string dest_id = cmd.substr( lhs, rhs-lhs );
  
  lhs = rhs+2;
  rhs = cmd.size()-1;
  
  cout << "Parsed command: " << cmd_type << " " << dest_id;
  
  if ( cmd_type.compare( "READ" ) == 0 ) {
    cout << endl;
    cmd_msg.header = MSG_T_READ_REQ;
    cmd_msg.dest_id = str_to_uint( dest_id );
  }
  
  else if ( cmd_type.compare( "WRITE" ) == 0 ) {
    cmd_msg.header = MSG_T_WRITE;
    cmd_msg.dest_id = str_to_uint( dest_id );
    string text = cmd.substr( lhs, rhs-lhs );
    cout << " " << text << endl;
    memset( cmd_msg.msg, 0, sizeof(cmd_msg.msg) );
    strncpy( cmd_msg.msg, text.c_str(), text.size() );
  }
  
  else if ( cmd_type.compare( "NODE-DOWN" ) == 0 ) {
    cout << endl;
    cmd_msg.header = MSG_T_NODE_DOWN;
    cmd_msg.dest_id = str_to_uint( dest_id );
  }
  
  else if ( cmd_type.compare( "NODE-UP" ) == 0 ) {
    cout << endl;
    cmd_msg.header = MSG_T_NODE_UP;
    cmd_msg.dest_id = str_to_uint( dest_id );
  }
  
  else if ( cmd_type.compare( "HALT" ) == 0 ) {
    cout << endl;
    cmd_msg.header = MSG_T_HALT;
    cmd_msg.dest_id = str_to_uint( dest_id );
  }
  
  else if ( cmd_type.compare( "WAIT" ) == 0 ) {
    cout << endl;
    int duration = str_to_int( dest_id );
    if ( duration == -1 ) {
      getchar();
    }

    else {
      wait_ms_duration( duration );
    }
    
    return;
  }
  
  else {
    cout << endl << "calc_handle_command Unrecognized command type " << cmd_type << endl;
    return;
  }
  
  comm_send( cmd_msg );
}