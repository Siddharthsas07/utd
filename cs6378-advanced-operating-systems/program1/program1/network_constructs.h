#ifndef NETWORK_CONSTRUCTS_H
#define NETWORK_CONSTRUCTS_H

#include <string>
using std::string;

#include <sys/time.h>

#include "NaiveAtomicNumber.hpp"
#include "Mutex.h"
#include "ConditionVariable.h"

// Different types of messages in the system
typedef enum {
  MSG_T_REQUEST_CS = 0,
  MSG_T_REPLY_CS,
  //MSG_T_NODE_COMPUTATION_COMPLETE,
  //MSG_T_NETWORK_COMPUTATION_COMPLETE,
  MSG_T_RESTART,
  //MSG_T_JOIN,
  MSG_T_BARRIER,
  MSG_T_END,
  MSG_T_TERMINATE,
  
  /* just for looping to */
  MSG_T_FINAL
} message_t;

// Message struct for sending between processes/nodes
typedef struct {
  unsigned int send_ID;
  unsigned int dest_ID;
  unsigned int ts;
  message_t    msg_type;  
} msg_t;

// Contact information for a node
typedef struct {
  unsigned int id;
  string       address;
  int          sockfd;
} node_contact_t;

typedef struct {
  //pthread_t    requester;
  unsigned int seq_num;
  double       wait_time;
  unsigned int msg_count;
} cs_stat_entry_t;

typedef struct {
  NaiveAtomicNumber<unsigned int> count;
  Mutex mutex;
  ConditionVariable cv;
} barrier_t;

// For printing receive and send messages for debugging
void print_recv_msg( const msg_t& incoming );
void print_send_msg( const msg_t& outgoing );
char* print_msg_name( const msg_t& msg );

#endif