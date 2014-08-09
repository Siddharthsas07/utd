#ifndef THREAD_CONSTRUCTS_H
#define THREAD_CONSTRUCTS_H

// Thread states
typedef enum {
  T_STATE_CREATE = 0,
  T_STATE_INIT,
  T_STATE_ACTIVE,
  T_STATE_STOP,
  T_STATE_IDLE,
  T_STATE_RESTART,
  
  /* just for looping to */
  T_STATE_FINAL
} thread_state_t;

// ID and state of a thread's execution
typedef struct {
  pthread_t id;
  thread_state_t state; 
} thread_t;

// ID and state of a thread's execution, plus a socket fd if it's receiving
typedef struct {
  pthread_t id;
  thread_state_t state;
  unsigned int sockfd;
} recv_thread_t;

#endif