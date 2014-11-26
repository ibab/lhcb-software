// $Id: GiGaCheckEventStatus.cpp,v 1.4 2007-11-21 20:55:54 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from GiGa
#include "GiGa/IGiGaSvc.h"

// from Geant4
#include "G4Event.hh"

// local
#include "GiGaCheckEventStatus.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaCheckEventStatus
//
// 2005-04-19 : Gloria Corti
// 2007-01-11 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GiGaCheckEventStatus )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaCheckEventStatus::GiGaCheckEventStatus( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_gigaSvc ( 0 )
{
  declareProperty( "GiGa" , m_gigaSvcName = "GiGa" ) ; 
}

//=============================================================================
// Destructor
//=============================================================================
GiGaCheckEventStatus::~GiGaCheckEventStatus() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GiGaCheckEventStatus::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  if ( 0 == gigaSvc() ) { 
    m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName , true );
    if ( 0 == gigaSvc() ) { 
      return Error( " execute(): IGiGaSvc* points to NULL" );
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GiGaCheckEventStatus::execute() {

  debug() << "==> Execute" << endmsg;

  // Retrieve the G4 event via GiGa
  const G4Event* event = 0;
  StatusCode sc = gigaSvc()->retrieveEvent(event);
  if( !sc.isSuccess() ) {
    return StatusCode::FAILURE;
  }

  // Check if it was processed without problems. 
  // If a problem occured set false in filtering status
  if ( event->IsAborted() ) {
    Error( "Event was aborted by G4" );
    setFilterPassed( false );
    return StatusCode::SUCCESS;
  }
  
  debug() << "Event succesfully processed by G4" << endmsg;
  return StatusCode::SUCCESS;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GiGaCheckEventStatus::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
