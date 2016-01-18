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

// Framework include files
#include "GaudiOnline/NetworkDataReceiver.h"
#include "GaudiKernel/SvcFactory.h"
#include "WT/wt_facilities.h"

// C/C++ Include files
#include <cstring>

int gauditask_task_lock();
int gauditask_task_trylock();
int gauditask_task_unlock();

/// Online namespace declaration
namespace Online  {

  using namespace TRANSFER_NS;

  /// Implementation of a generic network DataReceiver component
  /**
   *  Standard component template to implment DataReceiver component objects.
   *  The template is meant to be used by various DataTransfer
   *  implementations such as
   *
   *  - NET/DataTransfer
   *
   *  - ASIO/Transfer
   *
   *  - ZMQTransfer/Transfer
   *
   *  All these implementations have the same interface, which
   *  allows to transfer data between nodes.
   *
    *  @author Markus Frank
    */
  class TransferDataReceiver : public LHCb::NetworkDataReceiver  {

    typedef TransferDataReceiver self_t;

  protected:
    bool     m_finish;
    /// Pointer to netplug device
    NET*     m_netPlug;
    /// Property: Number of thread instances used by the network library
    int      m_nThreads;

    static void handle_death(const netheader_t& hdr, void* par, netentry_t*)  {
      int sc = gauditask_task_trylock();
      if ( sc == 1 ) {
        self_t* p = reinterpret_cast<self_t*>(par);
	p->taskDead(hdr.name);
        gauditask_task_unlock();
      }
    }
    static void handle_request(const netheader_t& hdr, void* par, netentry_t* entry)  {
      int sc = gauditask_task_trylock();
      if ( sc == 1 ) {
        self_t* p = reinterpret_cast<self_t*>(par);
	sc = net_receive(p->m_netPlug, entry, 0);
	if ( sc == NET_SUCCESS )  {
	  std::string source(hdr.name);
	  self_t* p = reinterpret_cast<self_t*>(par);
	  p->handleSourceRequest(p->receivers().size(),source,source);
	}
        gauditask_task_unlock();
        return;
      }
      // Things go awfully wrong....e.g. finalize was called during data taking.
      ::fprintf(stdout,"Loosing event request....was finalize called ?\n");
      ::fflush(stdout);
    }
    static void handle_event(const netheader_t& hdr, void* param, netentry_t* entry)  {
      int sc = gauditask_task_trylock();
      if ( sc == 1 ) {
	std::string source(hdr.name);
        self_t* p = reinterpret_cast<self_t*>(param);
        void* buffer = ::operator new(hdr.size);
        sc = net_receive(p->m_netPlug, entry, buffer);
        if ( sc == NET_SUCCESS )  {
	  RecvEntry* ent = p->receiver(source);
	  if ( !ent ) {
	    // In case the sender did not send the source request: 
	    // add data source on the fly....
	    p->handleSourceRequest(p->receivers().size(), source, source);
	    ent = p->receiver(source);
	  }
	  if ( ent ) {
	    if ( p->handleEventData(*ent, buffer, hdr.size).isSuccess() ) {
	      gauditask_task_unlock();
	      return;
	    }
	  }
	  ::fprintf(stdout,"Loosing EVENT....Cannot register data source:%s.\n",hdr.name);
	  ::fflush(stdout);
	}
	::operator delete(buffer);
	gauditask_task_unlock();
	return;
      }
      // Things go awfully wrong....e.g. finalize was called during data taking.
      ::fprintf(stdout,"Loosing event....was finailize called ?\n");
      ::fflush(stdout);
    }

  public:

    /// Standard algorithm constructor
    TransferDataReceiver(const std::string& nam, ISvcLocator* pSvc) 
      : LHCb::NetworkDataReceiver(nam, pSvc), m_finish(false), m_netPlug(0) 
    {
      declareProperty("NumThreads",m_nThreads=1);
    }

    /// Standard Destructor
    virtual ~TransferDataReceiver()   { }

    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()  {
      m_finish = false;
      std::string self = RTL::dataInterfaceName()+"::"+RTL::processName();
      m_netPlug = net_init(self, m_nThreads);
      net_subscribe(m_netPlug,this,WT_FACILITY_CBMEVENT,handle_event,handle_death);
      net_subscribe(m_netPlug,this,WT_FACILITY_CBMCONNECT,handle_request,handle_death);
      return StatusCode::SUCCESS;
    }

    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()  {
      m_finish = true;
      net_unsubscribe(m_netPlug,this,WT_FACILITY_CBMEVENT);
      net_unsubscribe(m_netPlug,this,WT_FACILITY_CBMCONNECT);
      net_close(m_netPlug);
      m_netPlug = 0;
      return StatusCode::SUCCESS;
    }

    /// Rearm network request for a single source
    virtual StatusCode rearmNetRequest(const RecvEntry& src)  {
      int sc = net_send(m_netPlug,"EVENT_REQUEST",14,src.service,WT_FACILITY_CBMREQEVENT);
      return sc==NET_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
    }

    /// Copy event data into buffer manager
    virtual StatusCode copyEventData(void* to, void* from, size_t len)  {
      if ( from != to )  {
        ::memcpy(to,from,len);
        delete [] (char*)from;
      }
      return StatusCode::SUCCESS;
    }

    /// Networking layer overload [Net producer+consumer]: Cancel current I/O
    virtual StatusCode cancelNetwork()   {
      net_cancel(m_netPlug);
      return StatusCode::SUCCESS;
    }
  };
}
DECLARE_NAMESPACE_SERVICE_FACTORY(Online,TransferDataReceiver)
