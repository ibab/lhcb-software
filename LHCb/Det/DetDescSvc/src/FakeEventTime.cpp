// Include files

// from Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"

// local
#include "FakeEventTime.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FakeEventTime
//
// 2006-09-21 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FakeEventTime )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FakeEventTime::FakeEventTime( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : AlgTool ( type, name , parent )
{
  declareInterface<IEventTimeDecoder>(this);

  // properties used to generate fake times
  declareProperty("StartTime",       m_startTime = 0);
  declareProperty("TimeStep",        m_timeStep  = 0);
}
//=============================================================================
// Destructor
//=============================================================================
FakeEventTime::~FakeEventTime() {}

//=========================================================================
//  Initialization
//=========================================================================
StatusCode FakeEventTime::initialize ( ) {
  // base class initialization
	StatusCode sc = AlgTool::initialize();
	if (!sc.isSuccess()) return sc;

  // local initialization
	MsgStream log(msgSvc(),name());
  if( log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << "--- initialize ---" << endmsg;

  log << MSG::INFO << "Event times generated from " << m_startTime << " with steps of " << m_timeStep << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Finalization
//=========================================================================
StatusCode FakeEventTime::finalize ( ) {
	// local finalization
	MsgStream log(msgSvc(),name());
  if( log.level() <= MSG::DEBUG )
    log << MSG::DEBUG << "--- finalize ---" << endmsg;

  return AlgTool::finalize();
}


//=========================================================================
//  Return the time of current event
//=========================================================================
Gaudi::Time FakeEventTime::getTime ( ) const {

  // As a real EventTimeDecoder, the event time can be retrieved only in RUNNING
  // state, i.e. when the event can be loaded.
  if (FSMState() != Gaudi::StateMachine::RUNNING)
    return Gaudi::Time::epoch();

  // Here we should get the time from the EventDataSvc
  Gaudi::Time currentTime(m_startTime);

 // increment for the next event
  FakeEventTime *myPtr = const_cast<FakeEventTime *>(this);
  myPtr->m_startTime += m_timeStep;

  return currentTime;
}
//=============================================================================
