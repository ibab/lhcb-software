// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/NetworkDataSender.cpp,v 1.18 2009-05-19 09:40:52 frankb Exp $
//  ====================================================================
//  NetworkDataSender.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiOnline/NetworkDataSender.h"
#include "GaudiOnline/ISuspendable.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/strcasecmp.h"
#include "Event/RawBank.h"
#include "MDF/MDFHeader.h"
#include "RTL/Lock.h"

using namespace LHCb;
using namespace std;

/// Standard algorithm constructor
NetworkDataSender::NetworkDataSender(const string& nam, ISvcLocator* pSvc)
  :  MDFWriter(MDFIO::MDF_BANKS, nam, pSvc),
     m_sendReq(0), m_sendError(0), m_sendBytes(0), m_evtSelector(0),
     m_sendEvents(false), m_incidentSvc(0)
{
  declareProperty("DataSink",         m_target);
  declareProperty("DataSinks",        m_sinks);
  declareProperty("UseEventRequests", m_useEventRequests=false);
  declareProperty("AllowSuspend",     m_allowSuspend=true);
  declareProperty("SendErrorDelay",   m_sendErrorDelay=1000);
  declareProperty("PauseOnError",     m_pauseOnError=true);
  lib_rtl_create_lock(0,&m_lock);
}

/// Standard Destructor
NetworkDataSender::~NetworkDataSender()      {
  lib_rtl_delete_lock(m_lock);
}

/// Initialize the object: allocate all necessary resources
StatusCode NetworkDataSender::initialize()   {
  StatusCode sc;
  MsgStream output(msgSvc(),name());
  // Do NOT call base class initialization: we are not writing to file/socket!
  m_recipients.clear();
  declareInfo("EventsOut", m_sendReq=0,   "Total number of items sent to receiver(s).");
  declareInfo("ErrorsOut", m_sendError=0, "Total number of send errors to receiver(s).");
  declareInfo("BytesOut",  m_sendBytes=0, "Total number of bytes sent to receiver(s).");
  if ( m_allowSuspend ) {
    sc = service("EventSelector",m_evtSelector,true);
    if ( !sc.isSuccess() )  {
      output << MSG::ERROR << "Failed to access ISuspendable interface from event selector." << endmsg;
      return sc;
    }
  }
  if ( m_target.empty() )  {
    output << MSG::INFO << "No data sink specified. "
           << "Clients will have to subscribe themselves." << endmsg;
  }
  sc = service("IncidentSvc",m_incidentSvc,true);
  if ( !sc.isSuccess() )  {
    output << MSG::ERROR << "Failed to access Incident service." << endmsg;
    return sc;
  }
  m_sendEvents = true;
  m_incidentSvc->addListener(this,"DAQ_CANCEL");
  m_incidentSvc->addListener(this,"DAQ_PAUSE");
  try  {
    sc = suspendEvents();
    if ( !sc.isSuccess() )  {
      output << MSG::ERROR << "Failed to suspend event receival from event selector." << endmsg;
      return sc;
    }
    output << MSG::DEBUG << "Register to data sink " << m_target << endmsg;
    sc = subscribeNetwork();
    if ( !sc.isSuccess() )  {
      ::lib_rtl_output(LIB_RTL_OS,"Failed to register to data sink %s.\n",m_target.c_str());
      return sc;
    }
    if ( !m_useEventRequests )  {
      if ( !m_target.empty() )  {
	handleEventRequest(m_recipients.size(),m_target,"EVENT_REQUEST");
      }
      for(auto i=m_sinks.begin(); i!=m_sinks.end(); ++i)  {
	handleEventRequest(m_recipients.size(),*i,"EVENT_REQUEST");
      }
    }
    return sc;
  }
  catch(const exception& e)  {
    output << MSG::ERROR << "Exception during initialization:" << e.what() << endmsg;
  }
  catch(...)  {
    output << MSG::ERROR << "Unknown exception during initialization:" << endmsg;
  }
  return StatusCode::FAILURE;
}

/// Finalize the object: release all allocated resources
StatusCode NetworkDataSender::finalize()     {
  m_sendEvents = false;
  if ( m_incidentSvc ) {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
  }
  m_incidentSvc = 0;
  m_recipients.clear();
  // Do NOT call base class finalization: we are not writing to file/socket!
  StatusCode sc = unsubscribeNetwork();
  IMonitorSvc* mS = monitorSvc().get();
  if ( mS ) mS->undeclareAll(this);
  if ( m_evtSelector ) m_evtSelector->release();
  m_evtSelector = 0;
  return sc;
}

/// Incident handler implemenentation: Inform that a new incident has occured
void LHCb::NetworkDataSender::handle(const Incident& inc)    {
  if ( inc.type() == "DAQ_CANCEL" )  {
    MsgStream info(msgSvc(), name());
    m_sendEvents = false;
    cancelNetwork();
    info << MSG::INFO << "Executing DAQ_CANCEL" << endmsg;
  }
  else if ( inc.type() == "DAQ_PAUSE" )  {
    // Nothing to do for now....
  }  
}

/// Producer implementation: Resume event access from MBM
StatusCode NetworkDataSender::resumeEvents()  {
  if ( m_allowSuspend ) {
    if ( !m_evtSelector->resume().isSuccess() )  {
      // This always happens if the event selector is still busy
      // Unclear is how a possible error can be handled. 
      // Hence return success for now...
      MsgStream output(msgSvc(),name());
      output << MSG::WARNING << "Internal error: Cannot resume event selector." << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

// Producer implementation: Resume event access from MBM
StatusCode NetworkDataSender::suspendEvents()  {
  if ( m_allowSuspend ) {
    StatusCode sc = m_evtSelector->suspend();
    if ( !sc.isSuccess() )  {
      MsgStream log(msgSvc(),name());
      log << MSG::WARNING << "Failed to suspend event selector until further notice." << endmsg;
    }
    return sc;
  }
  return StatusCode::SUCCESS;
}

// Producer implementation: Handle client request to receive event over the network
StatusCode NetworkDataSender::handleEventRequest(long clientID,const string& source,const char* buf)  {
  if ( ::strncasecmp(buf,"EVENT_REQUEST",13)==0 )  {
    return addRequest(Recipient(this,source,clientID));
  }
  MsgStream output(msgSvc(),name());
  output << MSG::WARNING << "Received unknown request:" << buf << " from " << source << endmsg;
  return StatusCode::FAILURE;
}

// Producer implementation: Add request to request queue
StatusCode NetworkDataSender::addRequest(const Recipient& task)  {
  RTL::Lock lck(m_lock);  // Lock recipient queue to prevent damage
  m_recipients.push_back(task);
  return resumeEvents();
}

// Issue alarm message to error logger
void NetworkDataSender::sendAlarm(const string& msg)  {
  MsgStream output(msgSvc(),name());
  output << MSG::ERROR << msg << endmsg;
}

// Callback on task dead notification
StatusCode NetworkDataSender::taskDead(const string& task_name)  {
  //sendAlarm("Datasink client:"+task_name+" died.");
  MsgStream output(msgSvc(),name());
  output << MSG::INFO << "Datasink client:" << task_name << " died." << endmsg;
  return StatusCode::SUCCESS;
}

// Need to override MDFWriter::execute: implement noop if no clients waiting.
StatusCode NetworkDataSender::execute()  {
  if ( !m_recipients.empty() )  {
    StatusCode sc = StatusCode::SUCCESS;
    if ( m_sendEvents ) {
      sc = MDFWriter::execute();
      if ( !sc.isSuccess() )  {
        sendAlarm("Failed to transfer event data to:"+m_target);
      }
      RTL::Lock lck(m_lock);  // Lock recipient queue to prevent damage
      Recipient rcp = m_recipients.front();
      m_recipients.pop_front();
      // Mode without requests: add entry again at end....
      if ( !m_useEventRequests ) {
        m_recipients.push_back(rcp);
        resumeEvents();
      }
      else if ( m_recipients.empty() ) {
        suspendEvents();
      }
    }
    return sc;
  }
  sendAlarm("Spurious request to transfer event to unknown endpoint.");
  return StatusCode::FAILURE;
}

// Write MDF record from serialization buffer
StatusCode NetworkDataSender::writeBuffer(void* const /* ioDesc */, const void* data, size_t len)  {
  // Now send the data ....
  if ( !m_sendEvents ) {
    MsgStream output(msgSvc(),name());
    output << MSG::ERROR << "Ignore request to send event -- got DAQ_CANCEL earlier." << endmsg;
    return StatusCode::SUCCESS;
  }
  if ( m_recipients.empty() )  {
    MsgStream output(msgSvc(),name());
    output << MSG::ERROR << "Failed to send MDF record [No known recipient]." << endmsg;
    return StatusCode::SUCCESS;
  }
  Recipient& recipient = m_recipients.front();
  if ( !sendData(recipient, data, len).isSuccess() )   {
    MsgStream output(msgSvc(),name());
    output << MSG::ERROR << "Failed to send MDF to " << recipient.name << "." << endmsg;
    if ( m_sendEvents )   {
      ::lib_rtl_sleep(m_sendErrorDelay);
      ++m_sendError;
      if ( m_pauseOnError )  {
        m_incidentSvc->fireIncident(Incident(name(),"DAQ_PAUSE"));
      }
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  else {
#if 0
    RawBank*   b = (RawBank*)data;
    MDFHeader* h = (MDFHeader*)b->data();
    MsgStream output(msgSvc(),name());
    output << MSG::ALWAYS << "Handle request for recipient:" << recipient.name  << " Mask:" 
           << hex << setw(10) << left << h->subHeader().H1->triggerMask()[0] << " "
           << hex << setw(10) << left << h->subHeader().H1->triggerMask()[1] << " "
           << hex << setw(10) << left << h->subHeader().H1->triggerMask()[2] << " "
           << hex << setw(10) << left << h->subHeader().H1->triggerMask()[3] << " "
           << endmsg;
#endif
  }
  ++m_sendReq;
  m_sendBytes += len;
  return StatusCode::SUCCESS;
}
