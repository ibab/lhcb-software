#ifndef __UDPCONNECTION__H__
#define __UDPCONNECTION__H__

#include "NET/defs.h"
//
//======================================================================
//
// This is a tricky: NetworkAddress should not be used
// as a variable type at all!
// Instead use the defined variable type of the specific
// connection class:
// e.g.   UdpConnection::Address  new_address;
// The wrapped address type contains the required structure
// and only this structure, i.e. the meory mapping and
// size is identical. Obviously socket calls should not be 
// exported.
//
//======================================================================
//
#include "NET/UdpNetworkChannel.h"
#include "NET/NetworkConnection.h"
#include <cstring>
//
//
/// Definition of the NetworkAddress data structure for UDP sockets
/** 
    {\Large{\bf Class NetworkAddress}}

    Small wrapper containing the inet socket address.
*/
class UdpNetworkAddress : public NetworkAddress  {
  //@Man: Friend classes
  /// Friend NetworkConnection
  friend class NetworkConnection;
  friend class UdpConnection;
public:
  //@Man: Class specific enumerations and typedefs
  /// Address data type
  typedef sockaddr_in Data;
protected:
  //@Man: Protected member variables
  /// Structure holding an inet address
  Data         m_addr;
  //@Man: Protected member variables
  /// Name of network connection host
  char         m_cHost[64];
public:
  //@Man: Public member functions
  /// Automatic cast to the data object
  operator Data& () {
    return m_addr;
  }
  /// Retrieve Name of Network Host
  const char* hostName() const {
    return m_cHost;
  }
  /// Store Name of Network host
  void setHostName();
};

/// Definition of a network connection based on UDP sockets
/**
 *  {\Large{\bf Class UdpConnection}}
 *
 *  Definition of the UdpConnection class.
 */
class UdpConnection  : public NetworkConnection {
public:
  //@Man: Class specific enumerations and typedefs
  /// Address :== NetworkAddress
  typedef UdpNetworkAddress Address;
  /// Indicate status of the connection request
  enum ConnectionStatus {
    CONNECTION_ERROR   = NetworkConnection::NETCONNECTION_ERROR,
    CONNECTION_SUCCESS = NetworkConnection::NETCONNECTION_SUCCESS
  };
protected:
  //@Man: Protected member variables
  /// Storage for the address
  Address             m_sin;
  /// UDP Network channel 
  UdpNetworkChannel   m_channel;
  /// UDP Network channel for sending
  UdpNetworkChannel   _send_channel;
  /// Pointer keeping the name of the service
  char                m_service[64];
public:
  //@Man: Public member functions
  /// Standard constructor with given service name
  explicit UdpConnection( const char* service = "UserService" );
  /// Standard constructor with given port number
  explicit UdpConnection( Port port );
  /// Standard destructor
  virtual ~UdpConnection();
  /// Initialize the connection (called by constructor)
  int initialize( Port port );
  /// Return name to the service the connection represents
  const char* Service() const;
  /// Return a copy of the address of this connection
  const NetworkAddress& address() const;
  /// Return a copy of the address of this connection
  NetworkChannel::Address& _InAddress();
  /// return Network channel for Sending
  NetworkChannel& sendChannel ();
  /// return Network channel for Receiving
  NetworkChannel& recvChannel();
  /// return Port number
  Port port () const;
  /// Return family type
  Family family () const;
  /// Virtual method to receive data from a given UDP address
  virtual int receive  (BasicRequest* req, NetworkAddress& from);
  /// Virtual method to send data to a given UDP address
  virtual int send     (BasicRequest* req, NetworkAddress& to);
  /// Standard constructor with given service name
  static int servicePort(const char* service = "UserService");
};
#endif
