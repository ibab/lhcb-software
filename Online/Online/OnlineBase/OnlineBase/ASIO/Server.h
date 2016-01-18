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
#ifndef ONLINEBASE_ASIO_ASIOSERVER_H
#define ONLINEBASE_ASIO_ASIOSERVER_H

// System / Boost include files
#include <thread>
#include <string>
#include <vector>
#include <memory>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace BoostAsio  {

  // Forward declarations and type definitions
  typedef boost::system::error_code error_t;
  class RequestHandler;
  class Connection;
  class ServerConfig;
  class Server;

  /// The common handler for all incoming requests.
  /**
   *  \author   M.Frank
   *  \version  1.0
   *  \defgroup LHCB_ONLINE
   */
  class RequestHandler
    : public boost::enable_shared_from_this<RequestHandler>,
      private boost::noncopyable
  {
  public:
    typedef boost::asio::const_buffer              Request;
    typedef std::vector<boost::asio::const_buffer> Reply;
    typedef boost::shared_ptr<RequestHandler>      pointer_t;

    /// Default constructor
    explicit RequestHandler();
    /// Default destructor
    virtual ~RequestHandler();
    /// Handle a request and produce a reply.
    virtual void handle(Connection& connection,
			const Request& request,
			Reply& reply);
    /// Handle connection finalization for cleanups
    virtual void handleClose(Connection& connection);
  };

  /// Represents a single connection from a client.
  /**
   *  \author   M.Frank
   *  \version  1.0
   *  \defgroup LHCB_ONLINE
   */
  class Connection
    : public boost::enable_shared_from_this<Connection>,
      private boost::noncopyable
  {
  public:
    typedef boost::asio::ip::tcp::socket     socket_t;
    typedef boost::asio::io_service::strand  strand_t;
    typedef boost::shared_ptr<Connection>    pointer_t;
    typedef std::vector<unsigned char>       buffer_t;

    /// Construct a connection with the given io_service.
    explicit Connection(boost::asio::io_service& io_service,
			RequestHandler::pointer_t& handler);
    
    /// Default destructor
    virtual ~Connection();

    /// Connect to target destination
    virtual Connection& connect(const char* dest, unsigned short port);
    /// Initiate graceful connection closure.
    virtual void shutdown();

    /// Start the first asynchronous operation for the connection.
    virtual void start() = 0;
    /// Handle completion of a asynchonous unlocked read/write operation.
    virtual void handle_read(const error_t& error, size_t bytes_transferred) = 0;
    /// Handle completion of a optional asynchonous unlocked read operation.
    virtual void handle_additional_read(const error_t& error, size_t bytes_transferred); 
    /// Handle completion of a asynchonous write operation.
    virtual void handle_write(const error_t& error) = 0;

    /// Get the socket associated with the connection.
    socket_t& socket()                   {  return m_socket;          }
    /// Accessor to the reply buffer
    buffer_t& request()                  {  return m_recv_buffer;     }    
    /// Accessor to the reply buffer
    buffer_t& reply()                    {  return m_reply_buffer;    }    
    /// Set user parameter
    void userParam(void* param)          {  m_param = param;          }
    /// Access user parameter
    void* userParam() const              {  return m_param;           }

  protected:
    /// Strand to ensure the connection's handlers are not called concurrently.
    strand_t        m_strand;
    /// Socket for the connection.
    socket_t        m_socket;
    /// Buffer for incoming data.
    buffer_t        m_recv_buffer;
    /// Buffer for incoming data.
    buffer_t        m_reply_buffer;
    /// The handler used to process the incoming request.
    RequestHandler::pointer_t m_handler;
    /// User data parameter
    void*           m_param;
  };

  class SyncConnection : public Connection  {
  protected:
    /// Default receive packet length to interprete request
    size_t  m_recvSize;
  public:
    /// Construct a connection with the given io_service.
    explicit SyncConnection(boost::asio::io_service& io_service,
			    RequestHandler::pointer_t& handler,
			    size_t recvSize);
    
    /// Default destructor
    virtual ~SyncConnection();
    /// Start the first asynchronous operation for the connection.
    virtual void start();
    /// Handle completion of a asynchonous unlocked read/write operation.
    virtual void handle_read(const error_t& error, size_t bytes_transferred);
    /// Handle completion of a asynchonous write operation.
    virtual void handle_write(const error_t& error);
  };

  class AsyncConnection : public Connection  {
  protected:
    /// Default receive packet length to interprete request
    size_t  m_recvSize;
  public:
    /// Construct a connection with the given io_service.
    explicit AsyncConnection(boost::asio::io_service& io_service,
			     RequestHandler::pointer_t& handler,
			     size_t recvSize);    
    /// Default destructor
    virtual ~AsyncConnection();
    /// Start the first asynchronous operation for the connection.
    virtual void start();
    /// Handle completion of a asynchonous unlocked read/write operation.
    virtual void handle_read(const error_t& error, size_t bytes_transferred);
    /// Handle completion of a asynchonous write operation.
    virtual void handle_write(const error_t& error);
  };

  /// Server configuration class
  class ServerConfig  {
  protected:
    Server* m_server;
  public:
    /// The handler for all incoming requests.
    RequestHandler::pointer_t handler;
    /// Default receive chunk size
    size_t  recvSize;
    /// Flag to protect actions using strends
    int     synchronous;
    
  public:
    /// Default constructor
    ServerConfig(Server* srv);
    /// Default destructor
    virtual ~ServerConfig();
    /// Access Boost asio service instance
    boost::asio::io_service& io_service() const;
    /// Attach specific request handler depending on service
    void setHandler(RequestHandler* new_handler)   {
      handler = RequestHandler::pointer_t(new_handler);
    }
    /// Create a new blank server connection
    virtual Connection* newConnection();
  };

  /// The top-level class of the HTTP server.
  /**
   *  \author   M.Frank
   *  \version  1.0
   *  \defgroup LHCB_ONLINE
   */
  class Server : private boost::noncopyable  {
    friend class ServerConfig;

  public:
    typedef boost::asio::signal_set                    signals_t;
    typedef boost::asio::io_service                    service_t;
    typedef boost::asio::ip::tcp::acceptor             acceptor_t;
    typedef std::vector<std::shared_ptr<std::thread> > thread_pool_t;
    typedef std::shared_ptr<ServerConfig>              config_t;
 
    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit Server(const std::string& address, 
		    const std::string& port,
                    size_t thread_pool_size);

    /// Default destructor
    virtual ~Server();

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

  private:
    /// Initiate an asynchronous accept operation.
    void startAccept();
    /// Handle completion of an asynchronous accept operation.
    void handleAccept(const error_t& error);
    /// Handle a request to stop the server.
    void handleStop();

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
    Connection::pointer_t      m_newConnection;
  public:
    /// Server configurator
    config_t                   config;
  };
}
#endif /*  ONLINEBASE_ASIO_ASIOSERVER_H */
