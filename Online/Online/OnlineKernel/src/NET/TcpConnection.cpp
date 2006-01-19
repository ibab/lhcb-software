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

void TcpNetworkAddress::SetHostName() {
  // ----------------------------------------------------------------------------
  // Return the hostname this address is pointing to.
  //  - Only valid for valid receive addresses
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  struct hostent *he = ::gethostbyaddr((char*)&_addr.sin_addr,sizeof(_addr.sin_addr),AF_INET);
  ::strcpy(_cHost, ( he == 0 ) ? ::inet_ntoa(_addr.sin_addr) : he->h_name);
}

TcpConnection::TcpConnection ( const char* service )  {
  // ----------------------------------------------------------------------------
  //  Standard handling of a UDP socket creation:
  //  - Get port number from service nam
  //  - Initialize
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  m_status = CONNECTION_ERROR;
  _pcc_service = new char[strlen(service)+1];
  ::strcpy(_pcc_service,service);
  struct servent* se = ::getservbyname( Service(), "udp");
  m_sin._addr.sin_port = 0;
  if ( se == 0 )   {
    printf("%s UdpConnection> Error getting service %s!\n",timestr(),Service());
    return;
  }
  Initialize ( m_sin._addr.sin_port = se->s_port );
}

TcpConnection::TcpConnection ( TcpConnection::Port port )  {
  // ----------------------------------------------------------------------------
  //  Standard handling of a UDP socket creation:
  //  - Initialize
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  char service[32];
  ::sprintf(service,"TCPservice_%d",port);
  _pcc_service = new char[strlen(service)+1];
  ::strcpy(_pcc_service,service);
  Initialize ( htons(port) );
}

int TcpConnection::Initialize ( TcpConnection::Port port )  {
  // ----------------------------------------------------------------------------
  //  Standard handling of a Tcp socket creation:
  //  - Create socket,
  //  - Set socket optins
  //  - Bind the socket
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  m_status = CONNECTION_ERROR;
  if ( !m_channel._IsValid() )  return m_status;
  //  Create a "sockaddr_in" structure which describes the port we
  //  want to listen to. Address INADDR_ANY means we will accept
  //  connections to any of our local IP addresses.
  //
  m_sin._addr.sin_family       = AF_INET;
  m_sin._addr.sin_port         = port;
  m_sin._addr.sin_addr.s_addr  = INADDR_ANY;
  ::memset(m_sin._addr.sin_zero,0,sizeof(m_sin._addr.sin_zero));
  //  Bind to that address...
  if ( m_channel._Bind(m_sin._addr) < 0 ) {
    printf("%s TcpConnection> Error BIND:%s\n",timestr(),m_channel._ErrMsg());
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

int TcpConnection::Listen ( EventHandler *handler )  {
  // ----------------------------------------------------------------------------
  //  Enable accepts on the socket
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  return m_channel._QueueAccept( _Port(), handler );
}

int TcpConnection::Receive(BasicRequest* req, NetworkAddress& origine, Iosb& ios)  {
  // ----------------------------------------------------------------------------
  //  Receive data from Tcp
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  int flags = 0, size  = req->MaxBuffSize();
  TcpNetworkAddress *from = (TcpNetworkAddress*)&origine;  
  int status = m_channel._Recv(req->Buffer(), size, 0, flags, &from->_addr);
  if ( status <= 0 )  {
    //  ----------------------------  D E B U G -----------------------------
    m_status = m_channel._Error();
    ::printf("%s  UdpConnection::Receive> Bad IO status. Status=0x%X %s\n",
      timestr(),m_channel._Error(),m_channel._ErrMsg());
    return CONNECTION_ERROR;
  }
  return CONNECTION_SUCCESS;
}

int TcpConnection::Send(BasicRequest* req, NetworkAddress& target, Iosb& ios)  {
  // ----------------------------------------------------------------------------
  //  Receive data from Tcp
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  int flags = 0, size  = req->BuffSize();
  TcpNetworkAddress *to = (TcpNetworkAddress*)&target;
  int status = m_channel._Send(req->Buffer(),size,0,flags,&to->_addr);
  if ( status <= 0 )  {
    //  ----------------------------  D E B U G -----------------------------
    m_status = m_channel._Error();
    ::printf("%s  TcpConnection::Send> Bad IO status. Status=0x%X %s\n",
      timestr(),m_channel._Error(),m_channel._ErrMsg());
    return CONNECTION_ERROR;
  }
  return CONNECTION_SUCCESS;
}
// ----------------------------------------------------------------------------
/// return Network channel
// ----------------------------------------------------------------------------
NetworkChannel& TcpConnection::_RecvChannel()  {
  return m_channel;
}
// ----------------------------------------------------------------------------
/// return Network channel
// ----------------------------------------------------------------------------
NetworkChannel& TcpConnection::_SendChannel()  {
  return m_channel;
}
// ----------------------------------------------------------------------------
/// Return name to the service the connection represents
// ----------------------------------------------------------------------------
const char* TcpConnection::Service() const  {
  return _pcc_service;
}
// ----------------------------------------------------------------------------
/// return Port number
// ----------------------------------------------------------------------------
TcpConnection::Port TcpConnection::_Port () const {
  return m_sin._addr.sin_port;
}
// ----------------------------------------------------------------------------
/// Address the connection points to (may be invalid)
// ----------------------------------------------------------------------------
const NetworkAddress& TcpConnection::_Address () const {
  return m_sin;
}
// ----------------------------------------------------------------------------
/// Return family type
// ----------------------------------------------------------------------------
TcpConnection::Family TcpConnection::_Family () const {
  return m_sin._addr.sin_family;
}
