#include <string>

#include "node.h"
#include "str_manip.h"

using std::string;

int node_set_id( char* n ) {
  NODE_INFO.id = str_to_uint( string(n) );
  
  return 0;
}