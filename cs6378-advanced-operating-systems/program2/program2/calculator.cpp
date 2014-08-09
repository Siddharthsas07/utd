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

// Calculator setup
int calc_start() {
  int error = -1;

  // Spawn calculation thread
  error = pthread_create( &(calc_thread_id.id),
                          NULL,
                          calc_thread_start,
                          &calc_thread_id );
  if ( error != 0 ) {
    calc_thread_id.state = T_STATE_STOP;
    cout << "calculator: failed to create calculator thread" << endl;
    return -1;
  }
  calc_thread_id.state = T_STATE_ACTIVE;
  
  return 0;
}

// Thread entry functions
void* calc_thread_start( void* data ) {
  thread_t* my_info = reinterpret_cast<thread_t*>(data);
  
  unsigned int my_group = comm_find_group( NODE_INFO.id );
  cout << "MY GROUP IS " << my_group << endl;
  
  // cin.get();
  
  // only write if client node
  if ( my_group == CLIENT_GROUP ) {
    unsigned int num_iters = 40;
    unsigned int min_wait  = 10;
    unsigned int max_wait  = 50;
    
    // Request critical section num_iters times
    for ( unsigned int i = 0; (i < num_iters) && (my_info->state == T_STATE_ACTIVE); i++ ) {
      wait_ms_between( min_wait, max_wait );
      
      comm_write();
    }
  }

  return NULL;
}

void calc_to_completion() {
  pthread_join ( calc_thread_id.id, NULL );
  
  // Wait for all other threads to complete calculations
  comm_barrier( BASE_GROUP );
}

// Calculator interaction functions
int calc_stop() {
  calc_thread_id.state = T_STATE_STOP;
  
  pthread_join( calc_thread_id.id, NULL );
  
  return 0;
}