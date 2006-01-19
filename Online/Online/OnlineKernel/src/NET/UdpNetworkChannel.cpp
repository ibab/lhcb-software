#include <cstdio>
#include "NET/defs.h"
#include "CPP/EventHandler.h"
#include "NET/IOPortManager.h"
#include "NET/UdpNetworkChannel.h"

// ----------------------------------------------------------------------------
//  Constructor: initialize network connection.
//                                      M.Frank
// ----------------------------------------------------------------------------
UdpNetworkChannel::UdpNetworkChannel()  {
  int on = 1;
  m_errno = 0;
  m_socket = ::socket(PF_INET, SOCK_DGRAM, 0);
  if ( m_socket <= 0 )  {
    ::printf("UdpNetworkChannel> socket(AF_INET,SOCK_DGRAM):%s\n",_ErrMsg());
    m_errno = lib_rtl_get_error();
  }
  else if (::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
    ::printf("UdpNetworkChannel> setsockopt(SO_REUSEADDR):%s\n",_ErrMsg());
    m_errno = lib_rtl_get_error();
  }
  else if (::setsockopt(m_socket, SOL_SOCKET, SO_DONTROUTE, (char*)&on, sizeof(on)) < 0) {
    ::printf("UdpNetworkChannel> setsockopt(SO_DONTROUTE):%s\n",_ErrMsg());
    m_errno = lib_rtl_get_error();
  }
  else  {
    m_bValid = true;
  }
}
// ----------------------------------------------------------------------------
//  Destructor: close socket, cancel eventually pending timers
//                                      M.Frank
// ----------------------------------------------------------------------------
UdpNetworkChannel::~UdpNetworkChannel()  {
  StopTimer();
  ::shutdown(m_socket,2);
  ::socket_close (m_socket);
}
// ----------------------------------------------------------------------------
//  Send data to network partner.
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_Send  (void* buff, int len, int tmo, int flags, const Address* addr)    {
  if ( m_socket > 0 )  {
    int status;
    socklen_t siz = sizeof(Address);
    StartTimer(tmo);
    if ( addr == 0 )
      status = ::send ( m_socket, (char*)buff, len, flags);
    else 
      status = ::sendto ( m_socket, (char*)buff, len, flags, (sockaddr*)addr, siz);
    if ( status != len )  m_errno = ::lib_rtl_get_error();
    StopTimer();
    return status;
  }
  return 0;
}
// ----------------------------------------------------------------------------
//  Receive data from network partner.
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_Recv  (void* buff, int len, int tmo, int flags, Address* addr)   {
  int status;
  socklen_t siz = sizeof(Address);
  StartTimer(tmo);
  if ( addr == 0 )
    status = ::recv(m_socket, (char*)buff, len, flags);  
  else
    status = ::recvfrom(m_socket, (char*)buff, len, flags, (sockaddr*)addr, &siz);  
  m_errno = (status < 0 || status != len) ? ::lib_rtl_get_error() : 0;
  StopTimer();
  return status;
}

// ----------------------------------------------------------------------------
//  Connect to network partner  (Connector)
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_Connect ( const Address& addr, int tmo )  {
  if ( m_socket > 0 )  {
    StartTimer(tmo);
    int status = ::connect(m_socket, (sockaddr*)&addr, sizeof(addr) );
    if ( !m_bCancel ) m_errno = (status < 0) ? ::lib_rtl_get_error() : 0;
    StopTimer();
    return status;
  }
  return (-1);
}

// ----------------------------------------------------------------------------
//  Bind Address (Acceptor)
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_Bind ( const Address& addr, int /* pend */ )  {
  if ( m_socket > 0 )  {
    int status = ::bind ( m_socket, (sockaddr*)&addr, sizeof(addr) );
    m_errno = (status < 0) ? ::lib_rtl_get_error() : 0;
    return status;
  }
  return (-1);
}
// ----------------------------------------------------------------------------
// Cancel eventually pending I/O requests
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_Cancel()  {
  return ::lib_rtl_cancel_io(m_socket);
}
// ----------------------------------------------------------------------------
//  Queue Receive request on receive socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_QueueReceive ( Port port, EventHandler* handler )  {
  return IOPortManager(m_port=port).add(1, m_socket, _DefaultAction, handler);
}
// ----------------------------------------------------------------------------
//  Unqueue Receive request on receive socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_UnqueueIO ( Port port )  {
  return IOPortManager(m_port=port).remove(m_socket);
}
// ----------------------------------------------------------------------------
//  Default ACTION after AST callback
//  Call handler callback....
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_DefaultAction ( void* par )  {
  EventHandler* handler = (EventHandler*)par;
  return handler->HandleEvent();
}
