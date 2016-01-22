// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================

// System / Boost include files

// Framework include files
#include "ASIO/Socket.h"
#include "ASIO/TanInterface.h"
#include "TAN/TanMessage.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <iostream>

using namespace BoostAsio;
using boost::asio::ip::tcp;
using namespace boost;

/// Standard constructor
Socket::Socket(int)
  : m_io(), m_socket(m_io), m_error(0) 
{
}

/// Default destructor
Socket::~Socket()   {
  system::error_code ec;
  m_socket.shutdown(tcp::socket::shutdown_both,ec);
  m_socket.close(ec);
}

int Socket::connect(const TanMessage& msg)  {
  char addr_name[64], port_name[64];
  const TanMessage::SubAddress& sa = msg.address();
  const unsigned char* a = reinterpret_cast<const unsigned char*>(&sa.s_addr);
  ::snprintf(addr_name,sizeof(addr_name),"%d.%d.%d.%d",a[0],a[1],a[2],a[3]);
  ::snprintf(port_name,sizeof(port_name),"%d",msg.port());
  return this->connect(addr_name, port_name);
}

int Socket::connect(const char* addr, const char* port)   {
  try {
    tcp::resolver resolver(m_io);
    tcp::resolver::query query(addr, port);
    asio::connect(m_socket, resolver.resolve(query));
    m_socket.set_option(asio::socket_base::reuse_address(true));
    m_socket.set_option(asio::socket_base::linger(true,0));
    m_error = 0;
    return 0;
  }
  catch(const std::exception& e)   {
    m_error = errno;
    return -1;
  }
  m_error = errno;
  return -1;
}

bool Socket::isCancelled() const  {
  return m_flags != 0; 
}

int Socket::send(const void* buffer, size_t len)   {
  try {
    return asio::write(m_socket, asio::buffer(buffer, len));
  }
  catch(const std::exception& e)   {
    m_error = errno;
    return -1;
  }
  m_error = errno;
  return -1;
}

int Socket::recv(void* buffer, size_t len, int tmo)   {
  system::error_code ec;
  struct pollfd fds;
  try {
    fds.events  = POLLIN;
    fds.revents = 0;
    fds.fd      = m_socket.native();
    tmo = (tmo<0) ? -1 : tmo*1000;
    tmo = -1;
    ::poll(&fds,1,tmo);
    if ( fds.revents&POLLIN )  {
      return asio::read(m_socket,asio::buffer(buffer, len), ec);
    }
  }
  catch (std::exception& e)   {
    ::lib_rtl_output(LIB_RTL_OS,"Boost::asio: exception: %s\n",e.what());
  }
  m_error = errno;
  return -1;
}

int Socket::close()   {
  system::error_code ec;
  m_socket.close(ec);
  return 0;
}

int Socket::setsockopt(int what, void* value, size_t len)   {
  switch(what)  {
  case TanInterface::Asio_TCP_KEEPALIVE:
    m_socket.set_option(asio::socket_base::keep_alive((*(int*)value) ? true : false));
    return 0;
  default:
    return len > 0 ? -1 : -1;
  }
}

