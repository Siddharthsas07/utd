#include "useful_things.h"

double fun::makeFinite( double d ) {
  if ( d > DBL_MAX ) { d = DBL_MAX; }
  else if ( d < -DBL_MAX ) { d = -DBL_MAX; }
  return d;
}
