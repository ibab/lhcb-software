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
#include "ASIO/UnixRequestHandler.h"
#include "ASIO/UnixConnection.h"
#include "RTL/rtl.h"

namespace asio  {
  using namespace boost::asio;
}

using namespace BoostAsio;

/// Default constructor
UnixRequestHandler::UnixRequestHandler()   {
}

/// Default destructor
UnixRequestHandler::~UnixRequestHandler()   {
}

/// Handle a request and produce a reply.
void UnixRequestHandler::handle(UnixConnection& /* connection */, 
			       const Request& request,
			       Reply& reply)
{
  reply.push_back(asio::buffer(asio::detail::buffer_cast_helper(request),
			       asio::detail::buffer_size_helper(request)));
}

/// Handle connection finalization for cleanups
void UnixRequestHandler::handleClose(UnixConnection& connection)   {
  ::lib_rtl_output(LIB_RTL_INFO,"Closing connection:%d\n",connection.socket().native());
}
