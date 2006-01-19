#ifndef __TcpNetworkChannel_H__
#define __TcpNetworkChannel_H__

/// Definition of the TCP NetworkChannel data structure
/**
        {\Large{\bf Class TcpNetworkChannel}}


  Networking object based on TCP sockets, which allows sending and receiving with timeouts

*/
#include "NET/NetworkChannel.h"
class EventHandler;

class TcpNetworkChannel : public NetworkChannel {
protected:
    struct IOSB  {
      unsigned short condition;          /* I/O status code           */
      unsigned short count;              /* Number of Bytes I/O'ed    */
      unsigned int   information;        /* Device specific info      */
    };
    //@Man: protected variables
    /// Accept IOSB:
    IOSB m_iosb;
    /// Dummy receive buffer
    int  m_recvBuff;
    /// Default ACTION after AST callback -> Call handler callback....
    static int _DefaultAction ( void* par );    
public:
    //@Man: public member functions
    /// Standard Constructor to initialize the connection for CONNECTOR
    TcpNetworkChannel();
    /// Constructor to initialize the connection for ACCEPTOR
    TcpNetworkChannel(Channel channel);
    /// Destructor: closes channel
    ~TcpNetworkChannel();
    /// Connect to network partner (Connector)
    int _Connect ( const Address&  addr, int tmo = 0);
    /// Bind Address + Listen to specified connection (Acceptor)
    int _Bind( const Address& addr, int con_pend = 5 );
    /// Accept connection on socket (Acceptor)
    Channel _Accept( Address& addr, int tmo = 0 );
    /// Send data to network partner.
    int _Send  (void* buff, int len, int tmo = 0, int flags = 0, const Address* addr = 0);
    /// Receive data from network partner.
    int _Recv  (void* buff, int len, int tmo = 0, int flags = 0, Address* addr = 0);
    /// Queue Accept call 
    int _QueueAccept  ( Port port, EventHandler *handler );
    /// Queue Receive call 
    int _QueueReceive ( Port port, EventHandler *handler );
    /// Queue Receive call
    int _UnqueueIO ( Port port );
    /// Cancel eventually pending I/O requests
    int _Cancel();
};
#endif  /* __TcpNetworkChannel_H__  */
