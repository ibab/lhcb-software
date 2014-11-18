// $Id: OscillationFlag.cpp,v 1.2 2004-04-07 15:15:31 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/02/20 19:35:30  ibelyaev
//  major update
//
// ============================================================================
// Include files

// Geant4
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"
#include "G4VUserPrimaryParticleInformation.hh"

// GiGa
#include "GiGa/GiGaUtil.h"
#include "GiGaCnv/GiGaPrimaryParticleInformation.h"
#include "GaussTools/GaussTrajectory.h"

// local
#include "GaussTools/OscillationFlag.h"
// ============================================================================

// ============================================================================
/** @file 
 *  Implementation file for OscillationFlag
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
// ============================================================================

StatusCode OscillationFlag( const G4Track*   track , 
                            GaussTrajectory* traj  )
{
  if ( 0 == track ) { return StatusCode::FAILURE ; }    
  if ( 0 == traj  ) { return StatusCode::FAILURE ; } 
  const G4DynamicParticle* dp = track->GetDynamicParticle() ;
  if ( 0 == dp    ) { return StatusCode::FAILURE ; }    // OK 
  G4PrimaryParticle* pp = dp->GetPrimaryParticle() ;
  if ( 0 == pp    ) { return StatusCode::FAILURE ; }    // OK 
  G4VUserPrimaryParticleInformation* ui = 
    pp -> GetUserInformation() ;
  if ( 0 == ui    ) { return StatusCode::FAILURE ; }    // OK
  //
  GiGaPrimaryParticleInformation * uI = 
    gigaPrimaryParticleInformation( ui ) ;
  if ( 0 == uI    ) { return StatusCode::FAILURE ; }    // OK
  
  // update the trajectory
  traj->setHasOscillated( uI->hasOscillated() ) ;
  if ( uI -> isSignal() ) {
    traj->setIsSignal( uI -> isSignal() ) ;
    traj->setSignalBarcode( uI -> signalBarcode() ) ;
    traj->setHepMCEvent( uI -> pHepMCEvent() ) ;
  }  
  
  return StatusCode::SUCCESS ;
}

// ============================================================================
// The END 
// ============================================================================
