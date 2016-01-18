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
#include "AMS/Transfer.h"
#include "AMS/amsdef.h"
#include "RTL/rtl.h"
#include <cstdio>
#include <cstdlib>
#include <mutex>

namespace AMS  {
  struct netentry_t : public netheader_t {};
  struct  NET  {
    unsigned int             m_refCount;
    int         m_count;
    NetConnectionType        m_type;
    struct {
      std::string   name;
    } m_me;
    std::mutex  m_lock;
    std::string m_name;

    explicit NET(const std::string& proc) : m_name(proc)  {
    }
    ~NET()  {
      ::amsc_close();
    }
    NetConnectionType type() const { return m_type; }
    void cancel()  {
    }
    NetErrorCode init(NetConnectionType /* type */, int /* nthreads */)   {
      int status = ::amsc_init(m_name.c_str());
      if ( status == AMS_SUCCESS ) status = ::amsuc_init();
      return status==AMS_SUCCESS ? NET_SUCCESS : NET_ERROR;
    }
    NetErrorCode receive(netentry_t* entry, void* data)   {
      char from[128];
      size_t len = 0;
      netentry_t e;
      int status = ::amsc_read_message(data, &len, entry->name, &e.facility, from);
      return status==AMS_SUCCESS ? NET_SUCCESS : NET_ERROR;
    }
    NetErrorCode send(const void* buff, size_t size, const std::string& dest, int fac)  {
      int status = ::amsc_send_message(buff, size, dest.c_str(), fac, m_me.name.c_str());
      return status==AMS_SUCCESS ? NET_SUCCESS : NET_ERROR;
    }
    NetErrorCode subscribe(void* param, unsigned int fac, net_handler_t data, net_handler_t death)  {
      int status = ::amsuc_subscribe(fac, (amsuc_callback_t)data, (amsuc_callback_t)death, param);
      return status==AMS_SUCCESS ? NET_SUCCESS : NET_ERROR;
    }
    NetErrorCode unsubscribe(void* , unsigned int fac)   {
      int status = ::amsuc_remove(fac);
      return status==AMS_SUCCESS ? NET_SUCCESS : NET_ERROR;
    }
    unsigned int release()   {
      return 0;
    }
    static NET*  instance(const std::string& proc, int nthreads, NetConnectionType type)   {
      static NET* s_ams = 0;
      if ( 0 == s_ams )  {
	s_ams = new NET(proc);
	s_ams->init(type, nthreads);
      }
      return s_ams;
    }
  };
}

//----------------------------------------------------------------------------------
AMS::NET* AMS::net_init(const std::string& proc, int nthreads, NetConnectionType type)
{ return NET::instance(proc, nthreads, type);                     }
void AMS::net_close(NET* net)
{ if ( net ) net->release();                                      }
int AMS::net_receive(NET* net, netentry_t* e, void* buff) 
{ return net ? net->receive(e,buff) : NET_ERROR;                  }
int AMS::net_send(NET* net, const void* buff, size_t size, const std::string& dest, unsigned int fac)
{ return net ? net->send(buff,size,dest,fac) : NET_ERROR;         }
int AMS::net_subscribe(NET* net, void* param, unsigned int fac, net_handler_t data, net_handler_t death)
{ return net ? net->subscribe(param,fac,data,death) : NET_ERROR;  }
int AMS::net_unsubscribe(NET* net, void* param, unsigned int fac)
{ return net ? net->unsubscribe(param,fac) : NET_SUCCESS;         }
void* AMS::net_lock(NET* net)
{ if(net) net->m_lock.lock();  return net ? &net->m_lock : 0;     }
void AMS::net_unlock(NET* net, void* lock)
{ if(net && lock == &net->m_lock ) net->m_lock.unlock();          }
void AMS::net_cancel(NET* net)
{ if ( net ) net->cancel();                                       }


using namespace AMS;
#define TRANSFERTEST_SEND test_socket_ams_send
#define TRANSFERTEST_RECV test_socket_ams_recv
#include "NET/TransferTest.h"
