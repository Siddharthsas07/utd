#include "useful_things.h"

double fun::makeFinite( double d ) {
  return bound( d, -DBL_MAX, DBL_MAX );
}
