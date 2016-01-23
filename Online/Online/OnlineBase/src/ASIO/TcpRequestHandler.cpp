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
#include "ASIO/TcpRequestHandler.h"
#include "ASIO/TcpConnection.h"
#include "RTL/rtl.h"

namespace asio  {
  using namespace boost::asio;
}

using namespace BoostAsio;

/// Default constructor
TcpRequestHandler::TcpRequestHandler()   {
}

/// Default destructor
TcpRequestHandler::~TcpRequestHandler()   {
}

/// Handle a request and produce a reply.
void TcpRequestHandler::handle(TcpConnection& /* connection */, 
			       const Request& request,
			       Reply& reply)
{
  reply.push_back(asio::buffer(asio::detail::buffer_cast_helper(request),
			       asio::detail::buffer_size_helper(request)));
}

/// Handle connection finalization for cleanups
void TcpRequestHandler::handleClose(TcpConnection& connection)   {
  ::lib_rtl_output(LIB_RTL_INFO,"Closing connection:%d\n",connection.socket().native());
}
