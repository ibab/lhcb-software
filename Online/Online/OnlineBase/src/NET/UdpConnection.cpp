#include <ctime>
#include <cstdio>
#include <cstring>
#include "NET/UdpConnection.h"
#include "CPP/BasicRequest.h"

static inline const char* timestr(void)    {
  time_t curr;
  time(&curr);
  char *tim = ctime(&curr);
  tim[24] = 0;
  return tim;
}

void UdpNetworkAddress::setHostName() {
  // ----------------------------------------------------------------------------
  // Return the hostname this address is pointing to.
  //  - Only valid for valid receive addresses
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  struct hostent *he = ::gethostbyaddr((char*)&m_addr.sin_addr,sizeof(m_addr.sin_addr),AF_INET);
  ::strncpy(m_cHost, ( he == 0 ) ? ::inet_ntoa(m_addr.sin_addr) : he->h_name, sizeof(m_cHost));
  m_cHost[sizeof(m_cHost)-1]=0;
}

UdpConnection::UdpConnection ( const char* service )  {
  // ----------------------------------------------------------------------------
  //
  //  Standard handling of a UDP socket creation:
  //  
  //  - Get port number from service nam
  //  - Initialize
  //                                      M.Frank
  // 
  // ----------------------------------------------------------------------------
  ::strncpy(m_service,service,sizeof(m_service));
  m_service[sizeof(m_service)-1]=0;
  initialize( (Port)servicePort(Service()) );
}

UdpConnection::UdpConnection ( UdpConnection::Port port )  {
  // ----------------------------------------------------------------------------
  //
  //  Standard handling of a UDP socket creation:
  //  
  //  - Initialize
  //                                      M.Frank
  // 
  // ----------------------------------------------------------------------------
  ::snprintf(m_service,sizeof(m_service),"UDPservice_%d",port);
  initialize( htons(port) );
}

int UdpConnection::initialize( UdpConnection::Port port )  {
  // ----------------------------------------------------------------------------
  //
  //  Initialisation of UDP socket:
  //  
  //  - Create socket,
  //  - Set socket optins
  //  - Bind the socket
  //
  //                                      M.Frank
  // 
  // ----------------------------------------------------------------------------
  // int on=1, sndbuf= 512, rcvbuf=128;
  m_status = CONNECTION_ERROR;
  if ( !m_channel.isValid() )  return m_status;
  //  Create a "sockaddr_in" structure which describes the port we
  //  want to listen to. Address INADDR_ANY means we will accept
  //  connections to any of our local IP addresses.
  //
  m_sin.m_addr.sin_port = port;
  m_sin.m_addr.sin_family = AF_INET;
  m_sin.m_addr.sin_addr.s_addr = INADDR_ANY;
  ::memset(m_sin.m_addr.sin_zero,0,sizeof(m_sin.m_addr.sin_zero));
  //  Bind to that address...
  if ( m_channel.bind(m_sin.m_addr) < 0 ) {
    ::lib_rtl_output(LIB_RTL_ERROR,"%s UdpConnection> Error binding address:%s\n",timestr(),m_channel.errMsg());
    return m_status;
  }
  return m_status = CONNECTION_SUCCESS;
}

UdpConnection::~UdpConnection() {
  // ----------------------------------------------------------------------------
  //
  //  Standard handling of a UDP socket deletion:
  //  
  //                                      M.Frank
  // 
  // ----------------------------------------------------------------------------
}

int UdpConnection::receive(BasicRequest* req, NetworkAddress& origine)  {
  // ----------------------------------------------------------------------------
  //
  //  receive data from UDP
  //  
  //                                      M.Frank
  // 
  // ----------------------------------------------------------------------------
  int flags = 0, size  = req->MaxBuffSize();
  UdpNetworkAddress *from = (UdpNetworkAddress*)&origine;
  int status = m_channel.recv(req->Buffer(), size, 0, flags, &from->m_addr);
  if ( status <= 0 )  {
    //  ----------------------------  D E B U G -----------------------------
    m_status = m_channel.error();
    ::lib_rtl_output(LIB_RTL_ERROR,"%s  UdpConnection::receive> Bad IO status. Status=0x%X %s\n",
                     timestr(),m_channel.error(),m_channel.errMsg());
    return CONNECTION_ERROR;
  }
  return CONNECTION_SUCCESS;
}

int UdpConnection::send(BasicRequest* req, NetworkAddress& target)  {
  // ----------------------------------------------------------------------------
  //
  //  receive data from UDP
  //  
  //                                      M.Frank
  // 
  // ----------------------------------------------------------------------------
  int       flags = 0;
  socklen_t size  = req->BuffSize();
  UdpNetworkAddress *to = (UdpNetworkAddress*)&target;
  int status = m_channel.send(req->Buffer(), size, 0, flags, &to->m_addr);
  if ( status <= 0 )  {
    //  ----------------------------  D E B U G -----------------------------
    m_status = m_channel.error();
    ::lib_rtl_output(LIB_RTL_ERROR,"%s  UdpConnection::send> Bad IO status. Status=0x%X %s\n",
                     timestr(),m_channel.error(),m_channel.errMsg());
    return CONNECTION_ERROR;
  }
  return CONNECTION_SUCCESS;
}
// ----------------------------------------------------------------------------
// return Network channel
// ----------------------------------------------------------------------------
NetworkChannel& UdpConnection::recvChannel()  {
  return m_channel;
}
// ----------------------------------------------------------------------------
// return Network channel
// ----------------------------------------------------------------------------
NetworkChannel& UdpConnection::sendChannel()  {
  return m_channel;
}
// ----------------------------------------------------------------------------
// Return name to the service the connection represents
// ----------------------------------------------------------------------------
const char* UdpConnection::Service() const  {
  return m_service;
}
// ----------------------------------------------------------------------------
// return Port number
// ----------------------------------------------------------------------------
UdpConnection::Port UdpConnection::port () const {
  return m_sin.m_addr.sin_port;
}
// ----------------------------------------------------------------------------
// Address the connection points to (may be invalid)
// ----------------------------------------------------------------------------
const NetworkAddress& UdpConnection::address () const {
  return m_sin;
}
// ----------------------------------------------------------------------------
// Address the connection points to (may be invalid)
// ----------------------------------------------------------------------------
NetworkChannel::Address& UdpConnection::_InAddress ()  {
  return m_sin.m_addr;
}
// ----------------------------------------------------------------------------
// Return family type
// ----------------------------------------------------------------------------
UdpConnection::Family UdpConnection::family () const {
  return m_sin.m_addr.sin_family;
}
// ----------------------------------------------------------------------------
// Standard constructor with given service name
// ----------------------------------------------------------------------------
int UdpConnection::servicePort(const char* service)   {
  struct servent* se = ::getservbyname( service, "udp");
  if ( se == 0 )   {
    ::lib_rtl_output(LIB_RTL_ERROR,"%s UdpConnection> Error getting service %s!\n",timestr(),service);
    return -1;
  }
  return se->s_port;
}

