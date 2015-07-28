#ifndef __UdpNetworkChannel_H__
#define __UdpNetworkChannel_H__
#ifdef __cplusplus
#include "RTL/QIO.h"
#include "CPP/EventHandler.h"
#include "NET/NetworkChannel.h"

/// Definition of the UDP NetworkChannel data structure
/**
 *   {\Large{\bf Class UdpNetworkChannel}}
 *
 *
 *  Networking object based on UDP sockets, which allows sending and receiving with timeouts
 *
 * 
 *  \author  M.Frank
 *  \version 1.0
 */
class UdpNetworkChannel : public NetworkChannel {
protected:
  /// Accept IOSB:
  IOSB_t m_iosb;
  /// Dummy receive buffer
  int  m_recvBuff;
  /// Default ACTION after AST callback -> Call handler callback....
  static int  _defaultAction ( void* par );    
public:
  //@Man: public member functions
  /// Constructor to initialize the connection
  UdpNetworkChannel();
  /// Destructor: closes channel
  virtual ~UdpNetworkChannel();
  /// Cancel eventually pending I/O requests
  int cancel();
  /// Bind Address (Acceptor)
  int bind( const Address& addr, int con_pend = 5 );
  /// Connect to target
  int connect( const Address& addr, int tmo = 0);
  /// send data to network partner.
  int send  (const void* buff, int len, int tmo = 0, int flags = 0, const Address* addr = 0);
  /// receive data from network partner.
  int recv  (void* buff, int len, int tmo = 0, int flags = 0, Address* addr = 0);
  /// Queue receive call 
  int queueReceive ( Port port, CPP::EventHandler *handler );
  /// Queue receive call
  int _unqueueIO ( Port port );
  /// Standard constructor with given service name
  //static int servicePort(const char* service = "UserService");
};
#endif   /*   __cplusplus               */
#endif   /*   __UdpNetworkChannel_H__   */
