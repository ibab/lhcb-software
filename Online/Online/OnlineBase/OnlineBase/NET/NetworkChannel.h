#ifndef __NetworkChannel_H__
#define __NetworkChannel_H__

#include "NET/defs.h"

#ifdef __cplusplus

/// Definition of the general purpose NetworkChannel data structure
/** @class NetworkChannel NetworkChannel.h NET/NetworkChannel.h
 *
 * General purpose networking object, which allows sending and receiving with timeouts
 */
class NetworkChannel  {
public:
  //@Man: Public type definitions:
  /// Family   == __NetworkFamily__;
  typedef __NetworkFamily__  Family;
  /// Channel  == __NetworkChannel__;
  typedef __NetworkChannel__ Channel;
  /// Port     == __NetworkPort__;
  typedef __NetworkPort__    Port;
  /// Address  == __NetworkAddress__;
  typedef __NetworkAddress__ Address;

  enum { READ=1<<1, WRITE=1<<2, EXCEPT=1<<3 };

protected:
  //@Man: protected member variables
  /// Boolean indicating that the timer fired and the I/O go cancelled
  bool m_bCancel;
  /// Boolean indicating the validity of the connection after initialisation
  bool m_bValid;
  /// Channel to hold the socket connection 
  Channel       m_socket;
  /// Port number
  Port          m_port;
  /// Alarm identifier
  unsigned long m_alarmID;
  /// Buffer to store status code
  unsigned int  m_errno;
  //@Man: protected member functions
  /// Start timer before receiving data, if you don't want to wait forever
  void startTimer( int tmo );    
  /// Cancel timer
  void stopTimer ();
public:
  //@Man: public member functions
  /// Default constructor
  NetworkChannel();
  /// Virtual destructor
  virtual ~NetworkChannel();
  /// AST called on timeout for receiving data on socket
  static int TmoAST(void* par);
  /// Retieve cancel flag
  bool isCancelled()   const {
    return m_bCancel;
  }
  /// Indicate if the connection is valid
  bool isValid() const  {
    return m_bValid;
  }
  /// Return errno of socket
  unsigned int error() const   {
    return m_errno;
  }
  /// Return channel/path number
  Channel channel() const  {
    return m_socket;
  }
  /// Return Error string
  const char* errMsg();

  /// Abstract method to cancel eventually pending I/O requests
  virtual int cancel() = 0;
  /// Abstract method to Bind Address (Acceptor)
  virtual int bind   (const Address& addr, int con_pend = 5) = 0;
  /// Abstract method to Connect to target
  virtual int connect(const Address& addr, int tmo = 0) = 0;
  /// Abstract method: send data on network
  virtual int send   (const void* buff, int len, int tmo = 0, int flags = 0, const Address* addr = 0) = 0;
  /// Abstract method: receive data from network.
  virtual int recv   (void* buff, int len, int tmo = 0, int flags = 0, Address* addr = 0) = 0;
  /// Set socket option to reuse address in case of re-creation (default implementation is empty)
  virtual int reuseAddress()  const;
  /// Set Select TMO on receive.
  virtual int selectTmo(int flags, int tmo);
};
#endif  /* __cplusplus           */
#endif   /* __NetworkChannel_H__  */
