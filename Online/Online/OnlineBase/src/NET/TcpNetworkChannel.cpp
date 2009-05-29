#include <cstdio>
#include <cstdlib>
#include "RTL/rtl.h"
#include "NET/defs.h"
#include "CPP/EventHandler.h"
#include "NET/TcpNetworkChannel.h"
#include "NET/IOPortManager.h"

// ----------------------------------------------------------------------------
//  Constructor: initialize network connection for CONNECTOR.
//                                      M.Frank
// ----------------------------------------------------------------------------
TcpNetworkChannel::TcpNetworkChannel() {
  m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
  if ( m_socket < 0 )  {
    m_errno = ::lib_rtl_socket_error();
    return;
  }
  struct linger Linger;
  Linger.l_onoff  = 0;
  Linger.l_linger = 0;
  ::setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (const char*)&Linger, sizeof(Linger));
  m_bValid = true;
}
// ----------------------------------------------------------------------------
//  Constructor: initialize network connection for ACCEPTOR.
//                                      M.Frank
// ----------------------------------------------------------------------------
TcpNetworkChannel::TcpNetworkChannel(Channel channel) {
  m_bValid = channel>0;
  m_socket = channel;
}
// ----------------------------------------------------------------------------
//  Destructor: close socket, cancel eventually pending timers
//                                      M.Frank
// ----------------------------------------------------------------------------
TcpNetworkChannel::~TcpNetworkChannel()  {
  //stopTimer();
  if ( m_socket > 0 ) {
    ::shutdown(m_socket,2);
    ::socket_close (m_socket);
  }
}
// ----------------------------------------------------------------------------
//  o Bind Address (Acceptor)
//  o listen to specified connection (Acceptor)
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::bind ( const Address& addr, int con_pend )  {
  if ( m_socket > 0 )  {
    m_errno = 0;
    int status, on = 1;
    status = ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
    status = ::setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(on));
    status = ::setsockopt(m_socket, SOL_SOCKET, SO_OOBINLINE, (const char*)&on, sizeof(on));
    status = ::bind ( m_socket, (sockaddr*)&addr, sizeof(addr) );
    if ( status < 0 )  {
      m_errno = ::lib_rtl_socket_error();
      return status;
    }
    status = ::listen( m_socket, con_pend );
    if ( status < 0 )  {
      m_errno = ::lib_rtl_socket_error();
      return status;
    }
    return status;
  }
  return (-1);
}
// ----------------------------------------------------------------------------
//  Accept connection on socket (Acceptor)
//                                      M.Frank
// ----------------------------------------------------------------------------
NetworkChannel::Channel TcpNetworkChannel::accept ( Address& addr, int tmo )  {
  if ( m_socket > 0 )  {
    socklen_t size = sizeof(addr);
    startTimer(tmo);
    Channel accepted = ::accept(m_socket,(sockaddr*)&addr, &size );
    stopTimer();
    if ( !m_bCancel ) m_errno = (accepted <= 0) ? ::lib_rtl_socket_error() : 0;
    if ( accepted > 0 )   {
      struct linger Linger;
      int status, on  = 1;
      Linger.l_onoff  = 0;
      Linger.l_linger = 0;
      status = ::setsockopt( accepted, SOL_SOCKET, SO_REUSEADDR,(const char*)&on, sizeof(on));
      status = ::setsockopt( accepted, SOL_SOCKET, SO_BROADCAST,(const char*)&on, sizeof(on));
      status = ::setsockopt( accepted, SOL_SOCKET, SO_OOBINLINE,(const char*)&on, sizeof(on));
      status = ::setsockopt( accepted, SOL_SOCKET, SO_LINGER,   (const char*)&Linger,sizeof(Linger));
    }
    return accepted;
  }
  return (-1);
}
// ----------------------------------------------------------------------------
//  Connect to network partner   (Connector)
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::connect ( const Address& addr, int tmo )  {
  if ( m_socket > 0 )  {
    startTimer(tmo);
    int status = ::connect ( m_socket, (sockaddr*)&addr, sizeof(addr) );
    stopTimer();
    if ( !m_bCancel ) m_errno = (status < 0) ? ::lib_rtl_socket_error() : 0;
    return status;
  }
  return (-1);
}
// ----------------------------------------------------------------------------
//  send data to network partner.
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::send  (const void* buff, int len, int tmo, int flags, const Address* addr )    {
  if ( m_socket > 0 )  {
    int status, addr_len = sizeof(Address);
    startTimer(tmo);
    if ( addr == 0 )
      status = ::send(m_socket,(char*)buff,len,flags);
    else
      status = ::sendto(m_socket,(char*)buff,len,flags,(sockaddr*)addr,addr_len);
    stopTimer();
    m_errno = (status <= 0) ? ::lib_rtl_socket_error() : 0;
    return status;
  }
  return 0;
}
// ----------------------------------------------------------------------------
//  receive data from network partner.
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::recv  (void* buff, int len, int tmo, int flags, Address* addr)   {
  socklen_t addr_len = sizeof(Address);
  int status = selectTmo(READ|EXCEPT,tmo);
  if ( status == 1 ) {
    if ( addr == 0 )
      status = ::recv(m_socket, (char*)buff, len, flags );
    else
      status = ::recvfrom(m_socket, (char*)buff, len, flags, (sockaddr*)addr, &addr_len);
    if ( !m_bCancel ) m_errno = (status <= 0) ? ::lib_rtl_socket_error() : 0;
  }
  return status;
}
// ----------------------------------------------------------------------------
//  Set send buffer size
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::setSendBufferSize(int len) {
  int status = ::setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF,(const char*)&len, sizeof(len));
  return status;
}

// ----------------------------------------------------------------------------
//  Queue Accept request on accept socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::queueAccept ( Port port, EventHandler* handler )  {
  return IOPortManager(m_port=port).add(0, m_socket, _defaultAction, handler);
}
// ----------------------------------------------------------------------------
//  Queue receive request on receive socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::queueReceive ( Port port, EventHandler* handler )  {
  return IOPortManager(m_port=port).add(1, m_socket, _defaultAction, handler);
}
// ----------------------------------------------------------------------------
//  Unqueue receive request on receive socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_unqueueIO ( Port port )  {
  return IOPortManager(m_port=port).remove(m_socket);
}
// ----------------------------------------------------------------------------
// Cancel eventually pending I/O requests
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::cancel()  {
  return ::lib_rtl_cancel_io(m_socket);
}
// ----------------------------------------------------------------------------
//  Default ACTION after AST callback
//  Call handler callback....
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_defaultAction ( void* par )  {
  EventHandler* handler = (EventHandler*)par;
  return handler->handleEvent();
}
