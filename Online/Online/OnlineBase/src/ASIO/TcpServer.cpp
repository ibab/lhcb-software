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
#include <thread>
#include <memory>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace asio  {
  using namespace boost::asio;
}

using namespace BoostAsio;

/// Default constructor
TcpServerConfig::TcpServerConfig(TcpServer* srv) 
  : m_server(srv), handler(), recvSize(8192), synchronous(1)
{
}

/// Default destructor
TcpServerConfig::~TcpServerConfig()   {
}

/// Access Boost asio service instance
boost::asio::io_service& TcpServerConfig::io_service() const  {
  return m_server->io_service();
}

/// Create a new blank server connection
TcpConnection* TcpServerConfig::newConnection()   {
  if ( synchronous > 0 )
    return new SyncTcpConnection(io_service(), handler, recvSize);
  return new AsyncTcpConnection(io_service(), handler, recvSize);
}

/// Construct the server to listen on the specified TCP address and port
TcpServer::TcpServer(const std::string& address, const std::string& port, size_t num_threads)
  : m_numThreads(num_threads),m_service(), 
    m_signals(m_service), m_acceptor(m_service),
    m_newConnection(), config()
{
  config.reset(new TcpServerConfig(this));
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  config->handler.reset(new TcpRequestHandler());
  
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
TcpServer::~TcpServer()  {
}

/// Attach default signal handlers to the server
void TcpServer::handleSignals()   {
  m_signals.add(SIGINT);
  m_signals.add(SIGTERM);
#if defined(SIGQUIT)
  m_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
  m_signals.async_wait(boost::bind(&TcpServer::handleStop, this));
}

/// Attach specific request handler depending on service
void TcpServer::setRecvSize(size_t new_recv_size)   {
  config->recvSize = new_recv_size;
}

/// Start dispatch pumps
void TcpServer::start()   {
  // Create a pool of threads to run all of the io_services.
  startAccept();
  for (size_t i = 0; i < m_numThreads; ++i)   {
    std::shared_ptr<std::thread> thread(new std::thread([this]{ this->m_service.run(); }));
    m_threads.push_back(thread);
  }
}

/// Wait until all threads finished to exit server cleanly
void TcpServer::join()  {
  // Wait for all threads in the pool to exit.
  for (size_t i = 0; i < m_threads.size(); ++i)
    m_threads[i]->join();
}

/// Run the server's io_service loop.
void TcpServer::run()   {
  try  {
    start();
    join();
  }
  catch(const std::exception& e)  {
    ::lib_rtl_output(LIB_RTL_OS,"ASIO: Failed to start TcpServer message pump. [%s]",e.what());
  }
}

/// Initiate an asynchronous accept operation.
void TcpServer::startAccept()   {
  m_newConnection.reset(config->newConnection());
  m_acceptor.async_accept(m_newConnection->socket(),
			  boost::bind(&TcpServer::handleAccept,this,asio::placeholders::error));
}

/// Handle completion of an asynchronous accept operation.
void TcpServer::handleAccept(const error_t& e)   {
  if (!e)  {
    m_newConnection->start();
  }
  startAccept();
}

/// Handle a request to stop the server.
void TcpServer::handleStop()   {
  m_service.stop();
}

/// Start default service as "echo" service
extern "C" int boost_asio_tcp_echo_server(int argc, char** argv)  {
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
    TcpServer s(argv[1], argv[2], num_threads);
    s.config->handler = TcpRequestHandler::pointer_t(new TcpRequestHandler());
    s.handleSignals();

    // Run the server until stopped.
    s.run();
  }
  catch (const std::exception& e)  {
    ::printf("exception: %s\n",e.what());
  }
  return 0;
}
