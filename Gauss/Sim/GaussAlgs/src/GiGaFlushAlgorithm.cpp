// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"

// from GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGa/DumpG4Event.h"

// local
#include "GiGaFlushAlgorithm.h"

// From Geant4 
class G4Event;

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaFlushAlgorithm
//
// 
// 2002-01-22 : Vanya Belyaev
// 2007-01-11 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GiGaFlushAlgorithm );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaFlushAlgorithm::GiGaFlushAlgorithm( const std::string& Name   ,
                                        ISvcLocator*       SvcLoc )
  : GaudiAlgorithm ( Name , SvcLoc ) 
  , m_gigaSvcName ( "GiGa" ) 
  , m_gigaSvc     ( 0         )
{ 
  declareProperty( "GiGa" , m_gigaSvcName ) ; 
}

//=============================================================================
// Destructor
//=============================================================================
GiGaFlushAlgorithm::~GiGaFlushAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GiGaFlushAlgorithm::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if( sc.isFailure() ) { return sc ; }
  
  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName , true ) ;
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GiGaFlushAlgorithm::execute() 
{  
  if ( 0 == gigaSvc() ) 
  { m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName , true ) ; }
  
  if ( 0 == gigaSvc() ) 
  { return Error ( " execute(): IGiGaSvc* points to NULL" ) ;}
  
  // extract the event ( "flush the GiGa" )
  const G4Event* event = 0 ;
  //*gigaSvc()  >> event     ;
  // equivalent to retrieveEvent(), behind the scene this uses tool 
  // GiGaRunManager to prepareTheEvent() if not prepared, processTheEvent()
  // and then retrieveTheEvent().
  StatusCode sc = gigaSvc()->retrieveEvent(event);
  if( !sc.isSuccess() ) {
    return StatusCode::FAILURE;
  }
  
  if ( msgLevel( MSG::DEBUG ) ) 
  { 
    debug() << " Dump G4 event object " << endmsg ;
    if( info().isActive() ) 
    { GiGaUtil::DumpG4Event ( info().stream() , event  ) ; }
    info() << endmsg ;
  };

  return StatusCode::SUCCESS;
}


//=============================================================================
