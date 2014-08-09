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

template <>
class RNG<float> {
public:
  RNG() {};
  ~RNG() {};

  float randRange( const float& low, const float& high ) {
    ::srand( ::time(NULL) );
    float random = static_cast<float>(::rand()) / static_cast<float>(RAND_MAX);
    float range = high - low;
    return low + (random * range);
  }

  float rand() {
    ::srand( ::time(NULL) );
    return static_cast<float>(::rand()) / static_cast<float>(RAND_MAX);;
  }

  float rand( const float& high ) {
    return randRange( 0.0f, high );
  } 
};

template <>
class RNG<double> {
public:
  RNG() {};
  ~RNG() {};

  double randRange( const double& low, const double& high ) {
    ::srand( ::time(NULL) );
    double random = static_cast<double>(::rand()) / static_cast<double>(RAND_MAX);
    double range = high - low;
    return low + (random * range);
  }

  double rand() {
    ::srand( ::time(NULL) );
    return static_cast<double>(::rand()) / static_cast<double>(RAND_MAX);;
  }

  double rand( const double& high ) {
    return randRange( 0.0f, high );
  } 
};

#endif