// $Id: CreateL1Event.cpp,v 1.1 2003-10-06 16:16:39 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h" 

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
  : Algorithm ( name , pSvcLocator ) {

}

//=============================================================================
// Destructor
//=============================================================================
CreateL1Event::~CreateL1Event() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CreateL1Event::initialize() {

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CreateL1Event::execute() {

  // Retrieve the L1Buffer
  SmartDataPtr<L1Buffer> l1Buffer( eventSvc(), L1BufferLocation::Default );

  if ( 0 == l1Buffer ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to retrieve L1 buffer" << endmsg;
    return StatusCode::FAILURE;
  }

  L1Event* l1Event = new  L1Event( l1Buffer );

  if ( 0 == l1Event ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to allocate memory to L1 event" << endmsg;
    return StatusCode::FAILURE;
  }

  StatusCode sc = eventSvc()->registerObject( L1EventLocation::Default,
                                              l1Event );

  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to register L1Event in TES" << endmsg;
    delete l1Event;
    return sc;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CreateL1Event::finalize() {

  return StatusCode::SUCCESS;
}

//=============================================================================
