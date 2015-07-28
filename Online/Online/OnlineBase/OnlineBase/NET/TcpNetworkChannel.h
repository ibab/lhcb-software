#ifndef ONLINEBASE_NET_TcpNetworkChannel_H__
#define ONLINEBASE_NET_TcpNetworkChannel_H__

#include "CPP/EventHandler.h"
#include "NET/NetworkChannel.h"

/// Definition of the TCP NetworkChannel data structure
/**
 *  {\Large{\bf Class TcpNetworkChannel}}
 *
 *  Networking object based on TCP sockets, which allows sending and receiving with timeouts
 *
 * 
 *  \author  M.Frank
 *  \version 1.0
 */
class TcpNetworkChannel : public NetworkChannel {
public:
  struct IOSB  {
    unsigned short condition;          /* I/O status code           */
    unsigned short count;              /* Number of Bytes I/O'ed    */
    unsigned int   information;        /* Device specific info      */
  };
protected:
  //@Man: protected variables
  /// Accept IOSB:
  IOSB m_iosb;
  /// Dummy receive buffer
  int  m_recvBuff;
  /// Default ACTION after AST callback -> Call handler callback....
  static int _defaultAction ( void* par );    
public:
  //@Man: public member functions
  /// Standard Constructor to initialize the connection for CONNECTOR
  TcpNetworkChannel();
  /// Constructor to initialize the connection for ACCEPTOR
  explicit TcpNetworkChannel(Channel channel);
  /// Destructor: closes channel
  ~TcpNetworkChannel();
  /// Connect to network partner (Connector)
  virtual int connect ( const Address&  addr, int tmo = 0);
  /// Bind Address + listen to specified connection (Acceptor)
  virtual int bind( const Address& addr, int con_pend = 5 );
  /// Accept connection on socket (Acceptor)
  virtual Channel accept( Address& addr, int tmo = 0 );
  /// Set send buffer size
  int setSendBufferSize(int len);
  /// send data to network partner.
  virtual int send  (const void* buff, int len, int tmo = 0, int flags = 0, const Address* addr = 0);
  /// receive data from network partner.
  virtual int recv  (void* buff, int len, int tmo = 0, int flags = 0, Address* addr = 0);
  /// Queue Accept call 
  int queueAccept  ( Port port, CPP::EventHandler *handler );
  /// Queue receive call 
  int queueReceive ( Port port, CPP::EventHandler *handler );
  /// Queue receive call
  int _unqueueIO ( Port port );
  /// Cancel eventually pending I/O requests
  virtual int cancel();
  /// Set socket option to reuse address in case of re-creation
  virtual int reuseAddress()  const;
};
#endif  /* ONLINEBASE_NET_TcpNetworkChannel_H__  */
