#include "network_constructs.h"

#include <iostream>
using std::cout;
using std::endl;

void print_recv_msg( const msg_t& incoming ) {
  cout << incoming.dest_ID << " <- " << incoming.send_ID << " " << print_msg_name( incoming ) << endl;
}

void print_send_msg( const msg_t& outgoing ) {
  cout << outgoing.send_ID << " -> " << outgoing.dest_ID << " " << print_msg_name( outgoing ) << endl;
}

char* print_msg_name( const msg_t& msg ) { 
  switch ( msg.msg_type ) {
    case MSG_T_REQUEST_CS:
      return "MSG_T_REQUEST_CS";
    case MSG_T_REPLY_CS:
      return "MSG_T_REPLY_CS";
    case MSG_T_RESTART:
      return "MSG_T_RESTART";
    case MSG_T_BARRIER:
      return "MSG_T_BARRIER";
    case MSG_T_END:
      return "MSG_T_END";
    case MSG_T_TERMINATE:
      return "MSG_T_TERMINATE";
    case MSG_T_FINAL:
      return "MSG_T_FINAL";
    default:
      return "MSG_T_UNKNOWN_BAD";     
  }
}