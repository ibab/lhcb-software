//$Id: EvtClock.cpp,v 1.2 2001-11-27 18:32:06 andreav Exp $
#include <stdio.h>

#include "EvtClock.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/TimePoint.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<EvtClock> Factory;
const IAlgFactory& EvtClockFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
EvtClock::EvtClock( const std::string&  name, 
		    ISvcLocator*        pSvcLocator )
  : Algorithm     ( name, pSvcLocator )
  , m_eventNumber ( 0 )
  , m_detDataSvc  ( 0 )
{
  declareProperty( "startTime",  m_startTime = 2 );
  declareProperty( "delayTime",  m_delayTime = 5 );
}

//----------------------------------------------------------------------------

/// Initialization of the algorithm. 

StatusCode EvtClock::initialize() {

  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initialize()" << endreq;

  // Query the IDetDataSvc interface of the detector data service
  sc = detSvc()->queryInterface(IID_IDetDataSvc, 
				(void**) &m_detDataSvc);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Could not query IDetDataSvc interface of DetectorDataSvc" 
	<< endreq;
    return sc;
  } else {
    log << MSG::DEBUG 
	<< "Retrieved IDetDataSvc interface of DetectorDataSvc" 
	<< endreq;
  }

  // Get properties from the JobOptionsSvc
  sc = setProperties();
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not set jobOptions properties" << endreq;
    return sc;
  }
  log << MSG::DEBUG << "Properties were read from jobOptions" << endreq;
  log << MSG::DEBUG << "Time of first event: " << m_startTime << endreq;
  log << MSG::DEBUG << "Time between events: " << m_delayTime << endreq;

  log << MSG::INFO << "Initialization completed" << endreq;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

StatusCode EvtClock::execute( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO 
      << "------------- NEW EVENT! -------------------------------------------"
      << endreq;
  log << MSG::INFO << "Execute()" << endreq;

  // Increment the event counter
  m_eventNumber++;
  log << MSG::INFO << "Event number: " << m_eventNumber << endreq;

  // Set the event time
  long time = i_evtTime();
  log << MSG::INFO << "Event time: " << time << endreq;
  m_detDataSvc->setEventTime( TimePoint(time) );

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

StatusCode EvtClock::finalize( ) {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalize()" << endreq;
  log << MSG::INFO << "Total #events: " << m_eventNumber << endreq;
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

longlong EvtClock::i_evtTime( ) {

  return m_startTime + ( m_eventNumber - 1 ) * m_delayTime;

}

//----------------------------------------------------------------------------

