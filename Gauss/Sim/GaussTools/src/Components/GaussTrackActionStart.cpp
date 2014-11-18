// $Id: GaussTrackActionStart.cpp,v 1.4 2007-01-12 15:36:44 ranjard Exp $ 
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// Geant4
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"

// GiGa
#include "GiGa/DumpG4Track.h"

// GaussTools 
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"

// local
#include "GaussTrackActionStart.h"


/** @file 
 *  Implementation file for class GaussTrackActionStart
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GaussTrackActionStart )

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
GaussTrackActionStart::GaussTrackActionStart
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionBase ( type , name , parent ) 
  //
  , m_store ( false )
  , m_first ( true  )
{}


// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionStart::~GaussTrackActionStart() {}


// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionStart::PreUserTrackingAction  
( const G4Track* /* track */ ) 
{
  // trick 
  restoreG4setting() ;
  
  // get the trajectory 
  GaussTrajectory* tr = trajectory() ;
  if( 0 == tr   ) 
  { Error ( "Pre...: GaussTrajectory*       points to NULL" ) ; }
  
  // get track information
  GaussTrackInformation* info = trackInfo () ;
  if( 0 == info ) 
  { Error ( "Pre...: GaussTrackInformation* points to NULL" ) ; }

}


// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionStart::PostUserTrackingAction  
( const G4Track* /* track */ ) 
{
  // get the trajectory 
  GaussTrajectory*       tr   = trajectory() ;  
  if( 0 == tr ) { Error ( "Post...: GaussTrajectory* points to NULL " ) ; }
  
  // get track information
  GaussTrackInformation* info = trackInfo () ;
  if( 0 == info ) 
  { Error ( "Post...: GaussTrackInformation* points to NULL" ) ; }

}


// ============================================================================
/** restore G4 initial policy for 
 *  saving of the particle on 
 *  track-by-track basis 
 * 
 *  In particulat it means that 
 *  after running the G4 interactive command
 *   "/tracking/storeTrajectory  1"
 *
 *  becomes disabled after the first invokation 
 *  of this tracking action 
 */ 
// ============================================================================
StatusCode GaussTrackActionStart::restoreG4setting() 
{
  G4TrackingManager* manager = trackMgr() ;
  
  if ( 0 == manager ) 
  { return Error ( "restoreG4Setting(): G4TrackingManager* poits to NULL" ) ; }
  
  if ( !m_first )
  {
    m_first = false ;
    m_store = manager -> GetStoreTrajectory () ;
  }
  else { manager -> SetStoreTrajectory( m_store ) ; }

  manager -> SetStoreTrajectory( true ) ; 
  
  return StatusCode::SUCCESS ;
}



// ============================================================================
// The END 
// ============================================================================
