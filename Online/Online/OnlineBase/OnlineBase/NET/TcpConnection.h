#ifndef __TcpCONNECTION__H__
#define __TcpCONNECTION__H__

#include "NET/defs.h"
//======================================================================
//
// This is a tricky: NetworkAddress should not be used
// as a variable type at all!
// Instead use the defined variable type of the specific
// connection class:
// e.g.   TcpConnection::Address  new_address;
// The wrapped address type contains the required structure
// and only this structure, i.e. the meory mapping and
// size is identical. Obviously socket calls should not be 
// exported.
//
//======================================================================
//
#include <cstring>
#include "NET/NetworkConnection.h"
#include "NET/TcpNetworkChannel.h"
//

/// Definition of the NetworkAddress data structure for TCP sockets
/** 
 *        {\Large{\bf Class NetworkAddress}}
 *
 *   Small wrapper containing the inet socket address.
 *
 *  \author  M.Frank
 *  \version 1.0
 */
class TcpNetworkAddress : public NetworkAddress {
public:
  //@Man: Class specific enumerations and typedefs
  /// Address data type
  typedef TcpNetworkChannel::Address Data;
  //@Man: Public member variables
  /// Structure holding an inet address
  Data m_addr;
  /// Name of network connection host
  char m_cHost[64];
  //@Man: Public member functions
  /// Automatic cast to the data object
  operator Data& () {
    return m_addr;
  }
  /// Retrieve Name of Network Host
  const char* hostName() const {
    return m_cHost;
  }
  void setHostName();
};

/// Definition of a network connection based on TCP sockets
/**
 *    {\Large{\bf Class TcpConnection}}
 *
 *  Definition of the TcpConnection class.
 *
 *  \author  M.Frank
 *  \version 1.0
 */
class TcpConnection  : public NetworkConnection {
public:
  //@Man: Class specific enumerations and typedefs
  /// Address :== NetworkAddress
  typedef TcpNetworkAddress Address;
  /// Indicate status of the connection request
  enum ConnectionStatus {
    CONNECTION_ERROR   = NetworkConnection::NETCONNECTION_ERROR,
    CONNECTION_SUCCESS = NetworkConnection::NETCONNECTION_SUCCESS
  };
protected:
  //@Man: Protected member variables
  /// Storage for the address
  Address	            m_sin;
  /// TCP Network channel
  TcpNetworkChannel   m_channel;
public:
  //@Man: Public member functions
  /// Standard constructor with given service name
  explicit TcpConnection( const char* service = "UserService" );
  /// Standard constructor with given port number
  explicit TcpConnection( Port port );
  /// Standard destructor
  virtual ~TcpConnection();
  /// Initialize the connection (called by constructor(s))
  int initialize( Port port );
  /// return Network channel 
  virtual NetworkChannel& sendChannel ();
  /// return Network channel 
  virtual NetworkChannel& recvChannel ();
  /// return Port number
  Port port () const;
  /// Return family type
  Family family () const;
  /// Address the connection points to (may be invalid)
  virtual const NetworkAddress& address () const;
  /// Virtual method to receive data
  virtual int receive  (BasicRequest* req, NetworkAddress&from);
  /// Virtual method to send data
  virtual int send     (BasicRequest* req, NetworkAddress& to);
  /// Virtual method to listen to the connection socket
  virtual int listen( CPP::EventHandler *handler );
  /// Standard constructor with given service name
  static int servicePort(const char* service = "UserService");
};
#endif
