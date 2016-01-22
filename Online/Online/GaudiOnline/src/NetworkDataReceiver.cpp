// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/NetworkDataReceiver.cpp,v 1.24 2010-09-21 14:28:26 frankb Exp $
//  ====================================================================
//  NetworkDataReceiver.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiOnline/NetworkDataReceiver.h"
#include "GaudiOnline/MEPManager.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/xtoa.h"
#include "Event/RawBank.h"
#include "MDF/MDFHeader.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include "RTL/Lock.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include <algorithm>
#include <memory>
#include <cmath>

using namespace LHCb;
static const std::string s_reqMsg("EVENT_REQUEST");


static void _doSleep(int errorDelay,bool* recvEvents)  {
  while(errorDelay>0 )  {
    if ( !(*recvEvents) ) break;
    errorDelay-= 10;
  }
}

// Standard algorithm constructor
NetworkDataReceiver::NetworkDataReceiver(const std::string& nam, ISvcLocator* pSvc)
  : OnlineService(nam,pSvc), m_recvReq(0), m_recvError(0), m_recvBytes(0), 
    m_backlog(0), m_lastbacklog(0), m_mepMgr(0), m_prod(0), m_evtSelector(0),
    m_lock(0), m_recvEvents(false)
{
  ::wtc_init();
  ::lib_rtl_create_lock(0,&m_lock);
  declareProperty("Buffer",              m_buffer);
  declareProperty("UseEventRequests",    m_useEventRequests=false);
  declareProperty("DeclareAsynchonously",m_declareAsynch=false);
  declareProperty("RoutingMask",         m_routingMask=0);
  declareProperty("VetoMask",            m_vetoMask=0);
  declareProperty("ErrorDelay",          m_errorDelay=100);
}

// Standard Destructor
NetworkDataReceiver::~NetworkDataReceiver()      {
  ::lib_rtl_delete_lock(m_lock);
}

// WT callback to declare asynchronously event data to MBM
int NetworkDataReceiver::rearm_net_request(unsigned int,void* param)  {
  std::auto_ptr<RecvEntry> h((RecvEntry*)param);
  h->handler->declareEventData(*h.get());
  return WT_SUCCESS;
}

// Initialize the object: allocate all necessary resources
StatusCode NetworkDataReceiver::initialize()   {
  StatusCode sc = OnlineService::initialize();
  MsgStream err(msgSvc(), name());
  if ( !sc.isSuccess() )  {
    return sc;
  }
  try {
    m_recvEvents = true;
    // Do NOT call base class initialization: we are not writing to file/socket!
    declareInfo("EventsIn", m_recvReq=0,   "Total number of items received.");
    declareInfo("ErrorsIn", m_recvError=0, "Total number of receive errors.");
    declareInfo("BytesIn",  m_recvBytes=0, "Total number of bytes received from clients.");
    sc = service("MEPManager",m_mepMgr);
    if ( !sc.isSuccess() )  {
      MsgStream info(msgSvc(), name());
      info << MSG::INFO << "Failed to access buffer manager service." << endmsg;
      return sc;
    }
    sc = subscribeNetwork();
    if ( !sc.isSuccess() )  {
      return sc;
    }
    m_prod = m_mepMgr->createProducer(m_buffer,RTL::processName());
    ::wtc_subscribe(WT_FACILITY_DAQ_EVENT,0,rearm_net_request,this);
    incidentSvc()->addListener(this,"DAQ_CANCEL");
    return sc;
  }
  catch(const std::exception& e)  {
    err << MSG::ERROR << "Exception during initialization:" << e.what() << endmsg;
  }
  catch(...)  {
    err << MSG::ERROR << "Unknown exception during initialization:" << endmsg;
  }
  return StatusCode::FAILURE;
}

// Finalize the object: release all allocated resources
StatusCode NetworkDataReceiver::finalize()     {
  m_recvEvents = false;
  ::wtc_flush(WT_FACILITY_DAQ_EVENT);
  ::wtc_remove(WT_FACILITY_DAQ_EVENT);
  unsubscribeNetwork(); // Cannot really do anything if this fails...except printing
  for(Receivers::iterator i=m_receivers.begin(); i != m_receivers.end(); ++i)
    deleteNetRequest(*i);
  m_receivers.clear();
  if ( m_prod ) delete m_prod;
  m_prod = 0;
  if ( m_mepMgr ) m_mepMgr->release();
  m_mepMgr = 0;
  undeclareInfo("EventsIn");
  undeclareInfo("ErrorsIn");
  undeclareInfo("BytesIn");
  return OnlineService::finalize();
}

// Service overload: Start transition callback
StatusCode NetworkDataReceiver::start()   {
  m_recvEvents = true;
  if ( m_prod ) m_prod->clear();
  return OnlineService::start();
}

// Service overload: Stop transition callback
StatusCode NetworkDataReceiver::stop()   {
  m_recvEvents = false;
  if ( m_prod ) m_prod->clear();
  return OnlineService::stop();
}

/// Incident handler implemenentation: Inform that a new incident has occured
void LHCb::NetworkDataReceiver::handle(const Incident& inc)    {
  if ( inc.type() == "DAQ_CANCEL" )  {
    MsgStream info(msgSvc(), name());
    m_recvEvents = false;
    info << MSG::INFO << "Executing DAQ_CANCEL" << endmsg;
    ::wtc_flush(WT_FACILITY_DAQ_EVENT);
    m_mepMgr->cancel();
    cancelNetwork();
  }
}

// Handle event data source registration
StatusCode 
NetworkDataReceiver::handleSourceRequest(int clientID,
                                         const std::string& source,
                                         const std::string& svc)  
{
  try {
    if ( 0 == receiver(source) )  {
      m_receivers.push_back(RecvEntry(this,source,svc,clientID));
      RecvEntry& entry = m_receivers.back();
      StatusCode sc    = createNetRequest(entry);
      if ( sc.isSuccess() )  {
        MsgStream log(msgSvc(), name());
        log << MSG::DEBUG << "Added new datasource [" << clientID << "]:" << source 
            << " [" << svc << "]" << endmsg;
        return rearmRequest(entry);
      }
    }
    MsgStream info(msgSvc(), name());
    info << MSG::INFO << "Unknown datasource request from:" << source 
         << " [" << svc << "]" << endmsg;
    return StatusCode::SUCCESS;
  }
  catch(const std::exception& e)   {
    return errorException("Exception during data source request:",e);
  }
  catch(...)   {
    return error("Unknown exception during data source request.");
  }
}

// Producer implementation: Request event from data source
NetworkDataReceiver::RecvEntry* NetworkDataReceiver::receiver(const std::string& nam)  {
  for(Receivers::iterator i=m_receivers.begin(); i != m_receivers.end(); ++i)  {
    if ( 0 == ::strcasecmp(nam.c_str(), (*i).name.c_str()) )  {
      return &(*i);
    }
  }
  return 0;
}

/// Rearm network request for a single event source
StatusCode NetworkDataReceiver::rearmRequest(const RecvEntry& entry)   {
  if ( m_useEventRequests )  {
    StatusCode sc = rearmNetRequest(entry);
    sc.ignore();
    if ( !m_recvEvents ) sc = StatusCode::SUCCESS;
    return sc;
  }
  return StatusCode::SUCCESS;
}

// Create network request for a single source
StatusCode NetworkDataReceiver::createNetRequest(RecvEntry& /* entry */)  {
  return StatusCode::SUCCESS;
}

// Reset event request
StatusCode NetworkDataReceiver::resetNetRequest(const RecvEntry& /* entry */)  {
  return StatusCode::SUCCESS;
}

// Rearm network request for a single source
void NetworkDataReceiver::deleteNetRequest(RecvEntry& /* entry */)  {
}

// Reset event request and insert entry into data queue of the buffer manager
StatusCode 
NetworkDataReceiver::handleEventData(const std::string& src, void* buf, size_t len)  {
  try  {
    RecvEntry* entry = receiver(src);
    if ( entry )  {
      return handleEventData(*entry,buf,len);
    }
    return error("Event entry received from unknown source:"+src);
  }
  catch(std::exception& e)   {
    return errorException("Exception during event receive request:",e);
  }
  catch(...)   {
    return error("Unknown exception during event receive request.");
  }
}

// Reset event request and insert entry into data queue of the buffer manager
StatusCode 
NetworkDataReceiver::handleEventData(const RecvEntry& entry,void* buf,size_t len)  {
  try  {
    // Need to clone entry - new message may be arriving before this one is handled...
    // (In mode without explicit rearm)
    RecvEntry* e = new RecvEntry(entry);
    e->buffer = (char*)buf;
    e->size = len;
    StatusCode sc = resetNetRequest(entry);
    if ( m_declareAsynch )  {
      ::wtc_insert(WT_FACILITY_DAQ_EVENT,e);
      int backlog = ++m_backlog;
      if ( backlog%100 == 0 ) {
        if ( m_lastbacklog != backlog ) {
          m_lastbacklog = backlog;
          error("Message backlog is now %d messages.",backlog);
        }
      }
    }
    else  {
      NetworkDataReceiver::rearm_net_request(WT_FACILITY_DAQ_EVENT,e);
    }
    return sc;
  }
  catch(std::exception& e)   {
    ++m_recvError;
    return errorException("Exception during event receive request:",e);
  }
  catch(...)   {
    ++m_recvError;
    return error("Unknown exception during event receive request.");
  }
}

// Copy event data into buffer manager
StatusCode NetworkDataReceiver::copyEventData(void* to, void* from, size_t len)  {
  ::memcpy(to, from, len);
  return StatusCode::SUCCESS;
}

// Handle event declaration into the buffer manager
StatusCode NetworkDataReceiver::declareEventData(RecvEntry& entry)  {
  try {
    int ret = m_recvEvents ? m_prod->getSpace(entry.size) : MBM_REQ_CANCEL;
    if ( MBM_NORMAL == ret ) {
      MBM::EventDesc& e = m_prod->event();
      RawBank*   b = (RawBank*)e.data;
      MDFHeader* h = (MDFHeader*)b->data();
      e.type       = EVENT_TYPE_EVENT;
      e.len        = entry.size;
      if ( copyEventData(e.data,entry.buffer,entry.size).isSuccess() )  {
        ::memcpy(e.mask,h->subHeader().H1->triggerMask(),sizeof(e.mask));
        e.mask[3] &= ~m_vetoMask;
        e.mask[3] |= m_routingMask;
        ret = m_prod->sendEvent();
        if ( MBM_NORMAL == ret )   {
          --m_backlog;
          ++m_recvReq;
          m_recvBytes += entry.size;
#if 0
          MsgStream log(msgSvc(), name());
          log << MSG::ALWAYS << "Declared event of size:" << entry.size 
              << " to MBM from:" << entry.name << " Routing:" << std::hex 
              << h->subHeader().H1->triggerMask()[3] << endmsg;
#endif
          return rearmRequest(entry);
        }
      }
      MsgStream log0(msgSvc(), name());
      log0 << MSG::ERROR << "Failed to retrieve network event data from:" << entry.name << endmsg;
      if ( m_recvEvents && ret != MBM_REQ_CANCEL ) rearmRequest(entry);
      ++m_recvError;
      _doSleep(m_errorDelay,&m_recvEvents);
      return StatusCode::SUCCESS;
    }
    MsgStream log1(msgSvc(), name());
    // Cannot do anything - must handle and rearm new request
    log1 << MSG::ERROR << "Failed to get space from buffer manager." << endmsg;
    if ( m_recvEvents && ret != MBM_REQ_CANCEL ) rearmRequest(entry);
    ++m_recvError;
    _doSleep(m_errorDelay,&m_recvEvents);
    return StatusCode::SUCCESS;
  }
  catch(std::exception& e)  {
    MsgStream err(msgSvc(), name());
    err << MSG::ERROR << "Got exception when declaring event from:" << entry.name << endmsg;
    err << e.what() << endmsg;
  }
  catch(...)  {
    MsgStream err(msgSvc(), name());
    err << MSG::ERROR << "Got unknown exception when declaring event from:" << entry.name << endmsg;
  }
  ++m_recvError;
  if ( m_errorDelay>0 ) ::lib_rtl_sleep(m_errorDelay);
  _doSleep(m_errorDelay,&m_recvEvents);
  return m_recvEvents ? rearmRequest(entry) : StatusCode::SUCCESS;
}

// Callback on task dead notification
StatusCode NetworkDataReceiver::taskDead(const std::string& task_name)  {
  MsgStream info(msgSvc(), name());
  info << MSG::INFO << "Dead task:" << task_name << "...." << endmsg;
  for(Receivers::iterator i=m_receivers.begin(); i!=m_receivers.end(); ++i)  {
    if ( 0 == strcasecmp(task_name.c_str(),(*i).name.c_str()) )  {
      m_receivers.erase(i);
      break;
      //i = m_receivers.begin();
    }
  }
  // error("Event provider:"+task_name+" died.");
  return StatusCode::SUCCESS;
}
