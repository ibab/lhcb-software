// $Id: GaussTrackActionStart.cpp,v 1.1 2004-02-20 19:35:28 ibelyaev Exp $ 
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
#include "G4ProcessType.hh"
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
#include "GaussTrackActionStart.h"
// ============================================================================

/** @file 
 *  Implementation file for class GaussTrackActionStart
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
IMPLEMENT_GiGaFactory( GaussTrackActionStart );
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
GaussTrackActionStart::GaussTrackActionStart
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionBase ( type , name , parent ) 
  //
{};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionStart::~GaussTrackActionStart() {}
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionStart::PreUserTrackingAction  
( const G4Track* /* track */ ) 
{
  // get the trajectory 
  GaussTrajectory* tr = trajectory() ;
  if( 0 == tr   ) 
  { Error ( "Pre...: GaussTrajectory*       points to NULL" ) ; }
  
  // get track information
  GaussTrackInformation* info = trackInfo () ;
  if( 0 == info ) 
  { Error ( "Pre...: GaussTrackInformation* points to NULL" ) ; }
  
};
// ============================================================================

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
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
