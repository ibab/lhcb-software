// $Id: CreateRawEvent.cpp,v 1.1 2004-06-28 11:41:03 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// From LHCb event model
#include "Event/RawEvent.h"

// local
#include "CreateRawEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CreateRawEvent
//
// 2004-06-28 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CreateRawEvent>          s_factory ;
const        IAlgFactory& CreateRawEventFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CreateRawEvent::CreateRawEvent( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ) {

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CreateRawEvent::execute() {

  // Retrieve the RawBuffer
  RawBuffer* rawBuffer = get<RawBuffer>( RawBufferLocation::Default );

  RawEvent* rawEvent = new RawEvent( *rawBuffer );
  if( NULL == rawEvent ) {
    return Error( "Unable to allocate memory to RawEvent" );
  }
  
  return put( rawEvent, RawEventLocation::Default );
};
