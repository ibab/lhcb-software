// $Id: CreateL1Event.cpp,v 1.2 2005-06-24 14:44:08 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// From LHCb event model
#include "Event/L1Event.h"

// local
#include "CreateL1Event.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CreateL1Event
//
// 2003-10-03 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CreateL1Event>          s_factory ;
const        IAlgFactory& CreateL1EventFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CreateL1Event::CreateL1Event( const std::string& name,
                              ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ) {

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CreateL1Event::execute() {

  debug() << "==> Execute" << endmsg;

  if ( !exist<L1Event>( L1EventLocation::Default ) ) {
    L1Buffer* buf = get<L1Buffer>( L1BufferLocation::Default );
    L1Event* evt = new L1Event( *buf );
    put( evt, L1EventLocation::Default );
  }

  return StatusCode::SUCCESS;
};
