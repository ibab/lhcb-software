// $Id: NetworkEvtSelector.cpp,v 1.5 2009-01-12 16:30:39 frankb Exp $
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
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "Event/RawBank.h"
#include "NET/DataTransfer.h"
#include "WT/wt_facilities.h"
#include "MBM/mepdef.h"
#include "RTL/Lock.h"

using namespace std;
using namespace LHCb;
using namespace DataTransfer;

static void handle_death(const netheader_t& h, void* param, netentry_t*) {
  NetworkContext* p = (NetworkContext*)param;
  p->taskDead(h.name);
}

static void handle_event(const netheader_t& header, void* param, netentry_t* entry)  {
  NetworkContext* p = (NetworkContext*)param;
  char* buff = new char[header.size];
  int sc = net_receive(p->m_netPlug, entry, buff);
  (sc == NET_SUCCESS) ? p->handleData(header.name,header.size,buff) : delete [] buff;
}

NetworkContext::NetworkContext(NetworkEvtSelector* s)
  : OnlineContext(s), m_cancelled(false)
{
  m_evdesc.setEventType(EVENT_TYPE_EVENT);
}

void NetworkContext::taskDead(const string& who)  {
  OnlineBaseEvtSelector* thisPtr = const_cast<OnlineBaseEvtSelector*>(m_sel);
  NetworkEvtSelector* sel = dynamic_cast<NetworkEvtSelector*>(thisPtr);
  sel->error("The event data source "+who+" died ....");
  freeEvent();
  if ( sel->cancelOnDeath() ) {
    sel->handle(Incident(sel->name(),"DAQ_CANCEL"));
  }
}

StatusCode NetworkContext::freeEvent()  {
  if ( m_needFree )   {
    try  {
      m_needFree = false;
      delete [] (char*)m_evdesc.header();
      m_evdesc.setSize(0);
      m_evdesc.setHeader(0);
    }
    catch(...) {
    }
  }
  return StatusCode::SUCCESS;
}

void NetworkContext::handleData(const string& src, size_t siz, char* buff) {
  MsgStream log(m_sel->msgSvc(),m_sel->name());
  log << MSG::DEBUG << "Got event data from " << src << ": " << siz <<" Bytes" << endreq;
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
      MsgStream log(m_sel->msgSvc(),m_sel->name());
      log << MSG::DEBUG << "Sent event request to " << m_input << endreq;
      return StatusCode::SUCCESS;
    }
    MsgStream log(m_sel->msgSvc(),m_sel->name());
    log << MSG::ERROR << "Failed to send event request to " << m_input << endreq;
    return StatusCode::FAILURE;
  }
  catch(const exception& e)  {
    m_sel->error(string("Failed to read next event:")+e.what());
  }
  catch(...)  {
    m_sel->error("Failed to read next event - Unknown exception.");
  }
  return StatusCode::FAILURE;
}

StatusCode NetworkContext::receiveEvent()  {
  if ( !m_cancelled ) {
    m_sel->increaseEvtCount();
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode NetworkContext::connect(const string& input)  {
  string self = RTL::dataInterfaceName()+"::"+RTL::processName();
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
    m_cancelled = false;
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Close network connection
void NetworkContext::close()  {
  net_unsubscribe(m_netPlug,this,WT_FACILITY_CBMEVENT);
  net_close(m_netPlug);
  m_cancelled = false;
  m_netPlug = 0;
}

/// Standard constructor
NetworkEvtSelector::NetworkEvtSelector(const string& nam, ISvcLocator* svc)
: OnlineBaseEvtSelector(nam,svc)
{
  m_decode = false;
  m_allowSuspend = true;
  declareProperty("CancelOnDeath",m_cancelDeath=false);
}

/// Create event selector iteration context
StatusCode NetworkEvtSelector::createContext(Context*& refpCtxt) const  {
  NetworkContext* ctxt = new NetworkContext(const_cast<NetworkEvtSelector*>(this));
  refpCtxt = ctxt;
  return ctxt->connect(m_input);
}
