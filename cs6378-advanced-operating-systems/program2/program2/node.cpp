#include "node.h"

#include "str_manip.h"
#include "communication_server.h"
#include <cfloat>
#include <climits>
#include "useful_things.h"
#include "globals.h"
#include <fstream>
#include <iostream>
#include <sstream>

using std::ofstream;
using std::stringstream;
using std::cout;
using std::endl;

int node_set_id( char* n ) {
  NODE_INFO.id = str_to_uint( string(n) );
  
  return 0;
}

int node_submit_results() {  
  double min_wait = DBL_MAX;
  double total_time = 0;
  unsigned int min_messages = UINT_MAX;
  unsigned int max_messages = 0;
  unsigned int total_messages = 0;
  
  stringstream log;
  
  log << "CS Entry Log for node " << NODE_INFO.id << endl;
  for ( unsigned int i = 0; i < comm_cs_log.size(); i++ ) {
    min_messages = fun::min( min_messages, comm_cs_log.m_q[i].msg_count );
    max_messages = fun::max( max_messages, comm_cs_log.m_q[i].msg_count );
    min_wait     = fun::min( min_wait,     comm_cs_log.m_q[i].wait_time );
    total_messages += comm_cs_log.m_q[i].msg_count;
    total_time     += comm_cs_log.m_q[i].wait_time;
    log << "\t" << i << ": wait_time = " << comm_cs_log.m_q[i].wait_time
      << " msg_count = " << comm_cs_log.m_q[i].msg_count << endl;
  }
  
  log << "Overall: " << endl;
  log << "\tTotal wait_time = " << total_time << endl;
  log << "\tAverage wait_time = " << total_time / comm_cs_log.size() << endl;
  log << "\tTotal messages = " << total_messages << endl;
  log << "\tAverage msg_count = " << (double) total_messages / (double) comm_cs_log.size() << endl;
  log << "\tMinimum msg_count = " << min_messages << endl;
  log << "\tMaximum msg_count = " << max_messages << endl;
  
  string output_path = "cs_log_" + int_to_str( NODE_INFO.id ) + ".txt";
  ofstream out_file( output_path, ios::out );
  
  if ( out_file.is_open() ) {
    out_file << log.str();
    out_file.close();
  }
  
  else {
    cout << log.str();
  }
  
  return 0;
}

int node_print_results() {
  if ( NODE_INFO.id == NODE_NETWORK_MASTER ) {
    //cout << "Results are..." << endl;
  }
  
  return 0;
}

int node_clean_up() {
  cout << "Cleaning up..." << endl;
  
  return 0;
}