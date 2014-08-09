#ifndef GLOBALS_H
#define GLOBALS_H

#include "network_constructs.h"

extern unsigned int   NODE_NETWORK_SIZE;
extern node_contact_t NODE_INFO;
extern unsigned int   NODE_NETWORK_MASTER;

// Yup
extern const unsigned int BASE_GROUP;
extern const unsigned int CLIENT_GROUP;
extern const unsigned int SERVER_GROUP;

// Wait x amount of milliseconds
void wait_ms_duration( unsigned int duration );

// Wait a random amount of milliseconds between min and max
void wait_ms_between( unsigned int min, unsigned int max );

// Wait x amount of seconds
void wait_s_duration( unsigned int duration );

// Wait a random amount of seconds between min and max
void wait_s_between( unsigned int min, unsigned int max );

#endif