// $Id: GaussTrackActionByEnergyProcess.cpp,v 1.2 2007-01-12 15:36:42 ranjard Exp $ 
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// Geant4
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"

// GaussTools 
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"

// local
// ============================================================================
#include "GaussTrackActionByEnergyProcess.h"
// ============================================================================

/** @file 
 *  Implementation file for class GaussTrackActionByEnergyProcess
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
DECLARE_TOOL_FACTORY( GaussTrackActionByEnergyProcess )


// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussTrackActionByEnergyProcess::GaussTrackActionByEnergyProcess
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionZ   ( type , name , parent ) 
  //
  , m_ownProcs    () 
  , m_childProcs  ()
  , m_ownPTypes   ( 1 , (int) fDecay ) 
  , m_childPTypes () 
  , m_threshold   ( 100 * MeV )
{
  declareProperty ( "OwnProcesses"      , m_ownProcs    ) ;
  declareProperty ( "ChildProcesses"    , m_childProcs  ) ;  
  declareProperty ( "OwnProcessTypes"   , m_ownPTypes   ) ;
  declareProperty ( "ChildProcessTypes" , m_childPTypes ) ;
  declareProperty ( "Threshold"         , m_threshold   ) ;
}


// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionByEnergyProcess::~GaussTrackActionByEnergyProcess() {}


// ============================================================================
/// perform initialization
// ============================================================================
StatusCode GaussTrackActionByEnergyProcess::initialize () 
{
  StatusCode sc = GaussTrackActionZ::initialize() ;
  if( sc.isFailure() ) { return sc ; }
  
  std::sort ( m_ownProcs     .begin () , m_ownProcs    .end () ) ;
  std::sort ( m_childProcs   .begin () , m_childProcs  .end () ) ;
  std::sort ( m_ownPTypes    .begin () , m_ownPTypes   .end () ) ;
  std::sort ( m_childPTypes  .begin () , m_childPTypes .end () ) ;
  
  return StatusCode::SUCCESS ;
}


// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionByEnergyProcess::PreUserTrackingAction 
( const G4Track* track ) 
{
  if ( 0 == track )
  { Error ( "Pre..: G4Track*           points to NULL!" ) ; return ; } // RETURN

  if ( track -> GetKineticEnergy() < m_threshold ) { return ; }
  
  // no action 
  if ( m_ownProcs.empty() && m_ownPTypes.empty()        ) { return ; } // RETURN
  
  if ( 0 == trackMgr() ) 
  { Error ( "Pre..: G4TrackingManager* points to NULL!" ) ; return ; } // RETURN
  
  if      ( track -> GetVertexPosition().z() < zMin()   ) { return ; } // RETURN
  else if ( track -> GetVertexPosition().z() > zMax()   ) { return ; } // RETURN
  
  // get the trajectory 
  GaussTrajectory* tr = trajectory() ;
  
  // check the validity 
  if( 0 == tr ) 
  { Error ( "Pre...: GaussTrajectory*       points to NULL" ) ; return ; }
  
  // check the track information
  GaussTrackInformation*   info = trackInfo() ;
  if( 0 == info ) 
  { Error ( "Pre...: GaussTrackInformation* points to NULL" ) ; return ; }

  if ( info -> toBeStored() ) { return ; }
   
  if ( storeByOwnProcess() ) { mark( info ) ; }

}


// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionByEnergyProcess::PostUserTrackingAction  
( const G4Track* track  ) 
{
  if ( 0 == track )
  { Error ( "Pre..: G4Track*           points to NULL!" ) ; return ; } // RETURN

  if ( track -> GetKineticEnergy() < m_threshold ) { return ; }

  // no action 
  if ( m_childProcs.empty() && m_childPTypes.empty()    ) { return ; } // RETURN
  
  if ( 0 == trackMgr() ) 
  { Error ( "Pos..: G4TrackingManager* points to NULL!" ) ; return ; } // RETURN
  
  if ( 0 == track )
  { Error ( "Pos..: G4Track*           points to NULL!" ) ; return ; } // RETURN

  if ( track -> GetKineticEnergy() < m_threshold ) { return ; }
   
  if      ( track -> GetVertexPosition().z() < zMin()   ) { return ; } // RETURN
  else if ( track -> GetVertexPosition().z() > zMax()   ) { return ; } // RETURN
  
  // get the trajectory 
  GaussTrajectory* tr = trajectory() ;
  
  // check the validity 
  if( 0 == tr ) 
  { Error ( "Pos..: GaussTrajectory*       points to NULL" ) ; return ; }
  
  // check the track information
  GaussTrackInformation*   info = trackInfo() ;
  if( 0 == info ) 
  { Error ( "Pos..: GaussTrackInformation* points to NULL" ) ; return ; }

  if ( info -> toBeStored() ) { return ; }
 
  if ( storeByChildProcess() ) { mark( info ) ; }
  
}



// ============================================================================
// The END 
// ============================================================================
