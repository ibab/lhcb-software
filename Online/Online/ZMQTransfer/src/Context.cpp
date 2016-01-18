#include "ZMQTransfer/Context.h"

static int s_num_threads = 1;
static zmq::context_t* s_context = 0;

zmq::context_t& ZMQ::Context::context()   {
  if ( 0 == s_context )   {
    s_context = new zmq::context_t{s_num_threads};
  }
  return *s_context;
}

zmq::socket_t ZMQ::Context::socket(int typ)   {
  return zmq::socket_t(context(),typ);
}

void ZMQ::Context::setNumberOfThreads(int nthreads)   {
  s_num_threads = nthreads;
}

