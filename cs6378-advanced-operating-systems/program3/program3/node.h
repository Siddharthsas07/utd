#ifndef NODE_H
#define NODE_H

#include "globals.h"

// set this node's ID
int node_set_id( char* n );

// Calculate averages and store results to file for the CS entries
int node_submit_results();

// Currently does nothing
int node_print_results();

// Stops communication and shuts down the node
int node_clean_up();

#endif