#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

/* Segments taken from Blake O'Hare */

class Mutex {
  public:
    Mutex() {
      pthread_mutexattr_t mutex_attr;
      pthread_mutexattr_init( &mutex_attr );
      
      pthread_mutex_init( &m_mutex, &mutex_attr );
      
      pthread_mutexattr_destroy( &mutex_attr );
    }
    
    ~Mutex() {
      pthread_mutex_destroy( &m_mutex );
    }
    
    void lock() {
      pthread_mutex_lock( &m_mutex );
    }
    
    void unlock() {
      pthread_mutex_unlock( &m_mutex );
    }
    
    int try_lock() {
      return (  pthread_mutex_trylock( &m_mutex ) == 0 ? 1 : 0 );
    }

    pthread_mutex_t m_mutex;
};

#endif