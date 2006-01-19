#include <cstdio>
#include <cstdlib>
#include "RTL/rtl.h"
#include "NET/defs.h"
#include "CPP/EventHandler.h"
#include "NET/TcpNetworkChannel.h"
#include "NET/IOPortManager.h"

#define TCP_errno errno

// ----------------------------------------------------------------------------
//  Constructor: initialize network connection for CONNECTOR.
//                                      M.Frank
// ----------------------------------------------------------------------------
TcpNetworkChannel::TcpNetworkChannel() {
  m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
  if ( m_socket < 0 )  {
    m_errno = TCP_errno;
    return;
  }
  struct linger Linger;
  Linger.l_onoff = 1;
  Linger.l_linger = 0;
  setsockopt(m_socket, SOL_SOCKET, SO_LINGER,     (const char*)&Linger, sizeof(Linger));
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
  StopTimer();
  ::shutdown(m_socket,2);
  ::socket_close (m_socket);
}
// ----------------------------------------------------------------------------
//  o Bind Address (Acceptor)
//  o Listen to specified connection (Acceptor)
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_Bind ( const Address& addr, int con_pend )  {
  if ( m_socket > 0 )  {
    m_errno = 0;
    int status, on = 1;
    status = ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
    status = ::setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(on));
    status = ::setsockopt(m_socket, SOL_SOCKET, SO_OOBINLINE, (const char*)&on, sizeof(on));
    status = ::bind ( m_socket, (sockaddr*)&addr, sizeof(addr) );
    if ( status < 0 )  {
      m_errno = TCP_errno;
      return status;
    }
    status = ::listen( m_socket, con_pend );
    if ( status < 0 )  {
      m_errno = TCP_errno;
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
NetworkChannel::Channel TcpNetworkChannel::_Accept ( Address& addr, int tmo )  {
  if ( m_socket > 0 )  {
    int size = sizeof(addr);
    StartTimer(tmo);
    NetworkChannel::Channel accepted = ::accept ( m_socket, (sockaddr*)&addr, &size );
    StopTimer();
    if ( !m_bCancel ) m_errno = (accepted <= 0) ? TCP_errno : 0;
    if ( accepted > 0 )   {
      int status, buf_siz = 1024, on = 1;
      struct linger Linger;
      Linger.l_onoff  = 1;
      Linger.l_linger = 0;
      status = ::setsockopt( accepted, SOL_SOCKET, SO_LINGER,    (const char*)&Linger,  sizeof(Linger));
      status = ::setsockopt( accepted, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(on));
      status = ::setsockopt( accepted, SOL_SOCKET, SO_OOBINLINE, (const char*)&on, sizeof(on));
    }
    return accepted;
  }
  return (-1);
}
// ----------------------------------------------------------------------------
//  Connect to network partner   (Connector)
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_Connect ( const Address& addr, int tmo )  {
  if ( m_socket > 0 )  {
    StartTimer(tmo);
    int status = ::connect ( m_socket, (sockaddr*)&addr, sizeof(addr) );
    if ( !m_bCancel ) m_errno = (status < 0) ? TCP_errno : 0;
    StopTimer();
    return status;
  }
  return (-1);
}
// ----------------------------------------------------------------------------
//  Send data to network partner.
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_Send  (void* buff, int len, int tmo, int flags, const Address* addr )    {
  if ( m_socket > 0 )  {
    int status, addr_len = sizeof(NetworkChannel::Address);
    StartTimer(tmo);
    if ( addr == 0 )
      status = ::send(m_socket,(char*)buff,len,flags);
    else
      status = ::sendto(m_socket,(char*)buff,len,flags,(sockaddr*)addr,addr_len);
    m_errno = (status <= 0) ? TCP_errno : 0;
    StopTimer();
    return status;
  }
  return 0;
}
// ----------------------------------------------------------------------------
//  Receive data from network partner.
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_Recv  (void* buff, int len, int tmo, int flags, Address* addr)   {
  int status, addr_len = sizeof(NetworkChannel::Address);
  StartTimer(tmo);
  if ( addr == 0 )
    status = ::recv(m_socket, (char*)buff, len, flags );
  else
    status = ::recvfrom(m_socket, (char*)buff, len, flags, (sockaddr*)addr, &addr_len);
  if ( !m_bCancel ) m_errno = (status <= 0) ? TCP_errno : 0;
  StopTimer();
  return status;
}
// ----------------------------------------------------------------------------
//  Queue Accept request on accept socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_QueueAccept ( Port port, EventHandler* handler )  {
  return IOPortManager(m_port=port).add(0, m_socket, _DefaultAction, handler);
}
// ----------------------------------------------------------------------------
//  Queue Receive request on receive socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_QueueReceive ( Port port, EventHandler* handler )  {
  return IOPortManager(m_port=port).add(1, m_socket, _DefaultAction, handler);
}
// ----------------------------------------------------------------------------
//  Unqueue Receive request on receive socket
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_UnqueueIO ( Port port )  {
  return IOPortManager(m_port=port).remove(m_socket);
}
// ----------------------------------------------------------------------------
// Cancel eventually pending I/O requests
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_Cancel()  {
  return ::lib_rtl_cancel_io(m_socket);
}
// ----------------------------------------------------------------------------
//  Default ACTION after AST callback
//  Call handler callback....
//                                      M.Frank
// ----------------------------------------------------------------------------
int TcpNetworkChannel::_DefaultAction ( void* par )  {
  EventHandler* handler = (EventHandler*)par;
  return handler->HandleEvent();
}
