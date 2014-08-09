#ifndef CONDITION_VARIABLE_H
#define CONDITION_VARIABLE_H

#include <pthread.h>
#include "Mutex.h"

/* Segments taken from Blake O'Hare */

class ConditionVariable {
  public:
    ConditionVariable() {
      pthread_cond_init( &m_condition, NULL );
    }
    
    ~ConditionVariable() {
      pthread_cond_destroy( &m_condition );
    }
    
    void wait( Mutex& mutex ) {
      pthread_cond_wait( &m_condition, &mutex.m_mutex );
    }
    
    void signal() {
      pthread_cond_signal( &m_condition );
    }
    
    void broadcast() {
      pthread_cond_broadcast( &m_condition );
    }

    pthread_cond_t m_condition;
};

#endif