// ============================================================================
#define GIGA_GaussPreTrackAction_CPP 1 
// ============================================================================
/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
/// Geant4 
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
/// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
// GaussTools
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
/// local
#include "GaussPreTrackAction.h"
///

/** @file 
 *  
 *  Implementation of class GaussPreTrackAction
 *
 *  @author Vanya Belyaev
 *  @author Witek Pokorski
 */

// ============================================================================
/// factory business 
// ============================================================================
DECLARE_TOOL_FACTORY( GaussPreTrackAction )


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
GaussPreTrackAction::GaussPreTrackAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent ) {}


// ============================================================================
/// destructor 
// ============================================================================
GaussPreTrackAction::~GaussPreTrackAction() {}


// ============================================================================
/** initialize the track action  
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GaussPreTrackAction::initialize () 
{
  // initialize the base 
  StatusCode status = GiGaTrackActionBase::initialize() ; 
  if( status.isFailure() ) 
    { return Error("Could not intialize base class GiGaTrackActionBase!", 
                   status ) ; }
  
  return Print("Initialized successfully" , 
               StatusCode::SUCCESS        , MSG::VERBOSE);
}


// ============================================================================
/** finalize the action object 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code
 */ 
// ============================================================================
StatusCode GaussPreTrackAction::finalize   () 
{ 
  // finalize the base 
  return GiGaTrackActionBase::finalize() ;
}


// ============================================================================
/**  perform action 
 *   @param pointer to new track opbject 
 */
// ============================================================================
void GaussPreTrackAction::PreUserTrackingAction  (const G4Track* track ) 
{
  // Is the track valid? Is tracking manager valid? 
  // Does trajectory already exist?
  if( 0 == track || 0 == trackMgr() || 0 != trackMgr()->GimmeTrajectory()  ) 
    { return ; }  
  
  //  
  trackMgr()->SetStoreTrajectory( true ) ;  
  //
  
  // create GaussTrajectory and inform Tracking Manager 
  GaussTrajectory* traj = new GaussTrajectory( track ) ; 
  trackMgr()->SetTrajectory( traj ) ;  
  //
}












