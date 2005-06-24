// $Id: CreateRawEvent.cpp,v 1.2 2005-06-24 14:44:08 cattanem Exp $
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

  debug() << "==> Execute" << endmsg;

  if ( !exist<RawEvent>( RawEventLocation::Default ) ) {
    RawBuffer* buf = get<RawBuffer>( RawBufferLocation::Default );
    RawEvent* evt = new RawEvent( *buf );
    put( evt, RawEventLocation::Default );
  }

  return StatusCode::SUCCESS;
};
