#ifndef NAIVE_CONCURRENT_QUEUE_H
#define NAIVE_CONCURRENT_QUEUE_H

#include <deque>
#include <pthread.h>

using namespace std;
/* Ideas taken from 
  http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
  */

template<typename T>
class concurrent_queue {
  private:
    pthread_mutex_t lock;  // Mutex for acquiring rights to the queue
    pthread_cond_t  cond;  // Conditional variable 
  
  public:
    deque<T>        m_q;   // The queue
  
    concurrent_queue() {
      // Initialize lock and cond appropriately
      pthread_mutex_init( &lock, NULL );
      pthread_cond_init( &cond, NULL );
    }
    
    ~concurrent_queue() {
      pthread_mutex_destroy( &lock );
      pthread_cond_destroy( &cond );
    }
    
    // Push a new item into the queue
    void push( const T& x ) {
      pthread_mutex_lock( &lock );
      m_q.push_back( x );
      pthread_mutex_unlock( &lock );
      pthread_cond_signal( &cond );
    }
    
    // Check if the queue is empty
    bool empty() const {
      return m_q.empty();
    }
    
    // Try to pop an item. Store item in data, return success/fail.
    bool try_pop( T& data ) {
      pthread_mutex_lock( &lock );
      if ( m_q.empty() ) {
        pthread_mutex_unlock( &lock );
        return false;
      }
      
      data = m_q.pop_front();
      m_q.pop();
      pthread_mutex_unlock( &lock );
      return true;
    }
    
    // Forced wait to pop
    void wait_and_pop( T& data ) {
      pthread_mutex_lock( &lock );
      while ( m_q.empty() ) {
        pthread_cond_wait( &cond, &lock );
      }
      
      data = m_q.front();
      m_q.pop_front();
      pthread_mutex_unlock( &lock );
    }
    
    inline unsigned int size() {
      return m_q.size();
    }
};


#endif