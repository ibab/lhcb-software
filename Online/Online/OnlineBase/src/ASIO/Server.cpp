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
#include "ASIO/Server.h"
#include "RTL/rtl.h"

// System / Boost include files
#include <thread>
#include <memory>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace asio  {
  using namespace boost::asio;
}

using namespace BoostAsio;

/// Default constructor
RequestHandler::RequestHandler()   {
}

/// Default destructor
RequestHandler::~RequestHandler()   {
}

/// Handle a request and produce a reply.
void RequestHandler::handle(Connection& /* connection */, 
			    const Request& request,
			    Reply& reply)
{
  reply.push_back(asio::buffer(asio::detail::buffer_cast_helper(request),
			       asio::detail::buffer_size_helper(request)));
}

/// Handle connection finalization for cleanups
void RequestHandler::handleClose(Connection& connection)   {
  ::printf("Closing connection:%d\n",connection.socket().native());
}

/// Construct a connection with the given io_service.
Connection::Connection(asio::io_service& io_service, 
		       RequestHandler::pointer_t& handler)
  : m_strand(io_service), m_socket(io_service), m_handler(handler), m_param(0)
{
}

/// Default destructor
Connection::~Connection()   {
  if ( m_handler )  {
    m_handler->handleClose(*this);
  }
}

/// Connect to target destination
Connection& Connection::connect(const char* dest, unsigned short port)   {
  char port_str[64];
  ::snprintf(port_str,sizeof(port_str),"%d",int(htons(port)));
  asio::ip::tcp::resolver::query query(dest, port_str);
  asio::ip::tcp::resolver resolver(m_socket.get_io_service());
  asio::connect(m_socket, resolver.resolve(query));
  //m_socket.connect(resolver.resolve(query));
  m_socket.set_option(asio::socket_base::reuse_address(true));
  m_socket.set_option(asio::socket_base::linger(true,0));
  return *this;
}

/// Initiate graceful connection closure.
void Connection::shutdown()   {
  if ( m_socket.is_open() )  {
    error_t ignored_ec;
    m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }
}

/// Handle completion of a optional asynchonous unlocked read operation.
void Connection::handle_additional_read(const error_t& /* error */, 
					size_t /* bytes_transferred */)  {
}

/// Construct a connection with the given io_service.
SyncConnection::SyncConnection(boost::asio::io_service& io_service,
			       RequestHandler::pointer_t& handler,
			       size_t recvSize)
  : Connection(io_service, handler), m_recvSize(recvSize)
{
  m_recv_buffer.resize(recvSize);
}
    
/// Default destructor
SyncConnection::~SyncConnection()   {
}

/// Start the first asynchronous operation for the connection.
void SyncConnection::start()  {
  m_socket.set_option(asio::socket_base::linger(true,0));
  m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			   m_strand.wrap(boost::bind(&Connection::handle_read,
						     shared_from_this(),
						     asio::placeholders::error,
						     asio::placeholders::bytes_transferred)));
}

/// Handle completion of a asynchonous unlocked read/write operation.
void SyncConnection::handle_read(const error_t& error, size_t bytes_transferred)  {
  if ( !error )  {
    if ( bytes_transferred > 0 )    {
      RequestHandler::Reply   reply;
      RequestHandler::Request request(m_recv_buffer.data(), bytes_transferred);
      m_handler->handle(*this,request, reply);
      if ( !reply.empty() )  {
	asio::async_write(m_socket, reply,
			  m_strand.wrap(boost::bind(&Connection::handle_write,
						    shared_from_this(),
						    asio::placeholders::error)));
	return;
      }
    }
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     m_strand.wrap(boost::bind(&Connection::handle_read,
						       shared_from_this(),
						       asio::placeholders::error,
						       asio::placeholders::bytes_transferred)));
    return;
  }
  shutdown();
}

/// Handle completion of a asynchonous write operation.
void SyncConnection::handle_write(const error_t& error)  {
  if (!error)  {
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     m_strand.wrap(boost::bind(&Connection::handle_read, shared_from_this(),
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
    m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
  }
}

/// Construct a connection with the given io_service.
AsyncConnection::AsyncConnection(boost::asio::io_service& io_service,
				 RequestHandler::pointer_t& handler,
				 size_t recvSize)
  : Connection(io_service, handler), m_recvSize(recvSize)
{
  m_recv_buffer.resize(recvSize);
}
    
/// Default destructor
AsyncConnection::~AsyncConnection()   {
}

/// Start the first asynchronous operation for the connection.
void AsyncConnection::start()  {
  m_socket.set_option(asio::socket_base::linger(true,0));
  m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			   boost::bind(&Connection::handle_read,
				       shared_from_this(),
				       asio::placeholders::error,
				       asio::placeholders::bytes_transferred));
}

/// Handle completion of a asynchonous unlocked read/write operation.
void AsyncConnection::handle_read(const error_t& error, size_t bytes_transferred)  {
  if ( !error )  {
    if ( bytes_transferred > 0 )    {
      RequestHandler::Reply   reply;
      RequestHandler::Request request(m_recv_buffer.data(), bytes_transferred);
      m_handler->handle(*this, request, reply);
      if ( !reply.empty() )  {
	asio::async_write(m_socket, reply,
			  boost::bind(&Connection::handle_write,
				      shared_from_this(),
				      asio::placeholders::error));
	return;
      }
    }
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     boost::bind(&Connection::handle_read,
					 shared_from_this(),
					 asio::placeholders::error,
					 asio::placeholders::bytes_transferred));
    return;
  }
  shutdown();
}

/// Handle completion of a asynchonous write operation.
void AsyncConnection::handle_write(const error_t& error)  {
  if (!error)  {
    m_socket.async_read_some(asio::buffer(m_recv_buffer.data(),m_recvSize),
			     boost::bind(&Connection::handle_read, shared_from_this(),
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

/// Default constructor
ServerConfig::ServerConfig(Server* srv) 
  : m_server(srv), handler(), recvSize(8192), synchronous(1)
{
}

/// Default destructor
ServerConfig::~ServerConfig()   {
}

/// Access Boost asio service instance
boost::asio::io_service& ServerConfig::io_service() const  {
  return m_server->io_service();
}

/// Create a new blank server connection
Connection* ServerConfig::newConnection()   {
  if ( synchronous > 0 )
    return new SyncConnection(io_service(), handler, recvSize);
  return new AsyncConnection(io_service(), handler, recvSize);
}

/// Construct the server to listen on the specified TCP address and port
Server::Server(const std::string& address, const std::string& port, size_t num_threads)
  : m_numThreads(num_threads),m_service(), 
    m_signals(m_service), m_acceptor(m_service),
    m_newConnection(), config()
{
  config.reset(new ServerConfig(this));
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  config->handler.reset(new RequestHandler());
  
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  asio::ip::tcp::resolver resolver(m_service);
  asio::ip::tcp::resolver::query query(address, port);
  asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  m_acceptor.open(endpoint.protocol());
  m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  m_acceptor.set_option(asio::socket_base::linger(true,0));
  m_acceptor.bind(endpoint);
  m_acceptor.listen();
}

/// Default destructor
Server::~Server()  {
}

/// Attach default signal handlers to the server
void Server::handleSignals()   {
  m_signals.add(SIGINT);
  m_signals.add(SIGTERM);
#if defined(SIGQUIT)
  m_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
  m_signals.async_wait(boost::bind(&Server::handleStop, this));
}

/// Attach specific request handler depending on service
void Server::setRecvSize(size_t new_recv_size)   {
  config->recvSize = new_recv_size;
}

/// Start dispatch pumps
void Server::start()   {
  // Create a pool of threads to run all of the io_services.
  startAccept();
  for (size_t i = 0; i < m_numThreads; ++i)   {
    std::shared_ptr<std::thread> thread(new std::thread([this]{ this->m_service.run(); }));
    m_threads.push_back(thread);
  }
}

/// Wait until all threads finished to exit server cleanly
void Server::join()  {
  // Wait for all threads in the pool to exit.
  for (size_t i = 0; i < m_threads.size(); ++i)
    m_threads[i]->join();
}

/// Run the server's io_service loop.
void Server::run()   {
  try  {
    start();
    join();
  }
  catch(const std::exception& e)  {
    ::lib_rtl_output(LIB_RTL_OS,"ASIO: Failed to start Server message pump. [%s]",e.what());
  }
}

/// Initiate an asynchronous accept operation.
void Server::startAccept()   {
  m_newConnection.reset(config->newConnection());
  m_acceptor.async_accept(m_newConnection->socket(),
			  boost::bind(&Server::handleAccept,this,asio::placeholders::error));
}

/// Handle completion of an asynchronous accept operation.
void Server::handleAccept(const error_t& e)   {
  if (!e)  {
    m_newConnection->start();
  }
  startAccept();
}

/// Handle a request to stop the server.
void Server::handleStop()   {
  m_service.stop();
}

/// Start default service as "echo" service
extern "C" int boost_asio_echo_server(int argc, char** argv)  {
  try  {
    // Check command line arguments.
    if (argc != 4)    {
      ::printf("Usage: http_server <address> <port> <threads>\n"
	       "  For IPv4, try:\n"
	       "    receiver 0.0.0.0 80 1 .\n" // INADDR_ANY, port 80 (http), 1 thread
	       "  For IPv6, try:\n"
	       "    receiver 0::0 80 1 .\n");
      return 1;
    }

    // Initialise the server.
    size_t num_threads = boost::lexical_cast<size_t>(argv[3]);
    Server s(argv[1], argv[2], num_threads);
    s.config->handler = RequestHandler::pointer_t(new RequestHandler());
    s.handleSignals();

    // Run the server until stopped.
    s.run();
  }
  catch (const std::exception& e)  {
    ::printf("exception: %s\n",e.what());
  }
  return 0;
}
