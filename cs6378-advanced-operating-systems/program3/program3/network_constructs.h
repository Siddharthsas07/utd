#ifndef NETWORK_CONSTRUCTS_H
#define NETWORK_CONSTRUCTS_H

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string>
#include <map>
#include <set>

#include "NaiveAtomicNumber.hpp"
#include "Mutex.h"
#include "ConditionVariable.h"
#include "NaiveConcurrentQueue.h"

using std::string;
using std::map;
using std::set;

// Different types of messages in the system
typedef enum {
  // Master control types
  MSG_T_WRITE,
  MSG_T_READ_REQ,
  MSG_T_READ_REP,
  MSG_T_NODE_DOWN,
  MSG_T_NODE_UP,
  MSG_T_HALT,
  
  // Algorithm types
  MSG_T_ABORT,
  MSG_T_COMMIT, 
  MSG_T_VOTE_REQ,
  MSG_T_VOTE_REP,
  MSG_T_CATCHUP_REQ,
  MSG_T_CATCHUP_REP,
  
  MSG_T_BARRIER,
  MSG_T_UNREACHABLE,
  MSG_T_REACHABLE,
  
  MSG_T_FINAL
} msg_header_t;

// Message struct for sending between processes/nodes
typedef struct {
  msg_header_t header;  
  unsigned int send_id;
  unsigned int dest_id;
  unsigned int data;
  char         msg[128];  
} msg_t;

// Contact information for a node
typedef struct {
  unsigned int id;
  string       address;
  int          type;
  int          sockfd; 
  addrinfo     *server_info;
} node_contact_t;

typedef struct {
  NaiveAtomicNumber<unsigned int> count;
  NaiveAtomicNumber<unsigned int> expected;
  Mutex mutex;
  ConditionVariable cv;
} barrier_t;

typedef map<unsigned int, node_contact_t> contact_list_t;

typedef map<unsigned int, barrier_t> barrier_list_t;

typedef concurrent_queue<msg_t> msg_queue_t;

void print_contact( const node_contact_t& contact );
void print_contact_list( const contact_list_t& contact_list );
void print_barrier( const barrier_t& barrier );
char* get_msg_header_string( const msg_t& msg );

msg_t create_msg( msg_header_t header, 
                  unsigned int send_id,
                  unsigned int dest_id,
                  unsigned int data,
                  const char*  text );

#endif