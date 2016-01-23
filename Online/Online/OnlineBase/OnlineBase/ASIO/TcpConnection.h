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
#ifndef ONLINEBASE_ASIO_ASIOTCPCONNECTION_H
#define ONLINEBASE_ASIO_ASIOTCPCONNECTION_H

// Framework include files
#include "ASIO/TcpRequestHandler.h"

// System / Boost include files
#include <memory>

/// BoostAsio namespace declaration for Online stuff using boost::asio
namespace BoostAsio  {

  // Forward declarations and type definitions
  typedef boost::system::error_code error_t;
  class TcpConnection;
  class TcpRequestHandler;

  /// Represents a single connection from a client.
  /**
   *  \author   M.Frank
   *  \version  1.0
   *  \defgroup LHCB_ONLINE
   */
  class TcpConnection
    : public boost::enable_shared_from_this<TcpConnection>,
    private boost::noncopyable      {
  public:
    typedef boost::asio::ip::tcp::socket     socket_t;
    typedef boost::asio::io_service::strand  strand_t;
    typedef boost::shared_ptr<TcpConnection>    pointer_t;
    typedef std::vector<unsigned char>       buffer_t;

    /// Construct a connection with the given io_service.
    explicit TcpConnection(boost::asio::io_service& io_service,
			   TcpRequestHandler::pointer_t& handler);
    
    /// Default destructor
    virtual ~TcpConnection();

    /// Connect to target destination
    virtual TcpConnection& connect(const char* dest, unsigned short port);
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
    TcpRequestHandler::pointer_t m_handler;
    /// User data parameter
    void*           m_param;
  };

  class SyncTcpConnection : public TcpConnection  {
  protected:
    /// Default receive packet length to interprete request
    size_t  m_recvSize;
  public:
    /// Construct a connection with the given io_service.
    explicit SyncTcpConnection(boost::asio::io_service& io_service,
			       TcpRequestHandler::pointer_t& handler,
			       size_t recvSize);
    
    /// Default destructor
    virtual ~SyncTcpConnection();
    /// Start the first asynchronous operation for the connection.
    virtual void start();
    /// Handle completion of a asynchonous unlocked read/write operation.
    virtual void handle_read(const error_t& error, size_t bytes_transferred);
    /// Handle completion of a asynchonous write operation.
    virtual void handle_write(const error_t& error);
  };

  class AsyncTcpConnection : public TcpConnection  {
  protected:
    /// Default receive packet length to interprete request
    size_t  m_recvSize;
  public:
    /// Construct a connection with the given io_service.
    explicit AsyncTcpConnection(boost::asio::io_service& io_service,
				TcpRequestHandler::pointer_t& handler,
				size_t recvSize);    
    /// Default destructor
    virtual ~AsyncTcpConnection();
    /// Start the first asynchronous operation for the connection.
    virtual void start();
    /// Handle completion of a asynchonous unlocked read/write operation.
    virtual void handle_read(const error_t& error, size_t bytes_transferred);
    /// Handle completion of a asynchonous write operation.
    virtual void handle_write(const error_t& error);
  };

}
#endif /*  ONLINEBASE_ASIO_ASIOTCPCONNECTION_H */
