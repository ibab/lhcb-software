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

#define HAVE_ASIO_IPC
#define TRANSFER_NS BoostAsioIpc
namespace BoostAsio  {
  typedef class UnixServer          TransferServer;
  typedef class UnixConnection      TransferConnection;
  typedef class AsyncUnixConnection AsyncTransferConnection;
  typedef class UnixRequestHandler  TransferRequestHandler;
  typedef class UnixServerConfig    TransferServerConfig;
}
#include <boost/asio.hpp>
namespace TRANSFER_NS  {
  typedef boost::system::error_code error_t;
}
using namespace BoostAsio;

#include "Transfer.cpp"

