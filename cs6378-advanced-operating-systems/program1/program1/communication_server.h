#ifndef COMMUNICATION_SERVER_H
#define COMMUNICATION_SERVER_H

#include "NaiveConcurrentQueue.h"
#include "network_constructs.h"

// Communications port for this node
extern int comm_port;

// The critical section log for this node
extern concurrent_queue<cs_stat_entry_t> comm_cs_log;

// Communication server setup
int comm_start( char* config_file_path );
int comm_add_contact( const node_contact_t& contact );
int comm_open_connection_once( string address );
int comm_close_connection( unsigned int n );
int comm_open_connection_until( string address );
int comm_open_connections();
int comm_bind_once( int port );

// Thread entry functions
void* comm_listen_thread_start( void* data );
void* comm_recv_thread_start( void* data );
void* comm_recv_parse_thread_start( void* data );
void* comm_send_thread_start( void* data );

// Server interaction functions
int   comm_send( msg_t msg );
int   comm_broadcast( msg_t msg );
msg_t comm_recv();
int   comm_barrier();
int   comm_stop();

// Requests and releases control of the critical section
int comm_request_cs();
int comm_release_cs();

#endif