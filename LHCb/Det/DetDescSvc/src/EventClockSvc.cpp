// $Id: EventClockSvc.cpp,v 1.3 2006-02-01 19:40:26 marcocle Exp $
// Include files 
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Time.h"

// local
#include "EventClockSvc.h"

// Factory implementation
static SvcFactory<EventClockSvc> s_factory;
const ISvcFactory &EventClockSvcFactory = s_factory;

//-----------------------------------------------------------------------------
// Implementation file for class : EventClockSvc
//
// 2005-07-08 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventClockSvc::EventClockSvc(const std::string& name, ISvcLocator* svcloc):
  Service(name,svcloc),m_incidentSvc(NULL),m_evtDataProvider(NULL),m_detDataSvc(NULL)
{
  // services names
  declareProperty("EventDataSvc",    m_evtDataProviderName = "EventDataSvc");
  declareProperty("DetectorDataSvc", m_detDataSvcName = "DetectorDataSvc");
  // generated times properties
  declareProperty("StartTime",       m_startTime = 0);
  declareProperty("TimeStep",        m_timeStep  = 0);
}
//=============================================================================
// Destructor
//=============================================================================
EventClockSvc::~EventClockSvc() {}; 

//=============================================================================
// IInterface implementation
//=============================================================================
StatusCode EventClockSvc::queryInterface(const InterfaceID& riid, void** ppvUnknown){
  if ( IID_IIncidentListener.versionMatch(riid) ) {
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
  if (m_timeStep == 0) {
    sc = service(m_evtDataProviderName,m_evtDataProvider,true);
    if (!sc.isSuccess()) {
      log << MSG::ERROR << "Unable to get a handle to the event data service" << endmsg;
      return sc;
    } else {
      log << MSG::DEBUG << "Got pointer to IDataProviderSvc \"" << m_evtDataProviderName << '"' << endmsg;
    }
    log << MSG::INFO << "Only fake event times implemented!" << endmsg;
  } else {
    log << MSG::INFO << "Event times generated from " << m_startTime << " with steps of " << m_timeStep << endmsg;
  }

  // Set the first event time at initialization.
  log << MSG::DEBUG << "Initialize event time to " << m_startTime << endmsg;
  m_detDataSvc->setEventTime(Gaudi::Time(m_startTime));

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
	if (m_evtDataProvider != NULL) m_evtDataProvider->release();
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
    if (m_timeStep) {
      m_detDataSvc->setEventTime(Gaudi::Time(m_startTime));
      m_startTime += m_timeStep;
    }// else {
    //      log << MSG::WARNING << "!!!Only fake event times implemented!!!" << endmsg;
    //    }
    log << MSG::DEBUG << "DetDataSvc::eventTime() -> " << m_detDataSvc->eventTime() << endmsg;
  } else {
    log << MSG::WARNING << inc.type() << " incident ignored" << endmsg;
  }
}
//=============================================================================
