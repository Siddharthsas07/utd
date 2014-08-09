#ifndef RNG_H
#define RNG_H

#include <ctime>
#include <cstdlib>

using namespace std;

template <class T>
class RNG {
public:
  RNG() {};
  ~RNG() {};

  T randRange( const T& low, const T& high ) {
    ::srand( ::time(NULL) );
    return low + (rand() % (high-low));
  }

  T rand() {
    ::srand( ::time(NULL) );
    return ::rand();
  }

  T rand( const T& high ) {
    return randRange( 0, high );
  }  
};

#endif