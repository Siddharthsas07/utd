#include "useful_things.h"

double fun::makeFinite( double d ) {
  return bound( d, -DBL_MAX, DBL_MAX );
}

float fun::makeFinite( float f ) {
  return bound( f, -FLT_MAX, FLT_MAX );
}

int fun::makeFinite( int i ) {
  return bound( i, -INT_MAX, INT_MAX );
}

unsigned int fun::makeFinite( unsigned int u ) {
  return bound( u, (unsigned int)0, UINT_MAX );
}

char fun::makeFinite( char c ) {
  return bound( c, (char)-CHAR_MAX, (char)CHAR_MAX );
}
