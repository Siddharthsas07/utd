#ifndef RNG_H
#define RNG_H

#include <ctime>

using namespace std;

template <class T>
class RNG {
public:
  RNG() {};
  ~RNG() {};

  T randRange( const T& low, const T& high ) {
    srand(static_cast<unsigned int>(time(NULL)));
    return low + (rand() % (high-low));
  }

  T rand() {
    return std::rand();
  }

  T rand( const T& high ) {
    return randRange( 0, high );
  }  
};

#endif