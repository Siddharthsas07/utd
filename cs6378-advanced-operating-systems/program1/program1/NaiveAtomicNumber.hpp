#ifndef NAIVE_ATOMIC_NUMBER_H
#define NAIVE_ATOMIC_NUMBER_H

#include "Mutex.h"

template<class T>
class NaiveAtomicNumber{
  private:
    T m_val;
    Mutex m_mutex;
    
  public:
    /*NaiveAtomicNumber() {
      m_val = static_cast<T>(0);
    }*/
  
    NaiveAtomicNumber( const T& val = static_cast<T>(0) ) {
      m_val = val;
    }
    
    NaiveAtomicNumber( const NaiveAtomicNumber<T>& val ) {
      m_val = val;
    }
    
    ~NaiveAtomicNumber() {
      // nothing
    }
    
    NaiveAtomicNumber& operator=( const NaiveAtomicNumber<T>& rhs ) { 
      //make sure we aren't assigning to ourselves
      if ( this == &rhs )
        return *this;
      
      set( rhs.get() );
      
      return *this;
    }
    
    T operator=( const T& rhs ) {
      if ( &m_val == &rhs ) {
        return get();
      }
      
      set( rhs );
      
      return get();      
    }
    
    T inc() {
      m_mutex.lock();
      m_val = m_val + static_cast<T>( 1 );
      m_mutex.unlock();
      return get();      
    }
    
    T dec() {
      m_mutex.lock();
      m_val = m_val - static_cast<T>( 1 );
      m_mutex.unlock();
      return get();      
    }
    
    T operator++() { return inc(); }
    T operator++( int ) { return inc(); }
    T operator--() { return dec(); }
    T operator--( int ) { return dec(); }
    
    T get() const {
      return m_val; 
    }
    
    void set( const T& rhs ) {
      m_mutex.lock();
      m_val = rhs;
      m_mutex.unlock();
    }
};

#endif