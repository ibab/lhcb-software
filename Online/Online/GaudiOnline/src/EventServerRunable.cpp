// $Id: EventServerRunable.cpp,v 1.10 2009-01-27 08:30:01 cattanem Exp $
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
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/EventServerRunable.h"
#include "GaudiOnline/MEPManager.h"
#include "WT/wt_facilities.h"
#include "RTL/Lock.h"
#include "RTL/bits.h"

using namespace std;
using namespace LHCb;
using namespace DataTransfer;

static void handle_death(const netheader_t& hdr, void* param, netentry_t*)    {  
  EventServerRunable* p = (EventServerRunable*)param;
  p->removeTarget(hdr.name);
}

static void handle_req(const netheader_t& hdr, void* param, netentry_t* entry)  {
  EventServerRunable* p = (EventServerRunable*)param;
  try  {
    p->handleEventRequest(hdr, entry);
  }
  catch(...)  {
  }
}

// Standard Constructor
EventServerRunable::EventServerRunable(const string& nam, ISvcLocator* svcLoc)   
  : OnlineService(nam, svcLoc), m_mepMgr(0), 
    m_evtCount(0), m_consumer(0), m_netPlug(0), 
    m_suspend(0), m_consState(WAIT_REQ)
{
  declareProperty("EvtMax",    m_evtMax=1);
  declareProperty("REQ",       m_req);
  declareProperty("Input",     m_input="Events");
  declareProperty("MEPManager",m_mepMgrName="LHCb::MEPManager/MEPManager");
  declareProperty("PrintNum",  m_printNum=1000);
  declareProperty("NumThreads",m_nThreads=1);
  ::lib_rtl_create_event(0,&m_suspend);
  ::lib_rtl_create_lock(0,&m_lock);
  ::lib_rtl_create_lock(0,&m_rcpLock);
}

// Standard Destructor
EventServerRunable::~EventServerRunable()  {
  ::lib_rtl_delete_event(m_suspend);
  ::lib_rtl_delete_lock(m_lock);
  ::lib_rtl_delete_lock(m_rcpLock);
  m_suspend = 0;
  m_lock = 0;
}

// IInterface implementation : queryInterface
StatusCode EventServerRunable::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( IRunable::interfaceID().versionMatch(riid) )  {
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
  string self = RTL::dataInterfaceName()+"::"+RTL::processName();
  m_netPlug = net_init(self, m_nThreads);
  net_subscribe(netPlug(),this,WT_FACILITY_CBMREQEVENT,handle_req,handle_death);
  incidentSvc()->addListener(this,"DAQ_CANCEL");
  declareInfo("EvtCount",m_evtCount=0,"Number of events processed");
  //::lib_rtl_clear_event(m_suspend);
  return sc;
}

// IService implementation: start the service
StatusCode EventServerRunable::start()   {
  StatusCode sc = OnlineService::start();
  if ( !sc.isSuccess() )  {
    return error("Failed to start service base class.");
  }
  if ( !(sc=service(m_mepMgrName,m_mepMgr)).isSuccess() )  {
    return error("Failed to access MEP manager service.");
  }
  map<string,BMID>::const_iterator i = m_mepMgr->buffers().find(m_input);
  if ( i == m_mepMgr->buffers().end() ) {
    return error("Failed to access Input buffer:"+m_input+".");
  }
  m_request.parse(m_req);
  m_consumer = new MBM::Consumer((*i).second,RTL::processName(),m_mepMgr->partitionID());
  m_consState = WAIT_REQ;
  return sc;
}

// IService implementation: stop the service
StatusCode EventServerRunable::stop()   {
  StatusCode sc = OnlineService::stop();
  if ( !sc.isSuccess() )  {
    return error("Failed to stop service base class.");
  }
  if ( m_consumer ) {
    try  {
      m_consumer->delRequest(m_request);
    }
    catch(...)   {
    }
    delete m_consumer;
  }
  m_consumer = 0;
  releaseInterface(m_mepMgr);
  return sc;
}

// IService implementation: finalize the service
StatusCode EventServerRunable::finalize()     {
  if ( incidentSvc() )  {
    incidentSvc()->removeListener(this);
  }
  shutdownRequests();
  m_recipients.clear();
  if ( m_consumer ) {
    delete m_consumer;
  }
  m_consumer = 0;
  releaseInterface(m_mepMgr);
  return OnlineService::finalize();
}

/// Shutdown all event requests from the network
void EventServerRunable::shutdownRequests() {
  if ( netPlug() )  {
    net_unsubscribe(netPlug(),this,WT_FACILITY_CBMREQEVENT);
    net_close(netPlug());
  }
  m_recipients.clear();
  m_netPlug = 0;
}

/// Incident handler implemenentation: Inform that a new incident has occured
void EventServerRunable::handle(const Incident& inc)    {
  info("Got incident:"+inc.source()+" of type "+inc.type());
  if ( inc.type() == "DAQ_CANCEL" )  {
    m_consState = DONE;
    if ( !m_mepMgrName.empty() )  {
      if ( 0 == m_mepMgr && FSMState() == Gaudi::StateMachine::RUNNING ) {
        error("Got incident:"+inc.source()+
              " -- Internal error:"+m_mepMgrName+" is not assigned.");
      }
      else if ( m_consumer ) {
        m_consumer->cancel();
      }
      else if ( m_mepMgr ) {
        m_mepMgr->cancel();
      }
    }
    // First clear pending event flags, then set the event,
    // so that we exit the in ::run
    ::lib_rtl_clear_event(m_suspend);
    ::lib_rtl_set_event(m_suspend);
  }
  else if ( inc.type() == "DAQ_ENABLE" )  {
    m_consState = m_recipients.empty() ? WAIT_REQ : WAIT_EVT;
  }
}

void EventServerRunable::removeTarget(const string& src)   {
#ifdef EventServerRunable_USE_MAP
  Recipients::iterator j = m_recipients.find(src);
#else
  Recipients::iterator j = m_recipients.end();
  for(Recipients::iterator i=m_recipients.begin(); i!=m_recipients.end(); ++i)  {
    if ( src == (*i).first ) {
      j = i;
      break;
    }
  }
#endif
  if (j != m_recipients.end())   {
    try {
      ::lib_rtl_lock(m_lock);
      m_recipients.erase(j);
      if ( m_consState != DONE ) {
        restartRequests();
      }
    }
    catch(...) {
      error("Exception in removeTarget from "+src);
    }
    ::lib_rtl_unlock(m_lock);
  }
}

/// Handle request from new network data consumer
void EventServerRunable::handleEventRequest(const netheader_t& hdr, void* ptr)   {
  bool empty;
  char buff[2048];
  string src = hdr.name;
  netentry_t* e = (netentry_t*)ptr;
  MBM::Requirement* r = (MBM::Requirement*)buff;
  int sc = net_receive(netPlug(),e,buff);
  if ( sc == NET_SUCCESS )  {
    //info("Got event request from "+src);
    try {
      ::lib_rtl_lock(m_lock);
      empty = m_recipients.empty();
#ifdef EventServerRunable_USE_MAP
      m_recipients[src] = make_pair(*r,e);
#else
      m_recipients.push_back(make_pair(src,make_pair(*r,e)));
#endif
      if ( empty && m_consState != DONE ) ::lib_rtl_set_event(m_suspend);
      //info("Received event request from "+src);
    }
    catch(...) {
      info("Exception in handleEventRequest from "+src);
    }
    ::lib_rtl_unlock(m_lock);
  }
}

/// Restart event request handling
void EventServerRunable::restartRequests()  {
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
  //debug("restartRequests: Recipients:%d\n", int(m_recipients.size()));
  if ( m_consumer )   {
    m_consumer->delRequest(old);
    if ( m_recipients.size() > 0 )  {
      m_consumer->addRequest(m_request);
      m_consState = WAIT_EVT;
    }
  }
}

StatusCode EventServerRunable::sendEvent()  {
  int cnt = 0;
  const MBM::EventDesc& e = m_consumer->event();
  for(Recipients::iterator i=m_recipients.begin(); i!=m_recipients.end(); )  {
    MBM::Requirement& r = (*i).second.first;
    if ( e.type == r.evtype )  {
      if ( mask_or_ro2(e.mask, r.trmask,sizeof(e.mask)/sizeof(e.mask[0])) )  {
        if ( !mask_and_ro2(e.mask,r.vetomask,sizeof(e.mask)/sizeof(e.mask[0])) )  {
          int sc = net_send(m_netPlug,e.data,e.len,(*i).first,WT_FACILITY_CBMEVENT);
          if ( sc==NET_SUCCESS )   {
            m_recipients.erase(i);
            i = m_recipients.begin();
            ++cnt;
            continue;
          }
          error("Cannot Send event data to "+(*i).first);
        }
      }
    }
    ++i;
  }
  m_consumer->freeEvent();
  if ( m_recipients.empty() )  {
    m_consumer->delRequest(m_request);
  }
  return cnt>0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// IRunable implementation : Run the class implementation
StatusCode EventServerRunable::run()   {
  int sc;
  bool empty;
  int printN = 0;
  while ( 1 )  {
    ::lib_rtl_lock(m_lock);
    switch(m_consState)  {
    case WAIT_REQ:
      //info("Wait for event requests......");
      if ( m_consState == DONE )  {
        info("End of event requests reached. Stopping...");
        ::lib_rtl_clear_event(m_suspend);
        ::lib_rtl_unlock(m_lock);
        return StatusCode::SUCCESS;
      }
      empty = m_recipients.empty();
      if ( empty ) ::lib_rtl_clear_event(m_suspend);
      // Unlock now and allow for new requests being added
      ::lib_rtl_unlock(m_lock);
      if ( empty && m_consState != DONE ) {
        ::lib_rtl_wait_for_event(m_suspend);
      }
      // Check if we got a cancel in the meantime
      if ( m_consState == DONE )  {
        info("End of event requests reached. Stopping...");
        ::lib_rtl_clear_event(m_suspend);
        return StatusCode::SUCCESS;
      }
      restartRequests();
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
          RTL::Lock obj_lock(m_lock);
          sendEvent();
          if ( ((++printN)%m_printNum) == 0 ) {
            info("Delivered %9d events to clients.",printN);
          }
          net_unlock(netPlug(),lock);
          lock = 0;
        }
        catch(...)  {
          info("Exception....");
          if ( lock ) net_unlock(netPlug(),lock);
        }
        if ( m_consState == DONE )  {
          return StatusCode::SUCCESS;
        }
        m_consState = WAIT_REQ;
      }
      break;
    case DONE:
      ::lib_rtl_unlock(m_lock);
      return StatusCode::SUCCESS;
    }
  }
}
