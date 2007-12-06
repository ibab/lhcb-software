// $Id: NetworkEvtSelector.cpp,v 1.1 2007-12-06 14:39:35 frankb Exp $
//====================================================================
//  NetworkEvtSelector.cpp
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: The GaudiOnline::NetworkEvtSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//  Author     : M.Frank
//====================================================================
#include "GaudiOnline/NetworkEvtSelector.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "Event/RawBank.h"
#include "NET/DataTransfer.h"
#include "WT/wt_facilities.h"
#include "MBM/mepdef.h"
#include "RTL/Lock.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,NetworkEvtSelector)
using namespace LHCb;
using namespace DataTransfer;

static void handle_death(netentry_t* /* e */, const netheader_t& h, void* param) {
  NetworkContext* p = (NetworkContext*)param;
  p->taskDead(h.name);
}

static void handle_event(netentry_t* e, const netheader_t& h, void* param)  {
  NetworkContext* p = (NetworkContext*)param;
  char* buff = new char[h.size];
  int sc = net_receive(p->m_netPlug,e,buff);
  (sc == NET_SUCCESS) ? p->handleData(h.name,h.size,buff) : delete [] buff;
}

NetworkContext::NetworkContext(const NetworkEvtSelector* s)
: OnlineContext(s)
{
}

void NetworkContext::taskDead(const std::string& who)  {
  m_sel->error("The event data source "+who+" died ....");
}

StatusCode NetworkContext::freeEvent()  {
  if ( m_needFree )   {
    try  {
      m_needFree = false;
      delete [] (char*)m_evdesc.header();
    }
    catch(...) {
    }
  }
  return StatusCode::SUCCESS;
}

void NetworkContext::handleData(const std::string& src, size_t siz, char* buff) {
  m_banks.clear();
  decodeRawBanks(buff,buff+siz,m_banks).ignore();
  for(Banks::const_iterator i=m_banks.begin(); i != m_banks.end(); ++i)  {
    RawBank* b = *i;
    if ( b->version() == DAQ_STATUS_BANK )  {
      MDFHeader* h = b->begin<MDFHeader>();
      m_evdesc.setPartitionID(0x1);
      m_evdesc.setTriggerMask(h->subHeader().H1->triggerMask());
      m_evdesc.setEventType(EVENT_TYPE_EVENT);
      m_evdesc.setHeader(buff);
      m_evdesc.setSize(siz);
      m_needFree = true;
      m_sel->resume();
      return;
    }
  }
  delete [] buff;
}

StatusCode NetworkContext::rearmEvent()  {
  m_banks.clear();
  try  {
    m_sel->increaseReqCount();
    m_needFree = false;
    m_sel->suspend();
    int sc = net_send(m_netPlug,&m_request,sizeof(m_request),m_input,WT_FACILITY_CBMREQEVENT);
    if ( sc == NET_SUCCESS )  {
      return StatusCode::SUCCESS;
    }
    return StatusCode::SUCCESS;
  }
  catch(const std::exception& e)  {
    m_sel->error(std::string("Failed to read next event:")+e.what());
  }
  catch(...)  {
    m_sel->error("Failed to read next event - Unknown exception.");
  }
  return StatusCode::FAILURE;
}

StatusCode NetworkContext::receiveEvent()  {
  m_sel->increaseEvtCount();
  return StatusCode::SUCCESS;
}

StatusCode NetworkContext::connect(const std::string& input)  {
  std::string self = RTL::dataInterfaceName()+"::"+RTL::processName();
  if ( m_sel->numReq() > 0 )   {
    m_request = m_sel->req(0);
    for(int j=0; j<BM_MASK_SIZE;++j)  {
      m_request.trmask[j] = 0;
      m_request.vetomask[j] = ~0;
    }
    for (int i=0, n=m_sel->numReq(); i<n; ++i )  {
      const MBM::Requirement& r = m_sel->req(i);
      for(int k=0; k<BM_MASK_SIZE; ++k)  {
        m_request.trmask[k]   |= r.trmask[k];
        m_request.vetomask[k] &= r.vetomask[k];
      }
    }
    m_input = input;
    m_netPlug = net_init(self, NET_CLIENT);
    net_subscribe(m_netPlug,this,WT_FACILITY_CBMEVENT,handle_event,handle_death);
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Close network connection
void NetworkContext::close()  {
  net_unsubscribe(m_netPlug,this,WT_FACILITY_CBMEVENT);
  net_close(m_netPlug);
  m_netPlug = 0;
}

/// Standard constructor
NetworkEvtSelector::NetworkEvtSelector(const std::string& nam, ISvcLocator* svc)
: OnlineBaseEvtSelector(nam,svc)
{
  m_decode = false;
  m_allowSuspend = true;
}

/// Create event selector iteration context
StatusCode NetworkEvtSelector::createContext(Context*& refpCtxt) const  {
  NetworkContext* ctxt = new NetworkContext(this);
  refpCtxt = ctxt;
  return ctxt->connect(m_input);
}
