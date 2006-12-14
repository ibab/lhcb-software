// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/NetworkDataReceiver.cpp,v 1.1 2006-12-14 18:59:21 frankb Exp $
//  ====================================================================
//  NetworkDataReceiver.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiOnline/NetworkDataReceiver.h"
#include "GaudiKernel/strcasecmp.h"
#include "Event/RawBank.h"
#include "MDF/MDFHeader.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include "RTL/Lock.h"
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include <algorithm>
#include <cmath>

using namespace LHCb;
static const std::string s_reqMsg("EVENT_REQUEST");

// Standard algorithm constructor
NetworkDataReceiver::NetworkDataReceiver(const std::string& nam, ISvcLocator* pSvc)
: OnlineService(nam,pSvc), m_recvReq(0), m_recvError(0), m_recvBytes(0)
{
  ::wtc_init();
  ::lib_rtl_create_lock(0,&m_lock);
  declareProperty("PartitionID",      m_partitionID);
  declareProperty("Buffer",           m_buffer);
  declareProperty("PartitionBuffer",  m_partitionBuffer=false);
  declareProperty("UseEventRequests", m_useEventRequests=false);
}

// Standard Destructor
NetworkDataReceiver::~NetworkDataReceiver()      {
  ::lib_rtl_delete_lock(m_lock);
}

// WT callback to declare asynchronously event data to MBM
int NetworkDataReceiver::rearm_net_request(unsigned int,void* param)  {
  RecvEntry* h = (RecvEntry*)param;
  h->handler->declareEventData(*h);
  return WT_SUCCESS;
}

// Initialize the object: allocate all necessary resources
StatusCode NetworkDataReceiver::initialize()   {
  StatusCode sc = OnlineService::initialize();
  MsgStream err(msgSvc(), name());
  if ( !sc.isSuccess() )  {
    return sc;
  }
  // Do NOT call base class initialization: we are not writing to file/socket!
  declareInfo("RecvCount",  m_recvReq=0,   "Total number of items received.");
  declareInfo("RecvErrors", m_recvError=0, "Total number of receive errors.");
  declareInfo("RecvBytes",  m_recvBytes=0, "Total number of bytes received from clients.");
  sc = subscribeNetwork();
  if ( !sc.isSuccess() )  {
    return sc;
  }
  err << MSG::DEBUG << "Using partitioned buffers:" << m_partitionBuffer << endmsg;
  m_bm_name = m_buffer;
  if ( m_partitionBuffer )  {
    char txt[32];
    m_bm_name += "_";
    m_bm_name += ::_itoa(m_partitionID,txt,16);
  }
  m_prod = new MBM::Producer(m_bm_name,RTL::processName(),m_partitionID);
  ::wtc_subscribe(WT_FACILITY_DAQ_EVENT,0,rearm_net_request,this);
  return sc;
}

// Finalize the object: release all allocated resources
StatusCode NetworkDataReceiver::finalize()     {
  ::wtc_flush(WT_FACILITY_DAQ_EVENT);
  ::wtc_remove(WT_FACILITY_DAQ_EVENT);
  unsubscribeNetwork(); // Cannot really do anything if this fails...except printing
  for(Receivers::iterator i=m_receivers.begin(); i != m_receivers.end(); ++i)
    deleteNetRequest(*i);
  m_receivers.clear();
  if ( m_prod ) delete m_prod;
  m_prod = 0;
  return OnlineService::finalize();
}

// Handle event data source registration
StatusCode 
NetworkDataReceiver::handleSourceRequest(int clientID,
                                         const std::string& source,
                                         const std::string& svc)  
{
  try {
    if ( 0 == receiver(source) )  {
      m_receivers.push_back(RecvEntry(this, source, svc, clientID));
      RecvEntry& entry = m_receivers.back();
      StatusCode sc = createNetRequest(entry);
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
  catch(std::exception& e)   {
    MsgStream err(msgSvc(), name());
    err << MSG::ERROR << "Exception during data source request" << e.what() << endmsg;
  }
  catch(...)   {
    MsgStream err(msgSvc(), name());
    err << MSG::ERROR << "Unknown exception during data source request." << endmsg;
  }
  return StatusCode::FAILURE;
}

// Producer implementation: Request event from data source
NetworkDataReceiver::RecvEntry* NetworkDataReceiver::receiver(const std::string& nam)  {
  for(Receivers::iterator i=m_receivers.begin(); i!=m_receivers.end();++i)  {
    if ( 0 == strcasecmp(nam.c_str(),(*i).name.c_str()) )  {
      return &(*i);
    }
  }
  return 0;
}

/// Rearm network request for a single event source
StatusCode NetworkDataReceiver::rearmRequest(const RecvEntry& entry)   {
  if ( m_useEventRequests )  {
    return rearmNetRequest(entry);
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
NetworkDataReceiver::handleEventData(const std::string& src,void* buf,size_t len)  {
  try  {
    RecvEntry* entry = receiver(src);
    if ( entry )  {
      MsgStream debug(msgSvc(), name());
      entry->buffer = (char*)buf;
      entry->size = len;
      debug << MSG::DEBUG << "Received event from:" << entry->name 
            << " [" << entry->service << "] Size:" << entry->size << endmsg;
      StatusCode sc = resetNetRequest(*entry);
      ::wtc_insert(WT_FACILITY_DAQ_EVENT,entry);
      return sc;
    }
    error("Event receive entry from unknown source:"+src);
  }
  catch(std::exception& e)   {
    MsgStream err(msgSvc(), name());
    err << MSG::ERROR << "Exception during event receive request" << e.what() << endmsg;
  }
  catch(...)   {
    MsgStream err(msgSvc(), name());
    err << MSG::ERROR << "Unknown exception during event receive request." << endmsg;
  }
  return StatusCode::FAILURE;
}

// Copy event data into buffer manager
StatusCode NetworkDataReceiver::copyEventData(void* to, void* from, size_t len)  {
  ::memcpy(to, from, len);
  return StatusCode::SUCCESS;
}

// Handle event declaration into the buffer manager
StatusCode NetworkDataReceiver::declareEventData(RecvEntry& entry)  {
  try {
    int ret = m_prod->getSpace(entry.size);
    if ( ret == MBM_NORMAL ) {
      MBM::EventDesc& e = m_prod->event();
      RawBank*   b = (RawBank*)e.data;
      MDFHeader* h = (MDFHeader*)b->data();
      e.type       = EVENT_TYPE_EVENT;
      e.len        = entry.size;
      if ( copyEventData(e.data,entry.buffer,entry.size).isSuccess() )  {
        ::memcpy(e.mask,h->subHeader().H1->triggerMask(),sizeof(e.mask));
        ret = m_prod->sendEvent();
        if ( MBM_NORMAL == ret )   {
          return rearmRequest(entry);
        }
      }
      MsgStream error(msgSvc(), name());
      error << MSG::ERROR << "Failed to retrieve network event data from:" << entry.name << endmsg;
      if ( ret != MBM_REQ_CANCEL ) rearmRequest(entry);
      return StatusCode::SUCCESS;
    }
    MsgStream error(msgSvc(), name());
    // Cannot do anything - must handle and rearm new request
    error << MSG::ERROR << "Failed to get space for buffer manager." << endmsg;
    if ( ret != MBM_REQ_CANCEL ) rearmRequest(entry);
    return StatusCode::SUCCESS;
  }
  catch(std::exception& e)  {
    MsgStream error(msgSvc(), name());
    error << MSG::ERROR << "Got exception when declaring event from:" << entry.name << endmsg;
    error << e.what() << endmsg;
  }
  catch(...)  {
    MsgStream error(msgSvc(), name());
    error << MSG::ERROR << "Got unknown exception when declaring event from:" << entry.name << endmsg;
  }
  return StatusCode::FAILURE;
}

// Issue alarm message to error logger
void NetworkDataReceiver::sendAlarm(const std::string& msg)  {
  MsgStream error(msgSvc(), name());
  error << MSG::ERROR << msg << endmsg;
}

// Callback on task dead notification
StatusCode NetworkDataReceiver::taskDead(const std::string& task_name)  {
  MsgStream info(msgSvc(), name());
  info << MSG::INFO << "Dead task:" << task_name << "...." << endmsg;
  for(Receivers::iterator i=m_receivers.begin(); i!=m_receivers.end();++i)  {
    if ( 0 == strcasecmp(task_name.c_str(),(*i).name.c_str()) )  {
      m_receivers.erase(i);
      break;
    }
  }
  sendAlarm("Event provider:"+task_name+" died.");
  return StatusCode::SUCCESS;
}
