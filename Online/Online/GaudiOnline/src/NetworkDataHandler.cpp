// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/NetworkDataHandler.cpp,v 1.6 2007-10-29 14:33:29 frankm Exp $
//  ====================================================================
//  NetworkDataHandler.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiOnline/NetworkDataHandler.h"
#include "GaudiOnline/ISuspendable.h"
#include "GaudiOnline/MEPManager.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/xtoa.h"
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
NetworkDataHandler::NetworkDataHandler(const std::string& nam, ISvcLocator* pSvc)
:  MDFWriter(MDFIO::MDF_BANKS, nam, pSvc),
  m_sendReq(0), m_sendError(0), m_sendBytes(0),
  m_recvReq(0), m_recvError(0), m_recvBytes(0),
  m_mepMgr(0), m_evtSelector(0)
{
  ::wtc_init();
  declareProperty("DataSources",    m_dataSources);
  declareProperty("Buffer",         m_buffer);
  declareProperty("BufferLength",   m_buffLength=128);  // In kBytes
  declareProperty("AsyncRearm",     m_asynchRearm=true);
  lib_rtl_create_lock(0,&m_lock);
}

// Standard Destructor
NetworkDataHandler::~NetworkDataHandler()      {
  lib_rtl_delete_lock(m_lock);
}


int NetworkDataHandler::rearm_net_request(unsigned int,void* param)  {
  RecvEntry* h = (RecvEntry*)param;
  h->handler->rearmRequest(*h);
  return WT_SUCCESS;
}

// Initialize the object: allocate all necessary resources
StatusCode NetworkDataHandler::initialize()   {
  // Do NOT call base class initialization: we are not writing to file/socket!
  m_recipients.clear();
  declareInfo("EventsOut", m_sendReq=0,   "Total number of items sent to receiver(s).");
  declareInfo("ErrorsOut", m_sendError=0, "Total number of send errors to receiver(s).");
  declareInfo("BytesOut",  m_sendBytes=0, "Total number of bytes sent to receiver(s).");
  declareInfo("EventsIn",  m_recvReq=0,   "Total number of items received.");
  declareInfo("ErrorsIn",  m_recvError=0, "Total number of receive errors.");
  declareInfo("BytesIn",   m_recvBytes=0, "Total number of bytes received from clients.");
  StatusCode sc = subscribeNetwork();
  if ( !sc.isSuccess() )  {
    return sc;
  }
  sc = service("MEPManager",m_mepMgr);
  if ( !sc.isSuccess() )  {
    MsgStream info(msgSvc(), name());
    info << MSG::INFO << "Failed to access buffer manager service." << endmsg;
    return sc;
  }  
  sc = isNetEventProducer() ? configureNetProducer() : configureNetConsumer();
  ::wtc_subscribe(WT_FACILITY_DAQ_EVENT,0,rearm_net_request,this);
  return sc;
}

// Finalize the object: release all allocated resources
StatusCode NetworkDataHandler::finalize()     {
  ::wtc_flush(WT_FACILITY_DAQ_EVENT);
  ::wtc_remove(WT_FACILITY_DAQ_EVENT);
  // Do NOT call base class finalization: we are not writing to file/socket!
  m_recipients.clear();
  StatusCode sc = unsubscribeNetwork();
  for(Receivers::iterator i=m_receivers.begin(); i!=m_receivers.end();++i)  {
    deleteNetRequest(*i);
    delete (*i).producer;
    (*i).producer = 0;
  }
#if 0
  undeclareInfo("EventsOut");
  undeclareInfo("ErrorsOut");
  undeclareInfo("BytesOut");
  undeclareInfo("EventsIn");
  undeclareInfo("ErrorsIn");
  undeclareInfo("BytesIn");
#endif
  m_receivers.clear();
  if ( m_evtSelector ) m_evtSelector->release();
  m_evtSelector = 0;
  if ( m_mepMgr ) m_mepMgr->release();
  m_mepMgr = 0;
  return sc;
}

// Request message
const std::string& NetworkDataHandler::requestMessage() const   {
  return s_reqMsg;
}

// Configure access to the buffer manager to declare events
StatusCode NetworkDataHandler::configureNetConsumer()  {
  if ( isNetEventConsumer() )  {
    int cnt = 0;
    for(DataSources::const_iterator i=m_dataSources.begin(); i!=m_dataSources.end();++i,++cnt)  {
      std::string proc = RTL::processName()+"_";
      proc += char('0'+cnt);
      MBM::Producer* prod = m_mepMgr->createProducer(m_buffer,proc);
      m_receivers.push_back(RecvEntry(this,*i,prod));
      RecvEntry& e = m_receivers.back();
      if ( !createNetRequest(e).isSuccess() ) {
        delete e.producer;
        m_receivers.pop_back();
        return StatusCode::FAILURE;
      }
      rearmRequest(e);
    }
    return StatusCode::SUCCESS;
  }
  MsgStream error(msgSvc(),name());
  error << MSG::ERROR << "Bad component configuration: Need data sources defined." << endmsg;
  return StatusCode::FAILURE;
}

// Configure access to the event selector to suspend/resume
StatusCode NetworkDataHandler::configureNetProducer()  {
  StatusCode sc = service("EventSelector",m_evtSelector,true);
  if ( !sc.isSuccess() )  {
    MsgStream error(msgSvc(),name());
    error << MSG::ERROR << "Failed to access ISuspendable interface from event selector." << endmsg;
  }
  return sc;
}

// Incident handler implemenentation: Inform that a new incident has occured
void NetworkDataHandler::handle(const Incident& inc)    {
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "Got incident:" << inc.source() << " of type " << inc.type() << endmsg;
  if ( inc.type() == "DAQ_CANCEL" )  {
  }
  else if ( inc.type() == "DAQ_ENABLE" )  {
  }
}

// Producer implementation: Request event from data source
NetworkDataHandler::RecvEntry* NetworkDataHandler::receiver(const std::string& nam)  {
  for(Receivers::iterator i=m_receivers.begin(); i!=m_receivers.end();++i)  {
    if ( 0 == strcasecmp(nam.c_str(),(*i).name.c_str()) )  {
      return &(*i);
    }
  }
  return 0;
}

// Create network request for a single source
StatusCode NetworkDataHandler::createNetRequest(RecvEntry& /* src */)  {
  return StatusCode::SUCCESS;
}

// Rearm event request
StatusCode NetworkDataHandler::rearmRequest(const RecvEntry& src)  {
  rearmBufferSpace(src);
  return rearmNetRequest(src);
}

// Reset event request
StatusCode NetworkDataHandler::resetNetRequest(const RecvEntry& /* src */)  {
  return StatusCode::SUCCESS;
}

// Rearm event buffer space
StatusCode NetworkDataHandler::rearmBufferSpace(const RecvEntry& src)  {
  if ( src.producer->getSpace(m_buffLength*1024)!=MBM_NORMAL ) {
    // Cannot do anything - must handle later and throw away event data...
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Failed to get space for events from:" << src.name << endmsg;
  }
  return StatusCode::SUCCESS;
}

// Rearm network request for a single source
StatusCode NetworkDataHandler::rearmNetRequest(const RecvEntry& src)  {
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Sending event request to data source:" << src.name << endmsg;
  StatusCode sc = sc = sendData(src,s_reqMsg.c_str(),s_reqMsg.length()+1);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Failed to send request for events from:" << src.name << endmsg;
  }
  return sc;
}

// Rearm network request for a single source
void NetworkDataHandler::deleteNetRequest(RecvEntry& /* src */)  {
}

// Resume event access from MBM
StatusCode NetworkDataHandler::resumeEvents()  {
  if ( !m_evtSelector->resume().isSuccess() )  {
    // This always happens if the event selector is still busy
    // Unclear is how a possible error can be handled. 
    // Hence return success for now...
    MsgStream log(msgSvc(),name());
    log << MSG::WARNING << "Internal error: Cannot resume event selector." << endmsg;
  }
  return StatusCode::SUCCESS;
}

// Resume event access from MBM
StatusCode NetworkDataHandler::suspendEvents()  {
  StatusCode sc = m_evtSelector->suspend();
  if ( !sc.isSuccess() )  {
    MsgStream log(msgSvc(),name());
    log << MSG::WARNING << "Failed to suspend event selector until further notice." << endmsg;
  }
  return sc;
}

// Add request to request queue
StatusCode NetworkDataHandler::addRequest(const Recipient& task)  {
  RTL::Lock lck(m_lock);  // Lock recipient queue to prevent damage
  m_recipients.push_back(task);
  return m_recipients.size() == 1 ? resumeEvents() : StatusCode::SUCCESS;
}

// Producer implementation: Handle client request to receive event over the network
StatusCode NetworkDataHandler::handleEventRequest(long clientID,const std::string& source, size_t /*len */)  {
  char buff[256];
  size_t len = sizeof(buff);
  if ( readEventRequest(buff,&len).isSuccess() )   {
    if ( ::strcasecmp(buff,s_reqMsg.c_str())==0 )  {
      return addRequest(Recipient(this,source,clientID));
    }
    buff[sizeof(buff)-1] = 0;
    MsgStream log(msgSvc(),name());
    log << MSG::WARNING << "Received unknown request:" << buff << endmsg;
  }
  return StatusCode::SUCCESS;
}

// Handle event declaration into the buffer manager
StatusCode NetworkDataHandler::handleEventData(const std::string& source, size_t len)  {
  const RecvEntry* entry = receiver(source);
  if ( entry )  {
    try {
      MBM::Producer* prod = entry->producer;
      MBM::EventDesc& e = prod->event();
      if ( e.data && int(len) <= e.len )  {
        StatusCode sc = readData(e.data,&len);
        if ( sc.isSuccess() )   {
          RawBank*   b = (RawBank*)e.data;
          MDFHeader* h = (MDFHeader*)b->data();
          e.type       = EVENT_TYPE_EVENT;
          e.len        = len;
          ::memcpy(e.mask,h->subHeader().H1->triggerMask(),sizeof(e.mask));
          sc = prod->sendEvent()==MBM_NORMAL ? StatusCode::SUCCESS : StatusCode::FAILURE;
          if ( sc.isSuccess() )   {
            sc = resetNetRequest(*entry);
            if ( m_asynchRearm )  {
              ::wtc_insert(WT_FACILITY_DAQ_EVENT,(void*)entry);
              return sc;
            }
            return rearmRequest(*entry);
          }
          MsgStream log(msgSvc(),name());
          log << MSG::ERROR << "Cannot declare network event from:" << source << " to MBM." << endmsg;
	  return rearmRequest(*entry);
        }
        MsgStream log(msgSvc(),name());
        log << MSG::ERROR << "Failed to retrieve network event data from:" << source << endmsg;
	return rearmRequest(*entry);
      }
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "Not sufficient data space availible to buffer event from:" << source << endmsg;
      return rearmRequest(*entry);
    }
    catch(std::exception& e)  {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "Got exception when declaring event from:" << entry->name << endmsg;
      log << e.what() << endmsg;
    }
    catch(...)  {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "Got unknown exception when declaring event from:" << entry->name << endmsg;
    }
    return rearmRequest(*entry);
  }
  return StatusCode::SUCCESS;
}

// Issue alarm message to error logger
void NetworkDataHandler::sendAlarm(const std::string& msg)  {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << msg << endmsg;
}

// Callback on task dead notification
StatusCode NetworkDataHandler::taskDead(const std::string& task_name)  {
  MsgStream log(msgSvc(),name());
  log << MSG::WARNING << "Dead task:" << task_name << " died...." << endmsg;
  if ( m_dataSources.empty() )    // This is a producer task to supply monitoring clients
    sendAlarm("Monitoring client:"+task_name+" died.");
  else                            // This is a consumer tasks receiving events
    sendAlarm("Event provider:"+task_name+" died.");
  return StatusCode::SUCCESS;
}

// Need to override MDFWriter::execute: implement noop if no clients waiting.
StatusCode NetworkDataHandler::execute()  {
  StatusCode sc = StatusCode::SUCCESS;
  bool pop = false;
  if ( !m_recipients.empty() )  {
    sc = MDFWriter::execute();
    pop = true;
  }
  RTL::Lock lck(m_lock);  // Lock recipient queue to prevent damage
  if ( pop )  {
    m_recipients.pop_front();
  }
  return m_recipients.empty() ? suspendEvents() : sc;
}

// Write MDF record from serialization buffer
StatusCode NetworkDataHandler::writeBuffer(void* const /* ioDesc */, const void* data, size_t len)  {
  // Now send the data ....
  MsgStream log(msgSvc(),name());
  if ( m_recipients.empty() )  {
    log << MSG::ERROR << "Failed to send MDF record [No known recipient]." << endmsg;
    return StatusCode::SUCCESS;
  }
  Recipient recipient(m_recipients.front());
  log << MSG::DEBUG << "Handle event request for recipient:" << recipient.name << endmsg;
  if ( !sendData(recipient, data, len).isSuccess() )   {
    log << MSG::ERROR << "Failed to send MDF to " << recipient.name << "." << endmsg;
    ++m_sendError;
    return StatusCode::FAILURE;
  }
  ++m_sendReq;
  m_sendBytes += len;
  return StatusCode::SUCCESS;
}
