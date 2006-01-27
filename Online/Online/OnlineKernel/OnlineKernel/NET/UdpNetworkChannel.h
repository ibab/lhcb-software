#ifndef __UdpNetworkChannel_H__
#define __UdpNetworkChannel_H__
#ifdef __cplusplus
#include "RTL/QIO.h"

/// Definition of the UDP NetworkChannel data structure
/**
{\Large{\bf Class UdpNetworkChannel}}


Networking object based on UDP sockets, which allows sending and receiving with timeouts

*/
#include "NET/NetworkChannel.h"
class EventHandler;

class UdpNetworkChannel : public NetworkChannel {
protected:
  /// Accept IOSB:
  IOSB_t m_iosb;
  /// Dummy receive buffer
  int  m_recvBuff;
  /// Default ACTION after AST callback -> Call handler callback....
  static int  _DefaultAction ( void* par );    
public:
  //@Man: public member functions
  /// Constructor to initialize the connection
  UdpNetworkChannel();
  /// Destructor: closes channel
  virtual ~UdpNetworkChannel();
  /// Cancel eventually pending I/O requests
  int _Cancel();
  /// Bind Address (Acceptor)
  int _Bind( const Address& addr, int con_pend = 5 );
  /// Connect to target
  int _Connect( const Address& addr, int tmo = 0);
  /// Send data to network partner.
  int _Send  (void* buff, int len, int tmo = 0, int flags = 0, const Address* addr = 0);
  /// Receive data from network partner.
  int _Recv  (void* buff, int len, int tmo = 0, int flags = 0, Address* addr = 0);
  /// Queue Receive call 
  int _QueueReceive ( Port port, EventHandler *handler );
  /// Queue Receive call
  int _UnqueueIO ( Port port );
  /// Standard constructor with given service name
  static int servicePort(const char* service = "UserService");
};
#endif   /*   __cplusplus               */
#endif   /*   __UdpNetworkChannel_H__   */
