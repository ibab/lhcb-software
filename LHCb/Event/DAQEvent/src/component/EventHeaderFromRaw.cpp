// $Id: EventHeaderFromRaw.cpp,v 1.2 2004-10-28 12:46:17 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// Event model
#include "Event/RawEvent.h"
#include "Event/EventHeader.h"

// local
#include "EventHeaderFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventHeaderFromRaw
//
// 2004-10-21 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<EventHeaderFromRaw>          s_factory ;
const        IAlgFactory& EventHeaderFromRawFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventHeaderFromRaw::EventHeaderFromRaw( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
EventHeaderFromRaw::~EventHeaderFromRaw() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode EventHeaderFromRaw::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventHeaderFromRaw::execute() {

  debug() << "==> Execute" << endmsg;

  RawEvent* rawEvt = get<RawEvent>( RawEventLocation::Default );
  const std::vector<RawBank>& data = rawEvt->banks( RawBuffer::Header );

  raw_int* evhData = (*(data.begin())).data();
  long runNum = (*evhData++);
  long evtNum = (*evhData++);
  
  EventHeader* evtHeader = new EventHeader;
  evtHeader->setRunNum( runNum );
  evtHeader->setEvtNum( evtNum );

  return put( evtHeader, EventHeaderLocation::Default );
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EventHeaderFromRaw::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
