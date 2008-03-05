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
    ::lib_rtl_output(LIB_RTL_ERROR,"UdpNetworkChannel> socket(AF_INET,SOCK_DGRAM):%s\n",errMsg());
    m_errno = ::lib_rtl_socket_error();
  }
  else if (::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
    ::lib_rtl_output(LIB_RTL_ERROR,"UdpNetworkChannel> setsockopt(SO_REUSEADDR):%s\n",errMsg());
    m_errno = ::lib_rtl_socket_error();
  }
  else if (::setsockopt(m_socket, SOL_SOCKET, SO_DONTROUTE, (char*)&on, sizeof(on)) < 0) {
    ::lib_rtl_output(LIB_RTL_ERROR,"UdpNetworkChannel> setsockopt(SO_DONTROUTE):%s\n",errMsg());
    m_errno = ::lib_rtl_socket_error();
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
  //stopTimer();
  if ( m_socket > 0 ) {
    ::shutdown(m_socket,2);
    ::socket_close (m_socket);
  }
}
// ----------------------------------------------------------------------------
//  send data to network partner.
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::send  (const void* buff, int len, int tmo, int flags, const Address* addr)    {
  if ( m_socket > 0 )  {
    int status;
    socklen_t siz = sizeof(Address);
    startTimer(tmo);
    if ( addr == 0 )
      status = ::send ( m_socket, (char*)buff, len, flags);
    else 
      status = ::sendto ( m_socket, (char*)buff, len, flags, (sockaddr*)addr, siz);
    stopTimer();
    if ( status != len )  m_errno = ::lib_rtl_socket_error();
    return status;
  }
  return 0;
}
// ----------------------------------------------------------------------------
//  receive data from network partner.
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::recv  (void* buff, int len, int tmo, int flags, Address* addr)   {
  socklen_t siz = sizeof(Address);
  int status = selectTmo(READ|EXCEPT,tmo);
  if ( status == 1 ) {
    if ( addr == 0 )
      status = ::recv(m_socket, (char*)buff, len, flags);  
    else
      status = ::recvfrom(m_socket, (char*)buff, len, flags, (sockaddr*)addr, &siz);  
    m_errno = (status < 0 || status != len) ? ::lib_rtl_socket_error() : 0;
  }
  return status;
}

// ----------------------------------------------------------------------------
//  Connect to network partner  (Connector)
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::connect ( const Address& addr, int tmo )  {
  if ( m_socket > 0 )  {
    startTimer(tmo);
    int status = ::connect(m_socket, (sockaddr*)&addr, sizeof(addr) );
    stopTimer();
    if ( !m_bCancel ) m_errno = (status < 0) ? ::lib_rtl_socket_error() : 0;
    return status;
  }
  return (-1);
}

// ----------------------------------------------------------------------------
//  Bind Address (Acceptor)
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::bind ( const Address& addr, int /* pend */ )  {
  if ( m_socket > 0 )  {
    int status = ::bind ( m_socket, (sockaddr*)&addr, sizeof(addr) );
    m_errno = (status < 0) ? ::lib_rtl_socket_error() : 0;
    return status;
  }
  return (-1);
}
// ----------------------------------------------------------------------------
// Cancel eventually pending I/O requests
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::cancel()  {
  return ::lib_rtl_cancel_io(m_socket);
}
// ----------------------------------------------------------------------------
//  Queue receive request on receive socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::queueReceive ( Port port, EventHandler* handler )  {
  return IOPortManager(m_port=port).add(1, m_socket, _defaultAction, handler);
}
// ----------------------------------------------------------------------------
//  Unqueue receive request on receive socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_unqueueIO ( Port port )  {
  return IOPortManager(m_port=port).remove(m_socket);
}
// ----------------------------------------------------------------------------
//  Default ACTION after AST callback
//  Call handler callback....
//                                      M.Frank
// ----------------------------------------------------------------------------
int UdpNetworkChannel::_defaultAction ( void* par )  {
  EventHandler* handler = (EventHandler*)par;
  return handler->handleEvent();
}
