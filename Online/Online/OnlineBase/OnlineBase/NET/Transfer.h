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

// No header guards!

#include <string>

namespace TRANSFER_NS  {
  enum NetErrorCode {
    NET_ERROR          =  0,
    NET_SUCCESS        =  1,
    NET_TASKNOTFOUND   =  2,
    NET_TERRIBLE       =  4,
    NET_NODATA         =  6,
    NET_ILLEGAL_LENGTH =  8,
    NET_UNKNOWNMODE    = 10,
    NET_CONNCLOSED     = 12,  // Also message type
    NET_TASKDIED       = 14,  // Also message type
    NET_MSG_DATA       = 16,  // Also message type
    NET_MSG_HELLO      = 17,  // Also message type
    NET_MSG_TERMINATE  = 18,  // Also message type
    NET_MAGIC          = 0xDEADCAFE
  };
  enum NetConnectionType  {
    NET_CLIENT = 1,
    NET_SERVER = 2
  };
  struct NET;
  struct netheader_t;
  struct netentry_t;

  struct netheader_t {
    unsigned int    size;
    unsigned int    msg_type;
    unsigned int    facility;
    unsigned int    magic;
    unsigned int    hash;
    char            name[128];
    netheader_t() : size(0), msg_type(NET_MSG_DATA), facility(0), magic(NET_MAGIC), hash(0) { name[0]=0; }
    ~netheader_t() {}
  };
  typedef void (*net_handler_t)(const netheader_t& hdr, void* param, netentry_t* entry);
  NET*  net_init(const std::string& proc, int nthreads, NetConnectionType type=NET_SERVER);
  void  net_close(NET* net);
  void* net_lock(NET* net);
  void  net_unlock(NET* net, void* lock);
  void  net_cancel(NET* net);
  int   net_subscribe(NET* net, void* param, unsigned int fac, net_handler_t data, net_handler_t death);
  int   net_unsubscribe(NET* net, void* param, unsigned int fac);
  int   net_receive(NET* net, netentry_t* e, void* buff);
  int   net_send(NET* net, const void* buff, size_t size, const std::string& dest, unsigned int fac);
}
