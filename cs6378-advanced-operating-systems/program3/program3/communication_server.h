/*
  @fn  comm_start
  @des Starts communication server given a configuration file.
  @par onfig_file_path -- location to the configuration file.
  @ret < 0 if unsuccessful read
  @not BLOCKING
  
  @fn  comm_add_contact
  @des adds contact to the known addresses for communication
  @par contact -- the information for the new contact
  @par group -- the group to add this contact to. If not specified, defaults to 0.
  @not All contacts are added to group 0 by default, even when added specifically
      to another group. Group 0 is the "full list" of contacts.
  @not If you add a contact with this node's ID, its mutex_token entry for the 
      group is automatically set to true.
  @ret < 0 if unsuccessful add
  
  @fn comm_add_contact
  @des adds contact to the known addresses for communication
  @par contact -- the information for the new contact
  @par groups -- a vector of groups to add the contact to. Contact is automatically
      added to group 0 in addition to any groups listed in the vector.
  @not If you add a contact with this node's ID, its mutex_token entry for the 
      group is automatically set to true.
  @ret < 0 if unsuccessful add
  
  @fn  comm_remove_contact
  @des Removes a contact from the known addresses
  @par id -- the network ID of the node to remove
  @par group -- The group to remove this contact from. If not specified, defaults
      to 0.
  @not Specifying group 0 will result in no removal. Use comm_delete_contact instead
  @ret < 0 if unsuccessful remove
  
  @fn  comm_remove_contact
  @des Remoeves a contact from the known addresses
  @par id -- the network ID of the node to remove
  @par groups -- a vector of groups to remove the contact from. 
  @not If any of the groups is 0, this will not remove from that group. Use
      comm_delete_contact instead.
  @ret < 0 if unsuccessful remove
  
  @fn  comm_delete_contact
  @des Completely removes a contact from the known list and from all groups
  @par id -- the network ID of the node to delete
  @ret < 0 if unsuccessful delete
  
  @fn  comm_send
  @des Send the supplied message
  @par msg -- The message to send. This must be populated with the proper 
      dest_ID, group_ID, and msg_type, otherwise it will cause erroneous behavior.
  @ret < 0 if unsuccessful send
  @not NONBLOCKING
  
  @fn  comm_broadcast
  @des Send the specified message to all nodes in the specified group
  @par msg -- The message to be sent. Must be populated with the proper msg_type.
  @par group -- The group to send the message to.
  @ret < 0 if unsuccessful broadcast
  @not NONBLOCKING (calls comm_send)
  
  @fn  comm_write
  @des Writes an entry to the server group's log files
  @ret < 0 if unsuccessful write
  @not BLOCKING
  
  @fn  comm_recv
  @des Gets the most recent message received
  @ret msg_t -- The most recent message received
  @not BLOCKING
  
  @fn  comm_barrier
  @des Causes the calling node to block until all nodes in the specified group 
      have reached the same execution point. 
  @not This assumes the calling node is part of the specified group.
  @par group -- The group to synchronize with.
  @ret < 0 if unsuccessful wait
  @not BLOCKING
  
  @fn  comm_ping
  @des Sends a message to the specified group and waits until all nodes in the 
      group respond.
  @par group -- The group to ping.
  @ret < 0 if ping unsuccessful
  @not BLOCKING
  
  @fn  comm_stop
  @des Stops the communication server entirely.
  @ret < 0 if unsuccessful stop
  
  @fn  comm_request_cs
  @des Requests critical section access via mutual exclusion wrt to the specified group
  @par group -- The group to request critical section access from.
  @ret < 0 if unsuccessful
  @not BLOCKING
  
  @fn  comm_release_cs
  @des Release critical section control of the specified group
  @par group -- The group to release critical section control of.
  @ret < 0 if unsuccessful
  @not NONBLOCKING
  
  @fn....print functions do what they say. Prints to stdout
 */

#ifndef COMMUNICATION_SERVER_H
#define COMMUNICATION_SERVER_H

#include <vector>
using std::vector;

#include "NaiveConcurrentQueue.h"
#include "network_constructs.h"

// Communications port for this node
extern int comm_port;

// Communication server setup
int comm_start( const char* config_file_path );
int comm_add_contact( const node_contact_t& contact );
int comm_delete_contact( unsigned int id );
int comm_open_connection_once( const node_contact_t& contact );
int comm_open_connection_until( const node_contact_t& contact );
int comm_close_connection( const node_contact_t& contact );
int comm_initiate_contacts();

// Server interaction functions
int   comm_send( msg_t msg );
int   comm_broadcast( msg_t msg );
msg_t comm_recv();
int   comm_barrier();
int   comm_stop();

// For printing receive and send messages for debugging
void comm_print_recv_msg( const msg_t& incoming );
void comm_print_send_msg( const msg_t& outgoing );

#endif