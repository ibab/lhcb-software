// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/NetworkDataHandler.cpp,v 1.2 2006-12-13 14:10:13 frankb Exp $
//	====================================================================
//  NetworkDataHandler.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiOnline/NetworkDataHandler.h"
#include "GaudiOnline/ISuspendable.h"
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
#include <algorithm>
#include <cmath>

using namespace LHCb;
static const std::string s_reqMsg("EVENT_REQUEST");

// Standard algorithm constructor
NetworkDataHandler::NetworkDataHandler(const std::string& nam, ISvcLocator* pSvc)
:	MDFWriter(MDFIO::MDF_BANKS, nam, pSvc), m_evtSelector(0), m_prod(0),
  m_sendReq(0), m_sendError(0), m_sendBytes(0),
  m_recvReq(0), m_recvError(0), m_recvBytes(0),
  m_logger(0)
{
  m_procName = RTL::processName();
  declareProperty("PrintFreq",      m_freq = 0.);
  declareProperty("DataSources",    m_dataSources);
  declareProperty("PartitionID",    m_partitionID);
  declareProperty("Buffer",         m_buffer);
  declareProperty("PartitionBuffer",m_partitionBuffer);
  lib_rtl_create_lock(0,&m_lock);
}

// Standard Destructor
NetworkDataHandler::~NetworkDataHandler()      {
  lib_rtl_delete_lock(m_lock);
}

// Initialize the object: allocate all necessary resources
StatusCode NetworkDataHandler::initialize()   {
  // Do NOT call base class initialization: we are not writing to file/socket!
  m_recipients.clear();
  m_logger = new MsgStream(msgSvc(),name());
  declareInfo("SendCount",  m_sendReq=0,   "Total number of items sent to receiver(s).");
  declareInfo("SendErrors", m_sendError=0, "Total number of send errors to receiver(s).");
  declareInfo("SendBytes",  m_sendBytes=0, "Total number of bytes sent to receiver(s).");
  declareInfo("RecvCount",  m_recvReq=0,   "Total number of items received.");
  declareInfo("RecvErrors", m_recvError=0, "Total number of receive errors.");
  declareInfo("RecvBytes",  m_recvBytes=0, "Total number of bytes received from clients.");
  m_net_handler = &NetworkDataHandler::handleEventRequest;
  StatusCode sc = subscribeNetwork();
  if ( !sc.isSuccess() )  {
    return sc;
  }
  sc = m_dataSources.empty() ? configureSelector() : configureBuffer();
  return sc;
}

// Finalize the object: release all allocated resources
StatusCode NetworkDataHandler::finalize()     {
  // Do NOT call base class finalization: we are not writing to file/socket!
  m_recipients.clear();
  StatusCode sc = unsubscribeNetwork();
  if ( sc.isSuccess() )  {
    if ( !m_dataSources.empty() )  {
      if ( m_prod ) delete m_prod;
      m_prod = 0;
    }
  }
  if ( m_logger ) delete m_logger;
  m_logger = 0;
  if ( m_evtSelector ) m_evtSelector->release();
  m_evtSelector = 0;
  return sc;
}

// Configure access to the buffer manager to declare events
StatusCode NetworkDataHandler::configureBuffer()  {
  m_bm_name = m_buffer;
  if ( m_partitionBuffer )  {
    char txt[32];
    m_bm_name += "_";
    m_bm_name += ::_itoa(m_partitionID,txt,16);
  }
  m_prod = new MBM::Producer(m_bm_name,m_procName,m_partitionID);
  m_net_handler = &NetworkDataHandler::handleEventData;
  return requestEvents();
}

// Configure access to the event selector to suspend/resume
StatusCode NetworkDataHandler::configureSelector()  {
  StatusCode sc = service("EventSelector",m_evtSelector,true);
  if ( !sc.isSuccess() )  {
    error() << "Failed to access ISuspendable interface from event selector." << endmsg;
  }
  return sc;
}

// Incident handler implemenentation: Inform that a new incident has occured
void NetworkDataHandler::handle(const Incident& inc)    {
  info() << "Got incident:" << inc.source() << " of type " << inc.type() << endmsg;
  if ( inc.type() == "DAQ_CANCEL" )  {
  }
  else if ( inc.type() == "DAQ_ENABLE" )  {
  }
}

/// Rearm network request for a single source
StatusCode NetworkDataHandler::rearmNetRequest(const std::string& src)  {
  debug() << "Sending event request to data source:" << src << endmsg;
  StatusCode sc = sendData(src,s_reqMsg.c_str(),s_reqMsg.length()+1);
  if ( !sc.isSuccess() ) {
    error() << "Failed to send request for events to:" << src << endmsg;
  }
  return sc;
}

// Producer implementation: Request event from data source
StatusCode NetworkDataHandler::requestEvents()  {
  if ( !m_dataSources.empty() )  {
    StatusCode status = StatusCode::SUCCESS;
    for(DataSources::const_iterator i=m_dataSources.begin(); i!=m_dataSources.end();++i)  {
      StatusCode sc = rearmNetRequest(*i);
      if ( !sc.isSuccess() ) {
        status = sc;
      }
    }
    return status;
  }
  error() << "Bad component configuration: Need data sources defined." << endmsg;
  return StatusCode::FAILURE;
}

// Producer implementation: Handle client request to receive event over the network
StatusCode NetworkDataHandler::handleEventRequest(size_t len)  {
  char buff[256], task[256];
  len = std::min(sizeof(buff),len);
  if ( readData(buff,&len,task).isSuccess() )   {
    if ( ::strcasecmp(buff,s_reqMsg.c_str())==0 )  {
      MsgStream log(msgSvc(),name());
      RTL::Lock lck(m_lock);  // Lock recipient queue to prevent damage
      log << MSG::DEBUG << "Received event request from target:" << task << endmsg;
      m_recipients.push_back(task);
      if ( m_recipients.size() == 1 )  {
        if ( !m_evtSelector->resume().isSuccess() )  {
          // This always happens if the event selector is still busy
          log << MSG::WARNING << "Internal error: Cannot resume event selector." << endmsg;
        }
      }
      return StatusCode::SUCCESS;
    }
    buff[sizeof(buff)-1] = 0;
    MsgStream log(msgSvc(),name());
    log << MSG::WARNING << "Received unknown request:" << buff << endmsg;
  }
  return StatusCode::SUCCESS;
}

// Handle event declaration into the buffer manager
StatusCode NetworkDataHandler::handleEventData(size_t len)  {
  if ( m_prod->getSpace(len) == MBM_NORMAL ) {
    char source[128];
    StatusCode sc = StatusCode::SUCCESS;
    MBM::EventDesc& e = m_prod->event();
    if ( (sc=readData(e.data,&len,source)).isSuccess() )   {
      RawBank*   b = (RawBank*)e.data;
      MDFHeader* h = (MDFHeader*)b->data();
      e.type       = EVENT_TYPE_EVENT;
      e.len        = len;
      ::memcpy(e.mask,h->subHeader().H1->triggerMask(),sizeof(e.mask));
      sc = m_prod->sendEvent() == MBM_NORMAL ? StatusCode::SUCCESS : StatusCode::FAILURE;
      if ( sc.isSuccess() )   {
        return rearmNetRequest(source);
      }
      error() << "Cannot declare network event to MBM." << endmsg;
      return sc;
    }
    error() << "Failed to retrieve network event data." << endmsg;
    return sc;
  }
  error() << "Failed to retrieve " << len << " bytes of space." << endmsg;
  return StatusCode::SUCCESS;
}

// Issue alarm message to error logger
void NetworkDataHandler::sendAlarm(const std::string& msg)  {
  error() << msg << endmsg;
}

// Callback on task dead notification
StatusCode NetworkDataHandler::taskDead(const std::string& task_name)  {
  warning() << "Dead task:" << task_name << " died...." << endmsg;
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
  if ( m_recipients.empty() )   {
    sc = m_evtSelector->suspend();
    if ( !sc.isSuccess() )  {
      warning() << "Failed to suspend event selector until further notice." << endmsg;
    }
  }
  return sc;
}

// Write MDF record from serialization buffer
StatusCode NetworkDataHandler::writeBuffer(void* const /* ioDesc */, const void* data, size_t len)  {
  longlong prtCount = fabs(m_freq) > 1./LONGLONG_MAX ? longlong(1./m_freq) : LONGLONG_MAX;
  // Now send the data ....
  if ( m_recipients.empty() )  {
    error() << "Failed to send MDF record [No known recipient]." << endmsg;
    return StatusCode::SUCCESS;
  }
  std::string recipient = m_recipients.front();
  debug() << "Handle event request for recipient:" << recipient << endmsg;
  if ( !sendData(recipient, data, len).isSuccess() )   {
    error() << "Failed to send MDF to " << recipient << "." << endmsg;
    ++m_sendError;
    return StatusCode::FAILURE;
  }
  ++m_sendReq;
  m_sendBytes += len;
  if ( 0 == (m_sendReq%prtCount) )  {
    info() << "Sent " << m_sendReq << " Events with " << m_sendBytes << " Bytes." << endmsg;
  }
  return StatusCode::SUCCESS;
}
