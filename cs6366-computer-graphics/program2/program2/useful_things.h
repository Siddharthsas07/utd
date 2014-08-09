#ifndef USEFUL_THINGS_H
#define USEFUL_THINGS_H

#include <cmath>

/* Define OS flags */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__) || defined(__WINDOWS__) || defined(__WIN32__)
#define WINDOWS true
#elif define(__unix__)
#define UNIX true
#elif defined(__linux__) || defined(__linux) || defined(linux)
#define LINUX true
#elif defined(__APPLE__ & __MACH__) || defined(Macintosh) || defined(macintosh)
#define APPLE true
#endif

/* Define compiler flags */
#ifdef __GNUC__
#define GCC true
#endif

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
  T max( const T& a, const T& b ) { return ( a > b ) ? a : b; }

  template <class T>
  T min( const T& a, const T& b ) { return ( a < b ) ? a : b; }

  template <class T>
  T dist( const T& x1, const T& y1, const T& z1,
          const T& x2, const T& y2, const T& z2 ) {
    return sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1) );
  }
}

#define arraySize(a) (sizeof(a) / sizeof(a[0]))

#endif
