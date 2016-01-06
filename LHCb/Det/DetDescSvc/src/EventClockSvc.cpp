// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Time.h"
#include "GaudiKernel/IEventTimeDecoder.h"

// local
#include "EventClockSvc.h"

DECLARE_SERVICE_FACTORY( EventClockSvc )

//-----------------------------------------------------------------------------
// Implementation file for class : EventClockSvc
//
// 2005-07-08 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventClockSvc::EventClockSvc(const std::string& name, ISvcLocator* svcloc):
  base_class(name,svcloc),m_detDataSvc(NULL),m_toolSvc(NULL),
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
EventClockSvc::~EventClockSvc() = default;

//=========================================================================
//
//=========================================================================
StatusCode EventClockSvc::initialize() {
  // base class initialization
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) return sc;

  // local initialization
  MsgStream log(msgSvc(),name());
  if( log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << "--- initialize ---" << endmsg;

  // find the detector data svc
  sc = service(m_detDataSvcName,m_detDataSvc,true);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to get a handle to the detector data service" << endmsg;
    return sc;
  } else {
    if( log.level() <= MSG::DEBUG )
      log << MSG::DEBUG << "Got pointer to IDetDataSvc \"" << m_detDataSvcName << '"' << endmsg;
  }

  // get a pointer to the tool service
  sc = service( "ToolSvc", m_toolSvc, true );
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to get a handle to the tool service" << endmsg;
    return sc;
  } else {
    if( log.level() <= MSG::DEBUG )
      log << MSG::DEBUG << "Got pointer to ToolSvc " << endmsg;
  }

  sc = m_toolSvc->retrieveTool(m_eventTimeDecoderName, m_eventTimeDecoder, this);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Unable to get a handle to the IEventTimeDecoder \"" << m_eventTimeDecoderName << '"' << endmsg;
    return sc;
  } else {
    if( log.level() <= MSG::DEBUG )
      log << MSG::DEBUG << "Got pointer to " <<  m_eventTimeDecoderName << endmsg;
  }

  // Set the first event time at initialization.
  Gaudi::Time initTime = m_eventTimeDecoder->getTime(); // try to get the actual event time
  if (initTime.ns() == 0) {
    // no event time available yet, use the configuration
    if (m_initialTime)
      initTime = Gaudi::Time(m_initialTime);
    else
      initTime = Gaudi::Time::current();
  }
  if( log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << "Initialize event time to " << initTime << endmsg;
  m_detDataSvc->setEventTime(initTime);

  // register to the incident service for BeginEvent incidents
  m_incidentSvc = service("IncidentSvc", false);
  if ( m_incidentSvc ) {
    m_incidentSvc->addListener(this,IncidentType::BeginEvent);
    if( log.level() <= MSG::DEBUG )
      log << MSG::DEBUG << "Got pointer to IncidentSvc" << endmsg;
  } else {
    log << MSG::WARNING << "Unable to register to the incident service." << endmsg;
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
//
//=========================================================================
StatusCode EventClockSvc::finalize ( ) {
  // local finalization
  MsgStream log(msgSvc(),name());
  if( log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << "--- finalize ---" << endmsg;

  // release the interfaces used
  if (m_toolSvc         != NULL) {
    if ( m_eventTimeDecoder != NULL )
      m_toolSvc->releaseTool(m_eventTimeDecoder).ignore();
    m_toolSvc->release();
  }
  if (m_detDataSvc      != NULL) m_detDataSvc->release();
  if (m_incidentSvc ) {
    // unregister from the incident svc
    m_incidentSvc->removeListener(this,IncidentType::BeginEvent);
  }
  m_incidentSvc.reset();

  return Service::finalize();
}
//=========================================================================
//  Handle BeginEvent incident
//=========================================================================
void EventClockSvc::handle(const Incident &inc) {
  MsgStream log( msgSvc(), name() );
  if ( inc.type() == IncidentType::BeginEvent ) {
    m_detDataSvc->setEventTime(m_eventTimeDecoder->getTime());
    if( log.level() <= MSG::DEBUG ) {
      log << MSG::DEBUG << "New BeginEvent incident received" << endmsg;
      log << MSG::DEBUG << "DetDataSvc::eventTime() -> " << m_detDataSvc->eventTime() << endmsg;
    }
  } else {
    log << MSG::WARNING << inc.type() << " incident ignored" << endmsg;
  }
}
//=============================================================================
