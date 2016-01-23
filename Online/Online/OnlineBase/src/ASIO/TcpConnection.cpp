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
// This code was developed based on the Boost asio library examples.
// See http://www.boost.org for details
// Credits: Christopher M. Kohlhoff (chris at kohlhoff dot com)
//==========================================================================

// Framework include files
#include "ASIO/TcpServer.h"
#include "RTL/rtl.h"

// System / Boost include files
#include <memory>
#include <boost/bind.hpp>

namespace asio  {
  using namespace boost::asio;
}

using namespace BoostAsio;

/// Construct a connection with the given io_service.
TcpConnection::TcpConnection(asio::io_service& io_service, 
			     TcpRequestHandler::pointer_t& handler)
  : m_strand(io_service), m_socket(io_service), m_handler(handler), m_param(0)
{
}

/// Default destructor
TcpConnection::~TcpConnection()   {
  if ( m_handler )  {
    m_handler->handleClose(*this);
  }
}

/// Connect to target destination
TcpConnection& TcpConnection::connect(const char* dest, unsigned short port)   {
  char port_str[64];
  ::snprintf(port_str,sizeof(port_str),"%d",int(htons(port)));
  asio::ip::tcp::resolver::query query(dest, port_str);
  asio::ip::tcp::resolver resolver(m_socket.get_io_service());
  asio::connect(m_socket, resolver.resolve(query));
  //m_socket.connect(resolver.resolve(query));
  m_socket.set_option(socket_t::reuse_address(true));
  m_socket.set_option(socket_t::linger(true,0));
  return *this;
}

/// Initiate graceful connection closure.
void TcpConnection::shutdown()   {
  if ( m_socket.is_open() )  {
    error_t ignored_ec;
    m_socket.shutdown(socket_t::shutdown_both, ignored_ec);
  }
}

/// Handle completion of a optional asynchonous unlocked read operation.
void TcpConnection::handle_additional_read(const error_t& /* error */, 
					   size_t /* bytes_transferred */)  {
}

/// Construct a connection with the given io_service.
SyncTcpConnection::SyncTcpConnection(boost::asio::io_service& io_service,
				     TcpRequestHandler::pointer_t& handler,
				     size_t recvSize)
  : TcpConnection(io_service, handler), m_recvSize(recvSize)
{
  m_recv_buffer.resize(recvSize);
}
    
/// Default destructor
SyncTcpConnection::~SyncTcpConnection()   {
}

/// Start the first asynchronous operation for the connection.
void SyncTcpConnection::start()  {
  m_socket.set_option(socket_t::linger(true,0));
  m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			   m_strand.wrap(boost::bind(&TcpConnection::handle_read,
						     shared_from_this(),
						     asio::placeholders::error,
						     asio::placeholders::bytes_transferred)));
}

/// Handle completion of a asynchonous unlocked read/write operation.
void SyncTcpConnection::handle_read(const error_t& error, size_t bytes_transferred)  {
  if ( !error )  {
    if ( bytes_transferred > 0 )    {
      TcpRequestHandler::Reply   reply;
      TcpRequestHandler::Request request(m_recv_buffer.data(), bytes_transferred);
      m_handler->handle(*this,request, reply);
      if ( !reply.empty() )  {
	asio::async_write(m_socket, reply,
			  m_strand.wrap(boost::bind(&TcpConnection::handle_write,
						    shared_from_this(),
						    asio::placeholders::error)));
	return;
      }
    }
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     m_strand.wrap(boost::bind(&TcpConnection::handle_read,
						       shared_from_this(),
						       asio::placeholders::error,
						       asio::placeholders::bytes_transferred)));
    return;
  }
  shutdown();
}

/// Handle completion of a asynchonous write operation.
void SyncTcpConnection::handle_write(const error_t& error)  {
  if (!error)  {
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     m_strand.wrap(boost::bind(&TcpConnection::handle_read, shared_from_this(),
						       asio::placeholders::error,
						       asio::placeholders::bytes_transferred)));
  }
  else   {
    // No new asynchronous operations are started. This means that all shared_ptr
    // references to the connection object will disappear and the object will be
    // destroyed automatically after this handler returns. The connection class's
    // destructor closes the socket.

    // Initiate graceful connection closure.
    error_t ignored_ec;
    m_socket.shutdown(socket_t::shutdown_both, ignored_ec);
  }
}

/// Construct a connection with the given io_service.
AsyncTcpConnection::AsyncTcpConnection(boost::asio::io_service& io_service,
				       TcpRequestHandler::pointer_t& handler,
				       size_t recvSize)
  : TcpConnection(io_service, handler), m_recvSize(recvSize)
{
  m_recv_buffer.resize(recvSize);
}
    
/// Default destructor
AsyncTcpConnection::~AsyncTcpConnection()   {
}

/// Start the first asynchronous operation for the connection.
void AsyncTcpConnection::start()  {
  m_socket.set_option(socket_t::linger(true,0));
  m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			   boost::bind(&TcpConnection::handle_read,
				       shared_from_this(),
				       asio::placeholders::error,
				       asio::placeholders::bytes_transferred));
}

/// Handle completion of a asynchonous unlocked read/write operation.
void AsyncTcpConnection::handle_read(const error_t& error, size_t bytes_transferred)  {
  if ( !error )  {
    if ( bytes_transferred > 0 )    {
      TcpRequestHandler::Reply   reply;
      TcpRequestHandler::Request request(m_recv_buffer.data(), bytes_transferred);
      m_handler->handle(*this, request, reply);
      if ( !reply.empty() )  {
	asio::async_write(m_socket, reply,
			  boost::bind(&TcpConnection::handle_write,
				      shared_from_this(),
				      asio::placeholders::error));
	return;
      }
    }
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     boost::bind(&TcpConnection::handle_read,
					 shared_from_this(),
					 asio::placeholders::error,
					 asio::placeholders::bytes_transferred));
    return;
  }
  shutdown();
}

/// Handle completion of a asynchonous write operation.
void AsyncTcpConnection::handle_write(const error_t& error)  {
  if (!error)  {
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     boost::bind(&TcpConnection::handle_read, shared_from_this(),
					 asio::placeholders::error,
					 asio::placeholders::bytes_transferred));
  }
  else   {
    // No new asynchronous operations are started. This means that all shared_ptr
    // references to the connection object will disappear and the object will be
    // destroyed automatically after this handler returns. The connection class's
    // destructor closes the socket.
    shutdown();
  }
}
