// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/NetworkDataSender.cpp,v 1.7 2007-10-29 14:33:29 frankm Exp $
//  ====================================================================
//  NetworkDataSender.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiOnline/NetworkDataSender.h"
#include "GaudiOnline/ISuspendable.h"
#include "GaudiKernel/strcasecmp.h"
#include "RTL/Lock.h"

using namespace LHCb;

// Standard algorithm constructor
NetworkDataSender::NetworkDataSender(const std::string& nam, ISvcLocator* pSvc)
:  MDFWriter(MDFIO::MDF_BANKS, nam, pSvc),
   m_sendReq(0), m_sendError(0), m_sendBytes(0), m_evtSelector(0)
{
  declareProperty("DataSink",         m_target);
  declareProperty("UseEventRequests", m_useEventRequests=false);
  declareProperty("AllowSuspend",     m_allowSuspend=true);
  lib_rtl_create_lock(0,&m_lock);
}

// Standard Destructor
NetworkDataSender::~NetworkDataSender()      {
  lib_rtl_delete_lock(m_lock);
}

// Initialize the object: allocate all necessary resources
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
  try  {
    sc = suspendEvents();
    if ( !sc.isSuccess() )  {
      output << MSG::ERROR << "Failed to suspend event receival from event selector." << endmsg;
      return sc;
    }
    output << MSG::DEBUG << "Register to data sink " << m_target << endmsg;
    sc = subscribeNetwork();
    if ( !sc.isSuccess() )  {
      output << MSG::ERROR << "Failed to register to data sink " << m_target << endmsg;
      return sc;
    }
    if ( !m_useEventRequests )  {
      handleEventRequest(m_recipients.size(),m_target,"EVENT_REQUEST");
    }
    return sc;
  }
  catch(const std::exception& e)  {
    output << MSG::ERROR << "Exception during initialization:" << e.what() << endmsg;
  }
  catch(...)  {
    output << MSG::ERROR << "Unknown exception during initialization:" << endmsg;
  }
  return StatusCode::FAILURE;
}

// Finalize the object: release all allocated resources
StatusCode NetworkDataSender::finalize()     {
  m_recipients.clear();
  // Do NOT call base class finalization: we are not writing to file/socket!
  StatusCode sc = unsubscribeNetwork();
#if 0
  undeclareInfo("EventsOut");
  undeclareInfo("ErrorsOut");
  undeclareInfo("BytesOut");
#endif
  if ( m_evtSelector ) m_evtSelector->release();
  m_evtSelector = 0;
  return sc;
}

// Producer implementation: Resume event access from MBM
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
StatusCode NetworkDataSender::handleEventRequest(long clientID,const std::string& source,const char* buf)  {
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
void NetworkDataSender::sendAlarm(const std::string& msg)  {
  MsgStream output(msgSvc(),name());
  output << MSG::ERROR << msg << endmsg;
}

// Callback on task dead notification
StatusCode NetworkDataSender::taskDead(const std::string& task_name)  {
  MsgStream output(msgSvc(),name());
  output << MSG::WARNING << "Dead task:" << task_name << " died...." << endmsg;
  sendAlarm("Datasink client:"+task_name+" died.");
  return StatusCode::SUCCESS;
}

// Need to override MDFWriter::execute: implement noop if no clients waiting.
StatusCode NetworkDataSender::execute()  {
  if ( !m_recipients.empty() )  {
    StatusCode sc = MDFWriter::execute();
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
    return sc;
  }
  sendAlarm("Spurious request to transfer event to unknown endpoint.");
  return StatusCode::FAILURE;
}

// Write MDF record from serialization buffer
StatusCode NetworkDataSender::writeBuffer(void* const /* ioDesc */, const void* data, size_t len)  {
  // Now send the data ....
  MsgStream output(msgSvc(),name());
  if ( m_recipients.empty() )  {
    output << MSG::ERROR << "Failed to send MDF record [No known recipient]." << endmsg;
    return StatusCode::SUCCESS;
  }
  Recipient& recipient = m_recipients.front();
  output << MSG::DEBUG << "Handle event request for recipient:" << recipient.name << endmsg;
  if ( !sendData(recipient, data, len).isSuccess() )   {
    output << MSG::ERROR << "Failed to send MDF to " << recipient.name << "." << endmsg;
    ++m_sendError;
    return StatusCode::FAILURE;
  }
  ++m_sendReq;
  m_sendBytes += len;
  return StatusCode::SUCCESS;
}
