#ifndef USEFUL_THINGS_H
#define USEFUL_THINGS_H

#include <cmath>
#include <climits>
#include <float.h>

// Define operating system flags
namespace os {
  #if defined(WIN32) || defined(_WIN32) || (defined(__WIN32) && !defined(__CYGWIN__)) || defined(__WINDOWS__) || defined(__WIN32__)
  const bool WINDOWS = true;
  #elif defined(__unix__)
  const bool UNIX = true;
  #elif defined(__linux__) || defined(__linux) || defined(linux)
  cosnt bool LINUX = true;
  #elif (defined(__APPLE__) && defined(__MACH__)) || defined(Macintosh) || defined(macintosh)
  const bool APPLE = true;
  #endif
};

// Define compiler flags
namespace compiler {
  #if defined(__GNUC__)
  const bool GCC = true;
  #elif defined(_MSC_VER)
  const bool MSVS = true;
  #endif
}

// Useful flags 
namespace flags {
  const char SUCCESS = 0;
  const char FAILURE = 1;
  const char IND     = 2;
}

// Numerical constants 
namespace nums {
  const double pi = 3.1415926535897932384626433832795028841971693993751058209749445923;
  const double e = 2.71828182845904523536028747135266249775724709369995957496696762772;
}

// Numerical conversions 
namespace conv {
  const double RadToDeg = 57.2957795;
  const double DegToRad = 0.0174532925;
}

// Functions 
namespace fun {
  template <class T>
  T max( const T& a, const T& b ) { 
    return ( a > b ) ? a : b;
  }

  template <class T>
  T min( const T& a, const T& b ) { 
    return ( a < b ) ? a : b; 
  }

  template <class T>
  T dist( const T& x1, const T& y1, const T& z1,
          const T& x2, const T& y2, const T& z2 ) {
    double t1 = x2-x1;
    double t2 = y2-y1;
    double t3 = z2-z1;
    return sqrt( t1*t1 + t2*t2 + t3*t3 );
  }

  template <class T>
  T dist( const T& x1, const T& y1,
          const T& x2, const T& y2 ) {
    double t1 = x2-x1;
    double t2 = y2-y1;
    return sqrt( t1*t1 + t2*t2 );
  }

  template <class T>
  T dist( const T& x, const T& y ) {
    return fabs( x - y );
  }

  template <class T>
  T bound( const T& num, const T& low, const T& high ) {
    if ( num > high )     { return high; }
    else if ( num < low ) { return low; }
    else                  { return num; }
  }

  double makeFinite( double d );
  float  makeFinite( float f );
  int    makeFinite( int i );
  unsigned int makeFinite( unsigned int u );
  char   makeFinite( char c );

  template <class T>
  bool in( const T& num, const T& low, const T& high ) {
    if ( num > high || num < low ) 
      return false;
    return true;
  }

  template <class T>
  void swap( T& a, T& b ) {
    T temp = a;
    a = b;
    b = temp;
    return;
  }
}

#define arraySize(a) ( (sizeof(a) == 0 ? 0 : sizeof(a) / sizeof(a[0])) )

#endif
