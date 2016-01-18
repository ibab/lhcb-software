#ifndef ZMQTRANSFER_ZMQSOCKET_H
#define ZMQTRANSFER_ZMQSOCKET_H

#include "zmq/zmq.hpp"

class TanMessage;

namespace ZMQ {
  class Socket : public zmq::socket_t  {
    int m_error, m_flags;
  public:
    /// Standard constructor
    Socket(int type_);
    /// Default destructor
    virtual ~Socket();

    int error() const  { return m_error; }
    bool isCancelled() const;
    int connect(const std::string& binding);
    int send(const void* buffer, size_t len);
    int recv(void* buffer, size_t len, int tmo);
  };
}       // End namespace ZMQ     
#endif  /* ZMQTRANSFER_ZMQSOCKET_H  */
