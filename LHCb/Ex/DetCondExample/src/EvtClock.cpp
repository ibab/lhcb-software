// $Id: EvtClock.cpp,v 1.6 2005-06-23 09:33:37 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/TimePoint.h"

// local
#include "EvtClock.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EvtClock
//
// 2005-06-23 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<EvtClock>          s_factory ;
const        IAlgFactory& EvtClockFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EvtClock::EvtClock( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_eventNumber ( 0 )
  , m_detDataSvc  ( 0 )
{
  declareProperty( "startTime",  m_startTime = 2 );
  declareProperty( "delayTime",  m_delayTime = 5 );
  declareProperty( "DetDataSvc", m_detDataSvcName = "DetectorDataSvc" );
}
//=============================================================================
// Destructor
//=============================================================================
EvtClock::~EvtClock() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode EvtClock::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {
    
    m_detDataSvc = svc<IDetDataSvc>(m_detDataSvcName);
    
  } catch (GaudiException) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode EvtClock::execute() {

  debug() << "==> Execute" << endmsg;

  info() << "------------- NEW EVENT! -------------------------------------------" << endmsg;

  // Increment the event counter
  ++m_eventNumber;
  info() << "Event number: " << m_eventNumber << endmsg;

  // Set the event time
  TimePoint time = i_evtTime();
  info() << "Event" << time
         << " (0x" << std::hex << time.absoluteTime() << std::dec << ")"
         << endmsg;
  m_detDataSvc->setEventTime(time);

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EvtClock::finalize() {

  debug() << "==> Finalize" << endmsg;
  info() << "Total #events: " << m_eventNumber << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  
//=========================================================================
TimePoint EvtClock::i_evtTime() {
  return TimePoint(m_startTime + ( m_eventNumber - 1 ) * m_delayTime);
}
//=============================================================================
