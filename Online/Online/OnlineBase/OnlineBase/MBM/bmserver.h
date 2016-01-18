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
#ifndef _MBM_MBMSERVER_H 
#define _MBM_MBMSERVER_H

#include "MBM/bmdef.h"
#if __cplusplus >= 201103L
#define MBM_HAVE_STD_MUTEX 1
#include <mutex>
#endif

struct ServerBMID_t : public BufferMemory {
  int              _spare;
  int              num_threads;
  int              stop;
  int              allow_declare;
  RTL_ast_t        free_event;
  void*            free_event_param;
  RTL_ast_t        alloc_event;
  void*            alloc_event_param;
  lib_rtl_thread_t client_thread;
#ifndef MBM_HAVE_STD_MUTEX
  lib_rtl_lock_t   lockid;
#else
  std::mutex       lockid;
#endif
  MBMCommunication communication;
  MBMConnection    clients;
  struct Server {
    MBMConnection    connection;
    lib_rtl_thread_t dispatcher;
  } server[BM_MAX_THREAD];

  struct ConsumerREQ {
    REQ      requirement;
    char     name[BM_USER_NAME_LEN];     // user name match
    int      count;
  } cons_req[BM_MAX_REQS];

  ServerBMID_t();
  ~ServerBMID_t();
};

#endif // _MBM_MBMSERVER_H
