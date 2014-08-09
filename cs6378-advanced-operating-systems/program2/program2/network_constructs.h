#ifndef NETWORK_CONSTRUCTS_H
#define NETWORK_CONSTRUCTS_H

#include <sys/time.h>
#include <vector>
#include <string>
#include <map>
#include <set>

#include "NaiveAtomicNumber.hpp"
#include "Mutex.h"
#include "ConditionVariable.h"
#include "NaiveConcurrentQueue.h"

using std::string;
using std::vector;
using std::map;
using std::set;

// Different types of messages in the system
typedef enum {
  MSG_T_REQUEST_CS = 0,
  MSG_T_REPLY_CS,
  MSG_T_RESTART,
  MSG_T_BARRIER,
  MSG_T_END,
  MSG_T_TERMINATE,
  MSG_T_WRITE,
  MSG_T_PING,
  
  /* just for looping to */
  MSG_T_FINAL
} message_types_t;

// Message struct for sending between processes/nodes
typedef struct {
  unsigned int    send_ID;
  unsigned int    dest_ID;
  unsigned int    ts;
  unsigned int    group_ID;
  message_types_t msg_type;  
} msg_t;

// Contact information for a node
typedef struct {
  unsigned int id;
  string       address;
  int          sockfd;
} node_contact_t;

typedef struct {
  unsigned int seq_num;
  double       wait_time;
  unsigned int msg_count;
} cs_entry_t;

typedef struct {
  NaiveAtomicNumber<unsigned int> count;
  Mutex mutex;
  ConditionVariable cv;
} barrier_t;

typedef struct {
  bool mutex_token;
  bool rep_deferred;
} critical_section_t;

typedef struct {
  Mutex             mutex;
  bool              requesting;
  ConditionVariable entry_ok;
  Mutex             entry_ok_mutex;
  map<unsigned int, critical_section_t> cs_map;  
} critical_section_group_t;

typedef struct {
  critical_section_group_t cs;
  barrier_t                barrier;  
  barrier_t                ping;
} node_group_t;

typedef struct {
  unsigned int num;
  unsigned int max_num;
  Mutex mutex;
} seq_num_t;

//typedef vector<node_contact_t> contact_list_t;
typedef map<unsigned int, node_contact_t> contact_list_t;

typedef map<unsigned int, node_group_t> group_map_t;

typedef concurrent_queue<msg_t> msg_queue_t;

#endif