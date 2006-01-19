#include <cstring>
#include <cerrno>
#include "NET/NetworkChannel.h"
#include "RTL/rtl.h"
#include "RTL/QIO.h"

#if defined(_WIN32)
#include "Winsock.h"
#include <stdexcept>
namespace {
  struct __init__ {
    __init__()  {
      static WSADATA g_WSAData;
      memset(&g_WSAData, 0, sizeof(WSADATA));
      if (WSAStartup ( MAKEWORD(1,1), &g_WSAData) != 0)    {
        throw std::runtime_error("NetworkChannel: WSAStartup failed!");
      }
    }
  };
  static __init__ g_init;
}
#endif

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
    StopTimer();
  }
}
// ----------------------------------------------------------------------------
//  Return error string
//                                      M.Frank
// ----------------------------------------------------------------------------
const char* NetworkChannel::_ErrMsg()  {
  return lib_rtl_error_message(m_errno);
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
void NetworkChannel::StartTimer( int tmo )  {
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
void NetworkChannel::StopTimer()  {
  lib_rtl_kill_timer(m_alarmID);
  m_alarmID = 0;
}
