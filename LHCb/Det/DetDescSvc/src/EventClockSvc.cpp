// $Id: EventClockSvc.cpp,v 1.10 2009-01-23 13:09:15 cattanem Exp $
// Include files 
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/IEventTimeDecoder.h"

// local
#include "EventClockSvc.h"

DECLARE_SERVICE_FACTORY( EventClockSvc );

//-----------------------------------------------------------------------------
// Implementation file for class : EventClockSvc
//
// 2005-07-08 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventClockSvc::EventClockSvc(const std::string& name, ISvcLocator* svcloc):
  Service(name,svcloc),m_incidentSvc(NULL),m_detDataSvc(NULL),m_toolSvc(NULL),
  m_eventTimeDecoder(NULL)
{
  // service name
  declareProperty("DetectorDataSvc",  m_detDataSvcName       = "DetectorDataSvc" );

  // tool name
  declareProperty("EventTimeDecoder", m_eventTimeDecoderName = "FakeEventTime"   );

  declareProperty("InitialTime",      m_initialTime          = 0                 );
  
}
//=============================================================================
// Destructor
//=============================================================================
EventClockSvc::~EventClockSvc() {}; 

//=============================================================================
// IInterface implementation
//=============================================================================
StatusCode EventClockSvc::queryInterface(const InterfaceID& riid, void** ppvUnknown){
  if ( IIncidentListener::interfaceID().versionMatch(riid) ) {
    *ppvUnknown = (IIncidentListener*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}
//=========================================================================
//  
//=========================================================================
StatusCode EventClockSvc::initialize() {
  // base class initialization
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) return sc;

  // local initialization
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "--- initialize ---" << endmsg;

  // find the detector data svc
  sc = service(m_detDataSvcName,m_detDataSvc,true);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to get a handle to the detector data service" << endmsg;
    return sc;
  } else {
    log << MSG::DEBUG << "Got pointer to IDetDataSvc \"" << m_detDataSvcName << '"' << endmsg;
  }

  // get a pointer to the tool service
  sc = service( "ToolSvc", m_toolSvc, true );
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to get a handle to the tool service" << endmsg;
    return sc;
  } else {
    log << MSG::DEBUG << "Got pointer to ToolSvc " << endmsg;
  }

  sc = m_toolSvc->retrieveTool(m_eventTimeDecoderName, m_eventTimeDecoder, this);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to get a handle to the IEventTimeDecoder \"" << m_eventTimeDecoderName << '"' << endmsg;
    return sc;
  } else {
    log << MSG::DEBUG << "Got pointer to " <<  m_eventTimeDecoderName << endmsg;
  }

  // Set the first event time at initialization.
  Gaudi::Time initTime;
  if (m_initialTime)
    initTime = Gaudi::Time(m_initialTime);
  else
    initTime = Gaudi::Time::current();
  log << MSG::DEBUG << "Initialize event time to " << initTime << endmsg;
  m_detDataSvc->setEventTime(initTime);

  // register to the incident service for BeginEvent incidents
  sc = service("IncidentSvc", m_incidentSvc, false);
  if ( sc.isSuccess() ) {
    m_incidentSvc->addListener(this,IncidentType::BeginEvent);
    log << MSG::DEBUG << "Got pointer to IncidentSvc" << endmsg;
  } else {
    log << MSG::WARNING << "Unable to register to the incident service." << endmsg;
    m_incidentSvc = NULL;
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
StatusCode EventClockSvc::finalize ( ) {
  // local finalization
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "--- finalize ---" << endmsg;

  // release the interfaces used
  if (m_toolSvc         != NULL) {
    if ( m_eventTimeDecoder != NULL )
      m_toolSvc->releaseTool(m_eventTimeDecoder).ignore();
    m_toolSvc->release();
  }
  if (m_detDataSvc      != NULL) m_detDataSvc->release();
  if (m_incidentSvc     != NULL) {
    // unregister from the incident svc
    m_incidentSvc->removeListener(this,IncidentType::BeginEvent);
    m_incidentSvc->release();
  }

  return Service::finalize();
}
//=========================================================================
//  Handle BeginEvent incident
//=========================================================================
void EventClockSvc::handle(const Incident &inc) {
  MsgStream log( msgSvc(), name() );
  if ( inc.type() == IncidentType::BeginEvent ) {
    log << MSG::DEBUG << "New BeginEvent incident received" << endmsg;
    m_detDataSvc->setEventTime(m_eventTimeDecoder->getTime());
    log << MSG::DEBUG << "DetDataSvc::eventTime() -> " << m_detDataSvc->eventTime() << endmsg;
  } else {
    log << MSG::WARNING << inc.type() << " incident ignored" << endmsg;
  }
}
//=============================================================================
