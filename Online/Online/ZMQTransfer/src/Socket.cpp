#include "ZMQTransfer/Context.h"
#include "ZMQTransfer/Socket.h"
#include "TAN/TanMessage.h"

using namespace ZMQ;

/// Standard constructor
Socket::Socket(int type_)
  : zmq::socket_t(zmq().context(),type_), m_error(0) 
{
}

/// Default destructor
Socket::~Socket()   {
}

int Socket::connect(const std::string& addr_name)  {
  try {
    this->zmq::socket_t::connect(addr_name.c_str());
    m_error = 0;
    return 0;
  }
  catch(const std::exception& e)   {
    m_error = ::zmq_errno();
    return -1;
  }
  m_error = ::zmq_errno();
  return -1;
}

bool Socket::isCancelled() const  {
  return m_flags != 0; 
}

int Socket::send(const void* buffer, size_t len)   {
  try {
    return this->zmq::socket_t::send(buffer, len);
  }
  catch(const std::exception& e)   {
    m_error = ::zmq_errno();
    return -1;
  }
  m_error = ::zmq_errno();
  return -1;
}

int Socket::recv(void* buffer, size_t len, int tmo)   {
  zmq::pollitem_t item[1];
  item[0].fd = 0;
  item[0].socket = *this;
  item[0].events = ZMQ_POLLIN|ZMQ_POLLERR;
  item[0].revents = 0;
  tmo = (tmo<0) ? -1 : tmo*1000;
  tmo = -1;
  int nclient = zmq::poll(item,1,tmo);
  if ( nclient == 1 && (item[0].revents&ZMQ_POLLIN) == ZMQ_POLLIN )  {
    zmq_msg_t frame;
    int rc = ::zmq_msg_init(&frame);
    if ( 0 == rc )  {
      rc = ::zmq_msg_recv(&frame, *this, 0);
      if ( sizeof(TanMessage) == rc )  {
        const char* msg_data = (char*)::zmq_msg_data(&frame);
        size_t msg_len = ::zmq_msg_size(&frame);
        ::memcpy(buffer, msg_data, msg_len<len ? msg_len : len);
        return msg_len;
      }
    }
  }
  m_error = ::zmq_errno();
  return -1;
}
