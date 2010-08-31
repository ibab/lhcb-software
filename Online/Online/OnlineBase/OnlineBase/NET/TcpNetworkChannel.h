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
    int connect ( const Address&  addr, int tmo = 0);
    /// Bind Address + listen to specified connection (Acceptor)
    int bind( const Address& addr, int con_pend = 5 );
    /// Accept connection on socket (Acceptor)
    Channel accept( Address& addr, int tmo = 0 );
    /// Set send buffer size
    int setSendBufferSize(int len);
    /// send data to network partner.
    int send  (const void* buff, int len, int tmo = 0, int flags = 0, const Address* addr = 0);
    /// receive data from network partner.
    int recv  (void* buff, int len, int tmo = 0, int flags = 0, Address* addr = 0);
    /// Queue Accept call 
    int queueAccept  ( Port port, EventHandler *handler );
    /// Queue receive call 
    int queueReceive ( Port port, EventHandler *handler );
    /// Queue receive call
    int _unqueueIO ( Port port );
    /// Cancel eventually pending I/O requests
    int cancel();
};
#endif  /* __TcpNetworkChannel_H__  */
