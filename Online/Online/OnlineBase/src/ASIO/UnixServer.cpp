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
#include <thread>
#include <memory>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace asio  {
  using namespace boost::asio;
}

using namespace BoostAsio;

/// Default constructor
UnixServerConfig::UnixServerConfig(UnixServer* srv) 
  : m_server(srv), handler(), recvSize(8192), synchronous(1)
{
}

/// Default destructor
UnixServerConfig::~UnixServerConfig()   {
}

/// Access Boost asio service instance
boost::asio::io_service& UnixServerConfig::io_service() const  {
  return m_server->io_service();
}

/// Create a new blank server connection
UnixConnection* UnixServerConfig::newConnection()   {
  if ( synchronous > 0 )
    return new SyncUnixConnection(io_service(), handler, recvSize);
  return new AsyncUnixConnection(io_service(), handler, recvSize);
}

/// Construct the server to listen on the specified TCP address and port
UnixServer::UnixServer(const std::string& file, size_t num_threads)
  : m_numThreads(num_threads),m_service(), 
    m_signals(m_service), m_acceptor(m_service, asio::local::stream_protocol::endpoint(file)),
    m_newConnection(), config()
{
  config.reset(new UnixServerConfig(this));
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  config->handler.reset(new UnixRequestHandler());
}

/// Default destructor
UnixServer::~UnixServer()  {
}

/// Attach default signal handlers to the server
void UnixServer::handleSignals()   {
  m_signals.add(SIGINT);
  m_signals.add(SIGTERM);
#if defined(SIGQUIT)
  m_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
  m_signals.async_wait(boost::bind(&UnixServer::handleStop, this));
}

/// Attach specific request handler depending on service
void UnixServer::setRecvSize(size_t new_recv_size)   {
  config->recvSize = new_recv_size;
}

/// Start dispatch pumps
void UnixServer::start()   {
  // Create a pool of threads to run all of the io_services.
  startAccept();
  for (size_t i = 0; i < m_numThreads; ++i)   {
    std::shared_ptr<std::thread> thread(new std::thread([this]{ this->m_service.run(); }));
    m_threads.push_back(thread);
  }
}

/// Wait until all threads finished to exit server cleanly
void UnixServer::join()  {
  // Wait for all threads in the pool to exit.
  for (size_t i = 0; i < m_threads.size(); ++i)
    m_threads[i]->join();
}

/// Run the server's io_service loop.
void UnixServer::run()   {
  try  {
    start();
    join();
  }
  catch(const std::exception& e)  {
    ::lib_rtl_output(LIB_RTL_OS,"ASIO: Failed to start UnixServer message pump. [%s]",e.what());
  }
}

/// Initiate an asynchronous accept operation.
void UnixServer::startAccept()   {
  m_newConnection.reset(config->newConnection());
  m_acceptor.async_accept(m_newConnection->socket(),
			  boost::bind(&UnixServer::handleAccept,this,asio::placeholders::error));
}

/// Handle completion of an asynchronous accept operation.
void UnixServer::handleAccept(const error_t& e)   {
  if (!e)  {
    m_newConnection->start();
  }
  startAccept();
}

/// Handle a request to stop the server.
void UnixServer::handleStop()   {
  m_service.stop();
}

/// Start default service as "echo" service
extern "C" int boost_asio_unix_echo_server(int argc, char** argv)  {
  try  {
    // Check command line arguments.
    if (argc != 4)    {
      ::printf("Usage: boost_asio_unix_echo_server <address> <threads>\n"
	       "    receiver /dev/shm/test 1 .\n");
      return 1;
    }

    // Initialise the server.
    size_t num_threads = boost::lexical_cast<size_t>(argv[2]);
    UnixServer s(argv[1], num_threads);
    s.config->handler = UnixRequestHandler::pointer_t(new UnixRequestHandler());
    s.handleSignals();

    // Run the server until stopped.
    s.run();
  }
  catch (const std::exception& e)  {
    ::printf("exception: %s\n",e.what());
  }
  return 0;
}
