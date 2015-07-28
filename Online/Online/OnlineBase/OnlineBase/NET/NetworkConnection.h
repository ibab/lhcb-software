#ifndef __NETWORKCONNECTION__H__
#define __NETWORKCONNECTION__H__
#include "RTL/rtl.h"
#include "CPP/SmartObject.h"
#include "NET/NetworkChannel.h"

// Forward declarations
class BasicRequest;

/**@class NetworkAddress
 */
class NetworkAddress  {
public:
  NetworkAddress() {
  }
  virtual ~NetworkAddress() {
  }
};

/// Defintion of the basic interface of ANY networking connection.
/**
   {\Large{\bf Class NetworkConnection}}
   Basic interface of ANY networking connection.

   This is a tricky: NetworkAddress should not be used
   as a variable type at all!
   Instead use the defined variable type of the specific
   connection class:
   e.g.   UdpConnection::Address  new_address;
   The wrapped address type contains the required structure
   and only this structure, i.e. the meory mapping and
   size is identical.

*/
class NetworkConnection  : public SmartObject<NetworkConnection> {
public:
  //@Man: Class specific enumerations and typedefs
  /// Socket  :== NetworkChannel::Channel
  typedef NetworkChannel::Channel Socket;
  /// Port    :== NetworkChannel::Port
  typedef NetworkChannel::Port    Port;
  /// Family  :== NetworkChannel::Family
  typedef NetworkChannel::Family  Family;
  /// Status enum of the networking connection
  enum NetConnectionStatus {
    NETCONNECTION_ERROR = 0,
    NETCONNECTION_SUCCESS
  };
  /// Pointer keeping the name of the service
  char                m_service[64];
protected:
public:
  //@Man: Public member functions
  /// Standard constructor
  NetworkConnection() {
    m_service[0] = 0;
    m_status = NETCONNECTION_ERROR;
  }
  /// Standard destructor
  virtual ~NetworkConnection() {
  }
  /// Return name to the service the connection represents
  const char* Service() const   {
    return m_service;
  }
  /// Address the connection points to (may be invalid)
  virtual const NetworkAddress& address () const = 0;
  /// Virtual method to manipulate the channel
  virtual NetworkChannel& sendChannel ()  = 0;
  /// Virtual method to manipulate the channel
  virtual NetworkChannel& recvChannel ()  = 0;
  /// Virtual method to receive data.
  virtual int receive  (BasicRequest* req, NetworkAddress& addr) = 0;
  /// Virtual method to send data
  virtual int send     (BasicRequest* req, NetworkAddress& addr) = 0;
};

#endif
