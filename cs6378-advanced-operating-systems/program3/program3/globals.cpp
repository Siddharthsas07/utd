#include "RNG.h"
#include "network_constructs.h"
#include <unistd.h>
#include "globals.h"

unsigned int   NODE_NETWORK_SIZE = 0;
unsigned int   NODE_SUBNET_SIZE = 0;
node_contact_t NODE_INFO;
unsigned int   NODE_NETWORK_MASTER = 0;

// Yup
const unsigned int BASE_GROUP = 0;
const unsigned int CLIENT_GROUP = 1;
const unsigned int SERVER_GROUP = 2;

RNG<unsigned int> rng;

void wait_ms_duration( unsigned int duration ) {
  usleep( duration * 1000 );
}

void wait_ms_between( unsigned int min, unsigned int max ) {
  unsigned int duration = rng.randRange( min, max );
  
  usleep( duration * 1000 );
}

void wait_s_duration( unsigned int duration ) {
  sleep( duration );
}

void wait_s_between( unsigned int min, unsigned int max ) {
  unsigned int duration = rng.randRange( min, max );
  
  sleep( duration );
}
