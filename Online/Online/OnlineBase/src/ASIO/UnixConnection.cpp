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
#include "ASIO/UnixServer.h"
#include "RTL/rtl.h"

// System / Boost include files
#include <memory>
#include <boost/bind.hpp>

namespace asio  {
  using namespace boost::asio;
}

using namespace BoostAsio;

/// Construct a connection with the given io_service.
UnixConnection::UnixConnection(asio::io_service& io_service, 
			       UnixRequestHandler::pointer_t& handler)
  : m_strand(io_service), m_socket(io_service), m_handler(handler), m_param(0)
{
}

/// Default destructor
UnixConnection::~UnixConnection()   {
  if ( m_handler )  {
    m_handler->handleClose(*this);
  }
}

/// Connect to target destination
UnixConnection& UnixConnection::connect(const std::string& file, int)   {
  error_t ignored_ec;
  m_socket.connect(asio::local::stream_protocol::endpoint(file), ignored_ec);
  return *this;
}

/// Initiate graceful connection closure.
void UnixConnection::shutdown()   {
  if ( m_socket.is_open() )  {
    error_t ignored_ec;
    m_socket.shutdown(socket_t::shutdown_both, ignored_ec);
  }
}

/// Handle completion of a optional asynchonous unlocked read operation.
void UnixConnection::handle_additional_read(const error_t& /* error */, 
					    size_t /* bytes_transferred */)  {
}

/// Construct a connection with the given io_service.
SyncUnixConnection::SyncUnixConnection(boost::asio::io_service& io_service,
				       UnixRequestHandler::pointer_t& handler,
				       size_t recvSize)
  : UnixConnection(io_service, handler), m_recvSize(recvSize)
{
  m_recv_buffer.resize(recvSize);
}
    
/// Default destructor
SyncUnixConnection::~SyncUnixConnection()   {
}

/// Start the first asynchronous operation for the connection.
void SyncUnixConnection::start()  {
  m_socket.set_option(socket_t::linger(true,0));
  m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			   m_strand.wrap(boost::bind(&UnixConnection::handle_read,
						     shared_from_this(),
						     asio::placeholders::error,
						     asio::placeholders::bytes_transferred)));
}

/// Handle completion of a asynchonous unlocked read/write operation.
void SyncUnixConnection::handle_read(const error_t& error, size_t bytes_transferred)  {
  if ( !error )  {
    if ( bytes_transferred > 0 )    {
      UnixRequestHandler::Reply   reply;
      UnixRequestHandler::Request request(m_recv_buffer.data(), bytes_transferred);
      m_handler->handle(*this,request, reply);
      if ( !reply.empty() )  {
	asio::async_write(m_socket, reply,
			  m_strand.wrap(boost::bind(&UnixConnection::handle_write,
						    shared_from_this(),
						    asio::placeholders::error)));
	return;
      }
    }
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     m_strand.wrap(boost::bind(&UnixConnection::handle_read,
						       shared_from_this(),
						       asio::placeholders::error,
						       asio::placeholders::bytes_transferred)));
    return;
  }
  shutdown();
}

/// Handle completion of a asynchonous write operation.
void SyncUnixConnection::handle_write(const error_t& error)  {
  if (!error)  {
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     m_strand.wrap(boost::bind(&UnixConnection::handle_read, shared_from_this(),
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
AsyncUnixConnection::AsyncUnixConnection(boost::asio::io_service& io_service,
					 UnixRequestHandler::pointer_t& handler,
					 size_t recvSize)
  : UnixConnection(io_service, handler), m_recvSize(recvSize)
{
  m_recv_buffer.resize(recvSize);
}
    
/// Default destructor
AsyncUnixConnection::~AsyncUnixConnection()   {
}

/// Start the first asynchronous operation for the connection.
void AsyncUnixConnection::start()  {
  m_socket.set_option(socket_t::linger(true,0));
  m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			   boost::bind(&UnixConnection::handle_read,
				       shared_from_this(),
				       asio::placeholders::error,
				       asio::placeholders::bytes_transferred));
}

/// Handle completion of a asynchonous unlocked read/write operation.
void AsyncUnixConnection::handle_read(const error_t& error, size_t bytes_transferred)  {
  if ( !error )  {
    if ( bytes_transferred > 0 )    {
      UnixRequestHandler::Reply   reply;
      UnixRequestHandler::Request request(m_recv_buffer.data(), bytes_transferred);
      m_handler->handle(*this, request, reply);
      if ( !reply.empty() )  {
	asio::async_write(m_socket, reply,
			  boost::bind(&UnixConnection::handle_write,
				      shared_from_this(),
				      asio::placeholders::error));
	return;
      }
    }
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     boost::bind(&UnixConnection::handle_read,
					 shared_from_this(),
					 asio::placeholders::error,
					 asio::placeholders::bytes_transferred));
    return;
  }
  shutdown();
}

/// Handle completion of a asynchonous write operation.
void AsyncUnixConnection::handle_write(const error_t& error)  {
  if (!error)  {
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     boost::bind(&UnixConnection::handle_read, shared_from_this(),
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
