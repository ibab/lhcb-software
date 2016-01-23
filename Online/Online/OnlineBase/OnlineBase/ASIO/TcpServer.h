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
#ifndef ONLINEBASE_ASIO_ASIOTCPSERVER_H
#define ONLINEBASE_ASIO_ASIOTCPSERVER_H

// System / Boost include files
#include "ASIO/TcpConnection.h"
#include <thread>
#include <string>

/// BoostAsio namespace declaration for Online stuff using boost::asio
namespace BoostAsio  {

  // Forward declarations and type definitions
  class TcpConnection;
  class TcpServerConfig;
  class TcpServer;

  /// Server configuration class
  class TcpServerConfig  {
  protected:
    TcpServer* m_server;
  public:
    /// The handler for all incoming requests.
    TcpRequestHandler::pointer_t handler;
    /// Default receive chunk size
    size_t  recvSize;
    /// Flag to protect actions using strends
    int     synchronous;
    
  public:
    /// Default constructor
    TcpServerConfig(TcpServer* srv);
    /// Default destructor
    virtual ~TcpServerConfig();
    /// Access Boost asio service instance
    boost::asio::io_service& io_service() const;
    /// Attach specific request handler depending on service
    void setHandler(TcpRequestHandler* new_handler)   {
      handler = TcpRequestHandler::pointer_t(new_handler);
    }
    /// Create a new blank server connection
    virtual TcpConnection* newConnection();
  };

  /// The top-level class of the HTTP server.
  /**
   *  \author   M.Frank
   *  \version  1.0
   *  \defgroup LHCB_ONLINE
   */
  class TcpServer : private boost::noncopyable  {
    friend class TcpServerConfig;

  public:
    typedef boost::asio::signal_set                    signals_t;
    typedef boost::asio::io_service                    service_t;
    typedef boost::asio::ip::tcp::acceptor             acceptor_t;
    typedef std::vector<std::shared_ptr<std::thread> > thread_pool_t;
    typedef std::shared_ptr<TcpServerConfig>           config_t;
 
    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit TcpServer(const std::string& address, 
		       const std::string& port,
		       size_t thread_pool_size);

    /// Default destructor
    virtual ~TcpServer();

    thread_pool_t&  thread_pool()       {   return m_threads;  }
    /// Access Boost asio service instance
    service_t&      io_service()        {   return m_service;  }

    /// Attach specific request handler depending on service
    void setRecvSize(size_t new_recv_size);

    /// Attach default signal handlers to the server
    void handleSignals();

    /// Start dispatch pumps
    void start();

    /// Wait until all threads finished to exit server cleanly
    void join();

    /// Run the server's io_service loop.
    void run();

    /// Handle a request to stop the server.
    void handleStop();

  private:
    /// Initiate an asynchronous accept operation.
    void startAccept();

    /// Handle completion of an asynchronous accept operation.
    void handleAccept(const error_t& error);

  private:
    /// The number of threads that will call io_service::run().
    size_t m_numThreads;
    /// The io_service used to perform asynchronous operations.
    service_t m_service;
    /// The signal_set is used to register for process termination notifications.
    signals_t m_signals;
    /// Acceptor used to listen for incoming connections.
    acceptor_t m_acceptor;
    /// Thread pool
    thread_pool_t m_threads;
    /// The next connection to be accepted.
    TcpConnection::pointer_t      m_newConnection;
  public:
    /// Server configurator
    config_t                   config;
  };
}
#endif /*  ONLINEBASE_ASIO_ASIOSERVER_H */
