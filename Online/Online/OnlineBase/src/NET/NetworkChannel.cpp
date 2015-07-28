#include <cstring>
#include <cerrno>
#include "NET/NetworkChannel.h"
#include "RTL/rtl.h"
#include "RTL/QIO.h"

// ----------------------------------------------------------------------------
//  Default constructor
//                                      M.Frank
// ----------------------------------------------------------------------------
NetworkChannel::NetworkChannel() 
  : m_bCancel(false), m_bValid(false), m_port(~0x0), m_alarmID(0), m_errno(0)
{
}
NetworkChannel::~NetworkChannel()  {
  if ( 0 != m_alarmID ) {
    stopTimer();
  }
}
// ----------------------------------------------------------------------------
//  Return error string
//                                      M.Frank
// ----------------------------------------------------------------------------
const char* NetworkChannel::errMsg()  {
  return lib_rtl_error_message(m_errno);
}

// ----------------------------------------------------------------------------
//  Set Select TMO on receive.
//                                      M.Frank
// ----------------------------------------------------------------------------
int NetworkChannel::selectTmo(int flags, int tmo) {
  if ( tmo ) {
    timeval tv = { tmo, 0 };
    fd_set write_fds, read_fds, exc_fds;
    fd_set *w_fds = flags&WRITE  ? &write_fds : 0;
    fd_set *r_fds = flags&READ   ? &read_fds  : 0;
    fd_set *e_fds = flags&EXCEPT ? &exc_fds   : 0;
    if ( w_fds ) { FD_ZERO(w_fds); FD_SET(m_socket,w_fds); }
    if ( r_fds ) { FD_ZERO(r_fds); FD_SET(m_socket,r_fds); }
    if ( e_fds ) { FD_ZERO(e_fds); FD_SET(m_socket,e_fds); }
    int res = select(m_socket+1, r_fds, w_fds, e_fds, &tv);
    if ( res < 0 ) {
      m_errno = ::lib_rtl_socket_error();
      return -1;
    }
    else if ( res == 0 ) {
      m_bCancel = true;
      return -1;
    }
  }
  return 1;
}

// ----------------------------------------------------------------------------
// o Set socket option to reuse address in case of re-creation
//                                      M.Frank
// ----------------------------------------------------------------------------
int NetworkChannel::reuseAddress()  const  {
  return 0;
}
// ----------------------------------------------------------------------------
//  AST called on timeout for receiving data on socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int NetworkChannel::TmoAST ( void* par )   {
  NetworkChannel* chan = (NetworkChannel*)par;
  chan->m_bCancel = true;
#ifdef _VMS
  chan->m_errno  = SS$_CANCEL;
#else
  chan->m_errno  = EIO;
#endif
  chan->m_alarmID = 0;
  lib_rtl_cancel_io(chan->m_socket);    
  return 0x1;
}
// ----------------------------------------------------------------------------
//  Start timer before receiving data, if you don't want to wait forever
//                                      M.Frank
// ----------------------------------------------------------------------------
void NetworkChannel::startTimer( int tmo )  {
  if ( tmo > 0 )  {
    if ( m_alarmID ) lib_rtl_kill_timer(m_alarmID);
    lib_rtl_set_timer(1000*tmo, TmoAST, this, &m_alarmID);
  }
  m_bCancel = false;
}
// ----------------------------------------------------------------------------
//  Cancel timer
//                                      M.Frank
// ----------------------------------------------------------------------------
void NetworkChannel::stopTimer()  {
  lib_rtl_kill_timer(m_alarmID);
  m_alarmID = 0;
}
