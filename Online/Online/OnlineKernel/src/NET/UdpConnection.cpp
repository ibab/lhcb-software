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

void UdpNetworkAddress::SetHostName() {
  // ----------------------------------------------------------------------------
  // Return the hostname this address is pointing to.
  //  - Only valid for valid receive addresses
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  struct hostent *he = ::gethostbyaddr((char*)&_addr.sin_addr,sizeof(_addr.sin_addr),AF_INET);
  ::strcpy(_cHost, ( he == 0 ) ? inet_ntoa(_addr.sin_addr) : he->h_name);
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
  ::strcpy(m_service,service);
  Initialize ( servicePort(Service()) );
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
  ::sprintf(m_service,"UDPservice_%d",port);
  Initialize ( htons(port) );
}

int UdpConnection::Initialize ( UdpConnection::Port port )  {
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
  if ( !m_channel._IsValid() )  return m_status;
  //  Create a "sockaddr_in" structure which describes the port we
  //  want to listen to. Address INADDR_ANY means we will accept
  //  connections to any of our local IP addresses.
  //
  m_sin._addr.sin_port = port;
  m_sin._addr.sin_family = AF_INET;
  m_sin._addr.sin_addr.s_addr = INADDR_ANY;
  ::memset(m_sin._addr.sin_zero,0,sizeof(m_sin._addr.sin_zero));
  //  Bind to that address...
  if ( m_channel._Bind(m_sin._addr) < 0 ) {
    ::printf("%s UdpConnection> Error binding address:%s\n",timestr(),m_channel._ErrMsg());
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

int UdpConnection::Receive(BasicRequest* req, NetworkAddress& origine)  {
  // ----------------------------------------------------------------------------
  //
  //  Receive data from UDP
  //  
  //                                      M.Frank
  // 
  // ----------------------------------------------------------------------------
  int flags = 0, size  = req->MaxBuffSize();
  UdpNetworkAddress *from = (UdpNetworkAddress*)&origine;
  int status = m_channel._Recv(req->Buffer(), size, 0, flags, &from->_addr);
  if ( status <= 0 )  {
    //  ----------------------------  D E B U G -----------------------------
    m_status = m_channel._Error();
    printf("%s  UdpConnection::Receive> Bad IO status. Status=0x%X %s\n",
      timestr(),m_channel._Error(),m_channel._ErrMsg());
    return CONNECTION_ERROR;
  }
  return CONNECTION_SUCCESS;
}

int UdpConnection::Send(BasicRequest* req, NetworkAddress& target)  {
  // ----------------------------------------------------------------------------
  //
  //  Receive data from UDP
  //  
  //                                      M.Frank
  // 
  // ----------------------------------------------------------------------------
  int       flags = 0;
  socklen_t size  = req->BuffSize();
  UdpNetworkAddress *to = (UdpNetworkAddress*)&target;
  int status = m_channel._Send(req->Buffer(), size, 0, flags, &to->_addr);
  if ( status <= 0 )  {
    //  ----------------------------  D E B U G -----------------------------
    m_status = m_channel._Error();
    ::printf("%s  UdpConnection::Send> Bad IO status. Status=0x%X %s\n",
      timestr(),m_channel._Error(),m_channel._ErrMsg());
    return CONNECTION_ERROR;
  }
  return CONNECTION_SUCCESS;
}
// ----------------------------------------------------------------------------
// return Network channel
// ----------------------------------------------------------------------------
NetworkChannel& UdpConnection::_RecvChannel()  {
  return m_channel;
}
// ----------------------------------------------------------------------------
// return Network channel
// ----------------------------------------------------------------------------
NetworkChannel& UdpConnection::_SendChannel()  {
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
UdpConnection::Port UdpConnection::_Port () const {
  return m_sin._addr.sin_port;
}
// ----------------------------------------------------------------------------
// Address the connection points to (may be invalid)
// ----------------------------------------------------------------------------
const NetworkAddress& UdpConnection::_Address () const {
  return m_sin;
}
// ----------------------------------------------------------------------------
// Address the connection points to (may be invalid)
// ----------------------------------------------------------------------------
NetworkChannel::Address& UdpConnection::_InAddress ()  {
  return m_sin._addr;
}
// ----------------------------------------------------------------------------
// Return family type
// ----------------------------------------------------------------------------
UdpConnection::Family UdpConnection::_Family () const {
  return m_sin._addr.sin_family;
}
// ----------------------------------------------------------------------------
// Standard constructor with given service name
// ----------------------------------------------------------------------------
int UdpConnection::servicePort(const char* service)   {
  struct servent* se = ::getservbyname( service, "udp");
  if ( se == 0 )   {
    printf("%s UdpConnection> Error getting service %s!\n",timestr(),service);
    return -1;
  }
  return se->s_port;
}

