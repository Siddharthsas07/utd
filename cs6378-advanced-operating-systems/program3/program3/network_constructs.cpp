#include "network_constructs.h"
#include <iostream>
#include <cstring>

void print_contact( const node_contact_t& contact ) {
  cout << contact.id << " @ " << contact.address << " : " << contact.sockfd << " (" << contact.type << ")" << endl;
}

void print_contact_list( const contact_list_t& contact_list ) {
  cout << "Contacts:" << endl;
  for ( contact_list_t::const_iterator it = contact_list.begin();
        it != contact_list.end();
        it++ ) {
    cout << it->first << ": ";
    print_contact( it->second );
  }
}

void print_barrier( const barrier_t& barrier ) {
  cout << "Barrier " << barrier.count.get() << " / "  << barrier.expected.get() << endl;
}

char* get_msg_header_string( const msg_t& msg ) {
  switch ( msg.header ) {
    case MSG_T_WRITE:
      return "MSG_T_WRITE";
    case MSG_T_READ_REQ:
      return "MSG_T_READ_REQ";
    case MSG_T_READ_REP:
      return "MSG_T_READ_REP";
    case MSG_T_NODE_DOWN:
      return "MSG_T_NODE_DOWN";
    case MSG_T_NODE_UP:
      return "MSG_T_NODE_UP";
    case MSG_T_HALT:
      return "MSG_T_HALT";
    case MSG_T_ABORT:
      return "MSG_T_ABORT";
    case MSG_T_COMMIT:
      return "MSG_T_COMMIT";
    case MSG_T_VOTE_REQ:
      return "MSG_T_VOTE_REQ";
    case MSG_T_VOTE_REP:
      return "MSG_T_VOTE_REP";
    case MSG_T_CATCHUP_REQ:
      return "MSG_T_CATCHUP_REQ";
    case MSG_T_CATCHUP_REP:
      return "MSG_T_CATCHUP_REP";
    case MSG_T_BARRIER:
      return "MSG_T_BARRIER";
    case MSG_T_UNREACHABLE:
      return "MSG_T_UNREACHABLE";
    case MSG_T_REACHABLE:
      return "MSG_T_REACHABLE";
    case MSG_T_FINAL:
      return "MSG_T_FINAL";
    default:
      return "MSG_T_UNKNOWN";
  }
}

msg_t create_msg( msg_header_t header, 
                  unsigned int send_id,
                  unsigned int dest_id,
                  unsigned int data,
                  const char*  text ) {
  msg_t res;
  res.header = header;
  res.send_id = send_id;
  res.dest_id = dest_id;
  res.data = data;
  strncpy( res.msg, text, strlen(text) );
}
