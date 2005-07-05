// $Id: EventHeaderFromRaw.cpp,v 1.4 2005-07-05 11:54:24 cattanem Exp $
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
// Main execution
//=============================================================================
StatusCode EventHeaderFromRaw::execute() {

  RawEvent* rawEvt = get<RawEvent>( RawEventLocation::Default );
  const std::vector<RawBank>& data = rawEvt->banks( RawBuffer::DAQ );

  if( 0 == data.size() ) return Error( "DAQ bank missing from RawBuffer" );

  raw_int* evhData = (*(data.begin())).data();
  long evtNum = (*evhData++);
  long runNum = (*evhData++);

  EventHeader* evtHeader = new EventHeader;
  evtHeader->setRunNum( runNum );
  evtHeader->setEvtNum( evtNum );

  return put( evtHeader, EventHeaderLocation::Default );
};
//=============================================================================
