// $Id: EventServerRunable.cpp,v 1.5 2008-05-21 09:51:50 frankm Exp $
//====================================================================
//  EventServerRunable
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: Runable to serv independent network clients with event
//               data. requests are purely served on demand ie. each
//               client has to ask for every event to be processed.
//
//  Author     : M.Frank
//  Created    : 4/12/2007
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/EventServerRunable.cpp,v 1.5 2008-05-21 09:51:50 frankm Exp $
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/EventServerRunable.h"
#include "GaudiOnline/MEPManager.h"
#include "WT/wt_facilities.h"
#include "RTL/bits.h"

// Instantiation of a static factory class used by clients to create instances of this service
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,EventServerRunable)

using namespace LHCb;
using namespace DataTransfer;

static void handle_death(netentry_t* /* e */, const netheader_t& hdr, void* param)    {  
  EventServerRunable* p = (EventServerRunable*)param;
  p->removeTarget(hdr.name);
}

static void handle_req(netentry_t* e, const netheader_t& hdr, void* param)  {
  EventServerRunable* p = (EventServerRunable*)param;
  try  {
    p->handleEventRequest(e,hdr);
  }
  catch(...)  {
  }
}

// Standard Constructor
EventServerRunable::EventServerRunable(const std::string& nam, ISvcLocator* svcLoc)   
: OnlineService(nam, svcLoc), m_mepMgr(0), 
  m_evtCount(0), m_consumer(0), m_netPlug(0), 
  m_suspend(0), m_consState(WAIT_REQ)
{
  declareProperty("EvtMax",    m_evtMax=1);
  declareProperty("REQ",       m_req);
  declareProperty("Input",     m_input="Events");
  declareProperty("MEPManager",m_mepMgrName="LHCb::MEPManager/MEPManager");
  declareProperty("PrintNum",  m_printNum=1000);
  ::lib_rtl_create_event(0,&m_suspend);
  ::lib_rtl_create_lock(0,&m_lock);
}

// Standard Destructor
EventServerRunable::~EventServerRunable()  {
  ::lib_rtl_delete_event(m_suspend);
  ::lib_rtl_delete_lock(m_lock);
  m_suspend = 0;
  m_lock = 0;
}

// IInterface implementation : queryInterface
StatusCode EventServerRunable::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( riid == IID_IRunable )  {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

// IService implementation: initialize the service
StatusCode EventServerRunable::initialize()   {
  StatusCode sc = OnlineService::initialize();
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize service base class.");
  }
  if ( !(sc=service(m_mepMgrName,m_mepMgr)).isSuccess() )  {
    return error("Failed to access MEP manager service.");
  }
  std::map<std::string,BMID>::const_iterator i = m_mepMgr->buffers().find(m_input);
  if ( i == m_mepMgr->buffers().end() ) {
    return error("Failed to access Input buffer:"+m_input+".");
  }
  m_consumer = new MBM::Consumer((*i).second,RTL::processName(),m_mepMgr->partitionID());
  // m_consumer->include();
  m_request.parse(m_req);
  m_consState = WAIT_REQ;
  std::string self = RTL::dataInterfaceName()+"::"+RTL::processName();
  m_netPlug = net_init(self);
  net_subscribe(netPlug(),this,WT_FACILITY_CBMREQEVENT,handle_req,handle_death);
  incidentSvc()->addListener(this,"DAQ_CANCEL");
  declareInfo("EvtCount",m_evtCount=0,"Number of events processed");
  m_reqActive = false;
  return sc;
}

// IService implementation: finalize the service
StatusCode EventServerRunable::finalize()     {
  if ( incidentSvc() )  {
    incidentSvc()->removeListener(this);
  }
  m_recipients.clear();
  if ( netPlug() )  {
    net_unsubscribe(netPlug(),this,WT_FACILITY_CBMREQEVENT);
    net_close(netPlug());
  }
  m_netPlug = 0;
  if ( m_consumer ) {
    // m_consumer->exclude();
    delete m_consumer;
  }
  m_consumer = 0;
  releaseInterface(m_mepMgr);
  return OnlineService::finalize();
}

/// Incident handler implemenentation: Inform that a new incident has occured
void EventServerRunable::handle(const Incident& inc)    {
  info("Got incident:"+inc.source()+" of type "+inc.type());
  if ( inc.type() == "DAQ_CANCEL" )  {
    m_consState = DONE;
    if ( !m_mepMgrName.empty() )  {
      if ( 0 == m_mepMgr ) {
        error("Got incident:"+inc.source()+
              " -- Internal error:"+m_mepMgrName+" is not assigned.");
      }
      else {
        m_mepMgr->cancel();
      }
    }
    ::lib_rtl_set_event(m_suspend);
  }
  else if ( inc.type() == "DAQ_ENABLE" )  {
    m_consState = m_recipients.empty() ? WAIT_REQ : WAIT_EVT;
  }
}

void EventServerRunable::removeTarget(const std::string& src)   {
  Recipients::iterator j = m_recipients.find(src);
  if (j != m_recipients.end())   {
    try {
      ::lib_rtl_lock(m_lock);
      m_recipients.erase(j);
      restart();
    }
    catch(...) {
      MsgStream log(msgSvc(),name());
      log << MSG::INFO << "Exception in removeTarget from " << src << endreq;
    }
    ::lib_rtl_unlock(m_lock);
  }
}

    
void EventServerRunable::handleEventRequest(netentry_t* e, const netheader_t& hdr)   {
  char buff[2048];
  std::string src = hdr.name;
  MBM::Requirement* r = (MBM::Requirement*)buff;
  int sc = net_receive(netPlug(),e,buff);
  if ( sc == NET_SUCCESS )  {
    // MsgStream log(msgSvc(),name());
    // log << MSG::INFO << "Got event request from " << src << endreq;
    try {
      ::lib_rtl_lock(m_lock);
      m_recipients[src] = std::make_pair(*r,e);
      restart();
    }
    catch(...) {
      MsgStream log(msgSvc(),name());
      log << MSG::INFO << "Exception in handleEventRequest from " << src << endreq;
    }
    ::lib_rtl_unlock(m_lock);
  }
}

void EventServerRunable::restart()  {
  MBM::Requirement old = m_request;
  for(int j=0; j<BM_MASK_SIZE;++j)  {
    m_request.trmask[j] = 0;
    m_request.vetomask[j] = ~0;
  }
  for(Recipients::iterator i=m_recipients.begin(); i!=m_recipients.end(); ++i)  {
    MBM::Requirement& r = (*i).second.first;
    for(int k=0; k<BM_MASK_SIZE; ++k)  {
      m_request.trmask[k] |= r.trmask[k];
      m_request.vetomask[k] &= r.vetomask[k];
    }
  }
  if ( m_reqActive ) {
    m_consumer->delRequest(old);
    m_reqActive = false;
  }
  if ( m_recipients.empty() )  {
    //m_consumer->pause();
    if ( m_consState != WAIT_REQ ) {
      m_consState = WAIT_REQ;
      ::lib_rtl_clear_event(m_suspend);
    }
  }
  else  {
    m_consumer->addRequest(m_request);
    m_reqActive = true;
    if ( m_consState == WAIT_EVT ) {
      m_consumer->cancel();
    }
    else if ( m_consState == WAIT_REQ )
      ::lib_rtl_set_event(m_suspend);
  }
}

void EventServerRunable::sendEvent()  {
  const MBM::EventDesc& e = m_consumer->event();
  for(Recipients::iterator i=m_recipients.begin(); i!=m_recipients.end(); )  {
    MBM::Requirement& r = (*i).second.first;
    if ( e.type == r.evtype )  {
      if ( mask_or_ro2(e.mask, r.trmask,sizeof(e.mask)/sizeof(e.mask[0])) )  {
        if ( !mask_and_ro2(e.mask,r.vetomask,sizeof(e.mask)/sizeof(e.mask[0])) )  {
          int sc = net_send(m_netPlug,e.data,e.len,(*i).second.second,WT_FACILITY_CBMEVENT);
          if ( sc==NET_SUCCESS )   {
            m_recipients.erase(i);
            i = m_recipients.begin();
            continue;
          }
	  MsgStream log(msgSvc(),name());
	  log << MSG::ERROR << "Cannot Send event data to " << (*i).first << endreq;
        }
      }
    }
    ++i;
  }
}

// IRunable implementation : Run the class implementation
StatusCode EventServerRunable::run()   {
  int sc;
  int printN = 0;
  while ( 1 )  {
    ::lib_rtl_lock(m_lock);
    switch(m_consState)  {
      case WAIT_REQ:
        ::lib_rtl_unlock(m_lock);
        ::lib_rtl_wait_for_event(m_suspend);
        if ( m_consState == DONE )  {
	  MsgStream log(msgSvc(),name());
	  log << MSG::INFO << "End of event requests reached. Stopping..." << endreq;
          return StatusCode::SUCCESS;
        }
        m_consState = WAIT_EVT;
        break;
      case WAIT_EVT:
        ::lib_rtl_unlock(m_lock);
        sc = m_consumer->getEvent();
        if ( m_consState == DONE )  {
          return StatusCode::SUCCESS;
        }
        else if ( sc == MBM_NORMAL )  {
          void* lock = 0;
          try {
            // Need to first lock all network channels to avoid dead-locks
            // In the event a data-ready callback is already active, which
            // would also want to task the local lock.
            // Only afterwards it is safe to send data.
            lock = net_lock(netPlug());
            ::lib_rtl_lock(m_lock);
            sendEvent();
	    if ( ((++printN)%m_printNum) == 0 ) {
	      MsgStream log(msgSvc(),name());
	      log << MSG::INFO << "Delivered " << printN << " events to clients." << endreq;
	    }
            net_unlock(netPlug(),lock);
            lock = 0;
            restart();
            ::lib_rtl_unlock(m_lock);
          }
          catch(...)  {
	    MsgStream log(msgSvc(),name());
	    log << MSG::INFO << "Exception...." << endreq;
            if ( lock ) net_unlock(netPlug(),lock);
            ::lib_rtl_unlock(m_lock);
          }
        }
        break;
      case DONE:
        ::lib_rtl_unlock(m_lock);
        return StatusCode::SUCCESS;
    }
  }
}
