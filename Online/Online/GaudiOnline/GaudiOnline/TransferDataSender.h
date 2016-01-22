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
#include "GaudiOnline/NetworkDataSender.h"
#include "WT/wt_facilities.h"

/// Online namespace declaration
namespace Online  {

  using namespace TRANSFER_NS;

  /// Implementation of a generic network DataSender component
  /**
   *  Standard component template to implment DataSender component objects.
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
  class TransferDataSender : public LHCb::NetworkDataSender  {

    /// Static task-dead handler
    static void handle_death(const netheader_t& hdr, void* param, netentry_t*)   {
      TransferDataSender* p = reinterpret_cast<TransferDataSender*>(param);
      p->taskDead(hdr.name);
    }

    /// Statis event request handler
    static void handle_req(const netheader_t& hdr, void* param, netentry_t* entry)  {
      TransferDataSender* p = reinterpret_cast<TransferDataSender*>(param);
      int sc = net_receive(p->m_netPlug, entry, 0);
      if ( sc == NET_SUCCESS )  {
	p->handleEventRequest(p->m_recipients.size(),hdr.name,hdr.name);
      }
    }
    /// Pointer to netplug device
    NET*     m_netPlug;
    /// Number of thread instances used by the network library
    int      m_nThreads;

  public:
    /// Standard algorithm constructor
    TransferDataSender(const std::string& nam, ISvcLocator* pSvc) 
    : LHCb::NetworkDataSender(nam, pSvc), m_netPlug(0)  
    {
      declareProperty("NumThreads",m_nThreads=2);
    }

    /// Standard Destructor
    virtual ~TransferDataSender()   {}

    /// Subscribe to network requests
    virtual StatusCode subscribeNetwork()   {
      std::string self = RTL::dataInterfaceName()+"::"+RTL::processName();
      m_netPlug = net_init(self, m_nThreads);
      net_subscribe(m_netPlug,this,WT_FACILITY_CBMREQEVENT,handle_req,handle_death);
      if ( !m_target.empty() )  {
        const char* req = "EVENT_SOURCE";
        int sc = net_send(m_netPlug,req,strlen(req)+1,m_target,WT_FACILITY_CBMCONNECT);
        return sc==NET_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
    }

    /// Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork() {
      net_unsubscribe(m_netPlug,this,WT_FACILITY_CBMREQEVENT);
      net_close(m_netPlug);
      m_netPlug = 0;
      return StatusCode::SUCCESS;
    }

    /// Send data to target destination
    virtual StatusCode sendData(const Recipient& tar, const void* data, size_t len)  {
      int sc = net_send(m_netPlug,data,len,tar.name,WT_FACILITY_CBMEVENT);
      return sc==NET_SUCCESS ? StatusCode::SUCCESS : StatusCode::FAILURE;
    }

    /// Networking layer overload [Net producer+consumer]: Cancel current I/O
    virtual StatusCode cancelNetwork()   {
      net_cancel(m_netPlug);
      return StatusCode::SUCCESS;
    }
  };
}

namespace LHCb  { 
  class TransferDataSender : public Online::TransferDataSender  {
  public:
    TransferDataSender(const std::string& nam, ISvcLocator* pSvc) 
      : Online::TransferDataSender(nam, pSvc) {}
    virtual ~TransferDataSender() {}
  };   
}

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,TransferDataSender)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Online,TransferDataSender)
