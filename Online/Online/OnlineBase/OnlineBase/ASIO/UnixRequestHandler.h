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
#ifndef ONLINEBASE_ASIO_ASIOUNIXREQUESTHANDLER_H
#define ONLINEBASE_ASIO_ASIOUNIXREQUESTHANDLER_H

// System / Boost include files
#include <vector>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

/// BoostAsio namespace declaration for Online stuff using boost::asio
namespace BoostAsio  {

  // Forward declarations and type definitions
  typedef boost::system::error_code error_t;
  class UnixConnection;
  class UnixRequestHandler;

  /// The common handler for all incoming requests.
  /**
   *  \author   M.Frank
   *  \version  1.0
   *  \defgroup LHCB_ONLINE
   */
  class UnixRequestHandler
    : public boost::enable_shared_from_this<UnixRequestHandler>,
      private boost::noncopyable
  {
  public:
    typedef boost::asio::const_buffer              Request;
    typedef std::vector<boost::asio::const_buffer> Reply;
    typedef boost::shared_ptr<UnixRequestHandler>   pointer_t;

    /// Default constructor
    explicit UnixRequestHandler();
    /// Default destructor
    virtual ~UnixRequestHandler();
    /// Handle a request and produce a reply.
    virtual void handle(UnixConnection& connection,
			const Request& request,
			Reply& reply);
    /// Handle connection finalization for cleanups
    virtual void handleClose(UnixConnection& connection);
  };
}
#endif /*  ONLINEBASE_ASIO_ASIOUNIXREQUESTHANDLER_H */
