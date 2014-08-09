#ifndef USEFUL_THINGS_H
#define USEFUL_THINGS_H

#include <string>
#include <cmath>

using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__) || defined(__WINDOWS__) || defined(__WIN32__)
#define WINDOWS true
#define OS "windows"
#elif define(__unix__)
#define UNIX true
#define OS "unix"
#elif defined(__linux__) || defined(__linux) || defined(linux)
#define LINUX true
#define OS "linux"
#elif defined(__APPLE__ & __MACH__) || defined(Macintosh) || defined(macintosh)
#define APPLE true
#define OS "mac"
#endif

#ifdef __GNUC__
#define GCC true
#endif

namespace nums {
  const double pi = 3.1415926535897932384626433832795028841971693993751058209749445923;
  const double e = 2.71828182845904523536028747135266249775724709369995957496696762772;
}

namespace conv {
  const double RadToDeg = 57.2957795;
  const double DegToRad = 0.0174532925;
}

namespace fun {
  template <class T>
  T max( const T& a, const T& b ) { return ( a > b ) ? a : b; }

  template <class T>
  T min( const T& a, const T& b ) { return ( a < b ) ? a : b; }
}

#endif
