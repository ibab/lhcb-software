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
#ifndef ONLINEBASE_ASIO_ASIOSOCKET_H
#define ONLINEBASE_ASIO_ASIOSOCKET_H

// System / Boost include files
#include "boost/asio.hpp"

class TanMessage;

namespace BoostAsio {

  class Socket  {
  protected:
    boost::asio::io_service      m_io;
    boost::asio::ip::tcp::socket m_socket;
    int                          m_error, m_flags;

  public:
    /// Standard constructor
    Socket(int=0);
    /// Default destructor
    virtual ~Socket();

    int error() const  { return m_error; }
    bool isCancelled() const;
    int connect(const char* addr, const char* port);
    int connect(const TanMessage& msg);
    int close();
    int send(const void* buffer, size_t len);
    int recv(void* buffer, size_t len, int tmo);
    int setsockopt(int what, void* value, size_t len);
  };
}       // End namespace ASIO     

#endif  /* ONLINEBASE_ASIO_ASIOSOCKET_H  */
