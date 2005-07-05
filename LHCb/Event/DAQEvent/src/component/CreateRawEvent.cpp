// $Id: CreateRawEvent.cpp,v 1.3 2005-07-05 11:54:24 cattanem Exp $
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

  if ( !exist<RawEvent>( RawEventLocation::Default ) ) {
    RawBuffer* buf = get<RawBuffer>( RawBufferLocation::Default );
    debug() << "Raw buffer size: " << buf->currentSize() << endmsg;
    RawEvent* evt = new RawEvent( *buf );
    put( evt, RawEventLocation::Default );
  }

  return StatusCode::SUCCESS;
};
