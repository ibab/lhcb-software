// $Id: GaussTrackActionByEnergy.cpp,v 1.1 2004-02-20 19:35:26 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// CLHEP
// ============================================================================
#include "CLHEP/Units/SystemOfUnits.h"
// ============================================================================
// Geant4
// ============================================================================
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"
// ============================================================================
// GiGa
// ============================================================================
#include "GiGa/GiGaMACROs.h"
// ============================================================================
// GaussTools 
// ============================================================================
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
// ============================================================================
// local
// ============================================================================
#include "GaussTrackActionByEnergy.h"
// ============================================================================

/** @file 
 *  Implementation file for class GaussTrackActionByEnergy
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
IMPLEMENT_GiGaFactory( GaussTrackActionByEnergy );
// ============================================================================

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
GaussTrackActionByEnergy::GaussTrackActionByEnergy
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionZ   ( type , name , parent ) 
  //
  , m_storeByEnergy ( true    )  
  , m_threshold     ( 1 * GeV )  
{
  declareProperty ( "StoreEnergy" , m_storeByEnergy ) ;
  declareProperty ( "Threshold"   , m_threshold     ) ;
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionByEnergy::~GaussTrackActionByEnergy() {}
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionByEnergy::PreUserTrackingAction  ( const G4Track* track ) 
{
  // no action 
  if ( !storeByEnergy()                                 ) { return ; } // RETURN
  
  if ( 0 == trackMgr() ) 
  { Error ( "Pre..: G4TrackingManager* points to NULL!" ) ; return ; } // RETURN
  
  if ( 0 == track )
  { Error ( "Pre..: G4Track*           points to NULL!" ) ; return ; } // RETURN
  
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
  
  bool store = false ;
  
  if      ( trackMgr() ->GetStoreTrajectory()        ) { store = true ; }
  else if ( storeByEnergy() && 
            threshold() <= track->GetKineticEnergy() ) { store = true ; }
  
  // update the global flag 
  if ( store ) { trackMgr() -> SetStoreTrajectory( true ) ; }
  
  // update track info 
  if ( trackMgr() ->GetStoreTrajectory() ) { info->setToBeStored( true ) ; }
  
};
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionByEnergy::PostUserTrackingAction  
( const G4Track* /* track */ ) {} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
