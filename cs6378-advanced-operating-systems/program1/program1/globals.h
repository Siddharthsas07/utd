extern unsigned int NODE_NETWORK_SIZE;
extern unsigned int NODE_ID;
extern unsigned int NODE_NETWORK_MASTER;

extern bool         node_computation_complete;
extern unsigned int network_nodes_completed;

// Wait x amount of milliseconds
void wait_ms_duration( unsigned int duration );

// Wait a random amount of milliseconds between min and max
void wait_ms_between( unsigned int min, unsigned int max );

// Wait x amount of seconds
void wait_s_duration( unsigned int duration );

// Wait a random amount of seconds between min and max
void wait_s_between( unsigned int min, unsigned int max );