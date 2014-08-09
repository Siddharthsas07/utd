#include "RNG.h"
#include <unistd.h>

unsigned int NODE_NETWORK_SIZE = 0;
unsigned int NODE_ID = 0;
unsigned int NODE_NETWORK_MASTER = 0;

bool         node_computation_complete = false;
unsigned int network_nodes_completed = 0;

RNG<unsigned int> rng;


// Wait x amount of milliseconds
void wait_ms_duration( unsigned int duration ) {
  usleep( duration * 1000 );
}

// Wait a random amount of milliseconds between min and max
void wait_ms_between( unsigned int min, unsigned int max ) {
  unsigned int duration = rng.randRange( min, max );
  
  usleep( duration * 1000 );
}

// Wait x amount of seconds
void wait_s_duration( unsigned int duration ) {
  sleep( duration );
}

// Wait a random amount of seconds between min and max
void wait_s_between( unsigned int min, unsigned int max ) {
  unsigned int duration = rng.randRange( min, max );
  
  sleep( duration );
}
