// $Id: CreateL1Event.cpp,v 1.2 2004-06-23 12:39:12 cattanem Exp $
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
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CreateL1Event::execute() {

  // Retrieve the L1Buffer
  L1Buffer* l1Buffer = get<L1Buffer>( L1BufferLocation::Default );

  L1Event* l1Event = new L1Event( *l1Buffer );

  if( 0 == l1Event ) return Error( "Unable to allocate memory to L1 event" );

  return put( l1Event, L1EventLocation::Default );
};
