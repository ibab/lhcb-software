#ifndef ZMQTRANSFER_ZMQCONTEXT_H
#define ZMQTRANSFER_ZMQCONTEXT_H

#include "zmq/zmq.hpp"

namespace ZMQ  {

  class Context  {
  public:
    Context() {}
    static zmq::context_t& context();
    static zmq::socket_t socket(int typ);
    static void setNumberOfThreads(int nthreads);
  };
  inline Context zmq()  { return Context(); }
}       // End namespace ZMQ     
#endif  /* ZMQTRANSFER_ZMQCONTEXT_H  */
