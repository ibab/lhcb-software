#include <ctime>
#include <cstdio>
#include <cstring>
#include "CPP/BasicRequest.h"
#include "NET/TcpConnection.h"

inline const char* timestr(void)    {
  time_t curr;
  time(&curr);
  char *tim = ctime(&curr);
  tim[24] = 0;
  return tim;
}

void TcpNetworkAddress::setHostName() {
  // ----------------------------------------------------------------------------
  // Return the hostname this address is pointing to.
  //  - Only valid for valid receive addresses
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  struct hostent *he = ::gethostbyaddr((char*)&m_addr.sin_addr,sizeof(m_addr.sin_addr),AF_INET);
  ::strncpy(m_cHost, ( he == 0 ) ? ::inet_ntoa(m_addr.sin_addr) : he->h_name, sizeof(m_cHost));
  m_cHost[sizeof(m_cHost)-1]=0;
}

TcpConnection::TcpConnection ( const char* service )  {
  // ----------------------------------------------------------------------------
  //  Standard handling of a UDP socket creation:
  //  - Get port number from service nam
  //  - Initialize
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  m_status = CONNECTION_ERROR;
  ::strncpy(m_service,service,sizeof(m_service));
  m_service[sizeof(m_service)-1]=0;
  initialize( Port(servicePort(Service())) );
}

TcpConnection::TcpConnection ( TcpConnection::Port port )  {
  // ----------------------------------------------------------------------------
  //  Standard handling of a UDP socket creation:
  //  - Initialize
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  ::snprintf(m_service,sizeof(m_service),"TCPservice_%d",port);
  initialize( htons(port) );
}

int TcpConnection::initialize( TcpConnection::Port port )  {
  // ----------------------------------------------------------------------------
  //  Standard handling of a Tcp socket creation:
  //  - Create socket,
  //  - Set socket optins
  //  - Bind the socket
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  m_status = CONNECTION_ERROR;
  if ( !m_channel.isValid() )  return m_status;
  //  Create a "sockaddr_in" structure which describes the port we
  //  want to listen to. Address INADDR_ANY means we will accept
  //  connections to any of our local IP addresses.
  //
  m_sin.m_addr.sin_family       = AF_INET;
  m_sin.m_addr.sin_port         = port;
  m_sin.m_addr.sin_addr.s_addr  = INADDR_ANY;
  ::memset(m_sin.m_addr.sin_zero,0,sizeof(m_sin.m_addr.sin_zero));
  //  Bind to that address...
  if ( m_channel.bind(m_sin.m_addr) < 0 ) {
    ::lib_rtl_output(LIB_RTL_ERROR,"%s TcpConnection> Error BIND:%s\n",timestr(),m_channel.errMsg());
    return CONNECTION_ERROR;
  }
  return m_status = CONNECTION_SUCCESS;
}

TcpConnection::~TcpConnection() {
  // ----------------------------------------------------------------------------
  //  Standard handling of a Tcp socket deletion:
  //  - close
  //                                      M.Frank
  // ----------------------------------------------------------------------------
}

int TcpConnection::listen ( EventHandler *handler )  {
  // ----------------------------------------------------------------------------
  //  Enable accepts on the socket
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  return m_channel.queueAccept( port(), handler );
}

int TcpConnection::receive(BasicRequest* req, NetworkAddress& org)  {
  // ----------------------------------------------------------------------------
  //  receive data from Tcp
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  int flags = 0, size  = req->MaxBuffSize();
  TcpNetworkAddress *from = (TcpNetworkAddress*)&org;
  int status = m_channel.recv(req->Buffer(), size, 0, flags, &from->m_addr);
  if ( status <= 0 )  {
    //  ----------------------------  D E B U G -----------------------------
    m_status = m_channel.error();
    ::lib_rtl_output(LIB_RTL_ERROR,"%s  TcpConnection::receive> Bad IO status. Status=0x%X %s\n",
                     timestr(),m_channel.error(),m_channel.errMsg());
    return CONNECTION_ERROR;
  }
  return CONNECTION_SUCCESS;
}

int TcpConnection::send(BasicRequest* req, NetworkAddress& target)  {
  // ----------------------------------------------------------------------------
  //  receive data from Tcp
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  int flags = 0, size  = req->BuffSize();
  TcpNetworkAddress *to = (TcpNetworkAddress*)&target;
  int status = m_channel.send(req->Buffer(),size,0,flags,&to->m_addr);
  if ( status <= 0 )  {
    //  ----------------------------  D E B U G -----------------------------
    m_status = m_channel.error();
    ::lib_rtl_output(LIB_RTL_ERROR,"%s  TcpConnection::send> Bad IO status. Status=0x%X %s\n",
                     timestr(),m_channel.error(),m_channel.errMsg());
    return CONNECTION_ERROR;
  }
  return CONNECTION_SUCCESS;
}
// ----------------------------------------------------------------------------
// return Network channel
// ----------------------------------------------------------------------------
NetworkChannel& TcpConnection::recvChannel()  {
  return m_channel;
}
// ----------------------------------------------------------------------------
// return Network channel
// ----------------------------------------------------------------------------
NetworkChannel& TcpConnection::sendChannel()  {
  return m_channel;
}
// ----------------------------------------------------------------------------
// return Port number
// ----------------------------------------------------------------------------
TcpConnection::Port TcpConnection::port () const {
  return m_sin.m_addr.sin_port;
}
// ----------------------------------------------------------------------------
// Address the connection points to (may be invalid)
// ----------------------------------------------------------------------------
const NetworkAddress& TcpConnection::address () const {
  return m_sin;
}
// ----------------------------------------------------------------------------
// Return family type
// ----------------------------------------------------------------------------
TcpConnection::Family TcpConnection::family () const {
  return m_sin.m_addr.sin_family;
}
// ----------------------------------------------------------------------------
// Standard constructor with given service name
// ----------------------------------------------------------------------------
int TcpConnection::servicePort(const char* service)   {
  struct servent* se = ::getservbyname( service, "tcp");
  if ( se == 0 )   {
    ::lib_rtl_output(LIB_RTL_ERROR,"%s TcpConnection> Error getting service %s!\n",timestr(),service);
    return -1;
  }
  return se->s_port;
}

