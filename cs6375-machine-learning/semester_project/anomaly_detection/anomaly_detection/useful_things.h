#ifndef USEFUL_THINGS_H
#define USEFUL_THINGS_H

#include <cmath>
#include <float.h>

/* Define OS flags */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__) || defined(__WINDOWS__) || defined(__WIN32__)
#define WINDOWS 1
#elif define(__unix__)
#define UNIX 1
#elif defined(__linux__) || defined(__linux) || defined(linux)
#define LINUX 1
#elif defined(__APPLE__ & __MACH__) || defined(Macintosh) || defined(macintosh)
#define APPLE 1
#endif

/* Define compiler flags */
#ifdef __GNUC__
#define GCC 1
#elif defined(_MSC_VER)
#define MSVC 1
#endif

#define arraySize(a) ( (sizeof(a) == 0 ? 0 : sizeof(a) / sizeof(a[0]);) )

// Numerical constants.
namespace nums {
  const double pi = 3.1415926535897932384626433832795028841971693993751058209749445923;
  const double e = 2.71828182845904523536028747135266249775724709369995957496696762772;
}

// Numerical conversions.
namespace conv {
  const double RadToDeg = 57.2957795;
  const double DegToRad = 0.0174532925;
}

// Functions.
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

  double makeFinite( double d );
}

#endif



