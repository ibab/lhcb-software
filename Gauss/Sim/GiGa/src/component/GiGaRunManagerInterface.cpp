// $Id: GiGaRunManagerInterface.cpp,v 1.8 2004-02-20 18:13:35 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2003/10/17 13:47:49  ranjard
// v14r6 - fix the code for geant4.5.2.ref04
//
// Revision 1.6  2003/09/22 13:57:11  ibelyaev
//  polishing of addRef/release/releaseTools/finalize
//
// Revision 1.5  2003/04/06 18:49:49  ibelyaev
//  see $GIGAROOT/doc/release.notes
//
// ============================================================================
/// GiGa 
#include "GiGa/GiGaUtil.h"
#include "GiGa/IGiGaGeoSrc.h"
#include "GiGa/IGiGaTool.h"
// Local
#include "GiGaRunManager.h"
/// G4 
#include "G4StateManager.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserStackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserTrackingAction.hh"
#include "G4UIsession.hh"

// ============================================================================
/** @file
 *  Implementation of public interface for class  GiGaRunManager
 *
 *  @date   31/07/2001 
 *  @author Vanya Belyaev
 */

// ============================================================================
/** initialize the run manager 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaRunManager::initialize () 
{
  /// initialise the base class GiGaBase
  StatusCode sc = GiGaBase::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class " , sc ) ; }
  ///
  G4RunManager::SetVerboseLevel( m_verbosity );
  G4ParticleTable::GetParticleTable()->SetVerboseLevel( 0 ) ;
  ///
  for( Names::const_iterator iname = 
         m_runToolsList.begin() ; m_runToolsList.end() != iname ; ++iname )
  {
    IGiGaTool* runTool = tool<IGiGaTool>( *iname , this );
    if( 0 == runTool ) { return StatusCode::FAILURE ; }
    m_runTools.push_back( runTool );
  }
  ///
  return Print("GiGaRunManager initialized successfully" , 
               StatusCode::SUCCESS                       , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalize the run manager 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaRunManager::finalize   () 
{
  Print("GiGaRunManager Finalization");
  StatusCode sc = finalizeRunManager();
  if( sc.isFailure() ) 
  { Error ( "could not finalize RunManager base " , sc ) ; }
  // release  "geometry source"
  if( 0 != m_geoSrc ) { m_geoSrc -> release() ; m_geoSrc = 0 ; }
  // clear run tools 
  m_runTools.clear();
  // 
  return GiGaBase::finalize();
};
// ============================================================================

// ============================================================================
/** declare the Geant4 Primary Generator Action 
 *  @param obj pointer  to Geant4 Primary Generator Action 
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4VUserPrimaryGeneratorAction  * obj )  
{ 
  ///
  if( 0 != G4RunManager::GetUserPrimaryGeneratorAction() ) 
    {
      ///
      G4VUserPrimaryGeneratorAction* prev = 
        G4RunManager::userPrimaryGeneratorAction;
      Warning("Existing "          + GiGaUtil::ObjTypeName( prev ) +
              " is replaced with " + GiGaUtil::ObjTypeName( obj  ) );
      if( 0 != prev ) { delete prev ; }
      ///
      G4RunManager::userPrimaryGeneratorAction = 0 ; 
    } 
  ///
  G4RunManager::SetUserAction( obj );  
  ///
  set_krn_Is_Initialized ( false ) ; 
  set_run_Is_Initialized ( false ) ; 
  set_evt_Is_Prepared    ( false ) ; 
  set_evt_Is_Processed   ( false ) ; 
  ///
  if( 0 != G4StateManager::GetStateManager() ) 
    { G4StateManager::GetStateManager()->SetNewState(G4State_PreInit); } 
  ///
  return StatusCode::SUCCESS ; 
};

// ============================================================================
/** declare the Geant4 Detector Construction Action
 *  @param obj pointer  to Geant4 Detector Construction Action  
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4VUserDetectorConstruction    * obj ) 
{
  /// geometry tree exists, could not use Detector Construction 
  if( 0 != m_rootGeo )
    { return Error("Could not use DetectorConstruction!"); }
  /// 
  if( 0 != G4RunManager::GetUserDetectorConstruction() ) 
    {
      G4VUserDetectorConstruction* prev = 
        G4RunManager::userDetector ;
      Warning("Existing "          + GiGaUtil::ObjTypeName( prev ) + 
              " is replaced with " + GiGaUtil::ObjTypeName( obj  ) );
      if( 0 != prev ) { delete prev ; }
      ///
    }  
  ///
  G4RunManager::geometryInitialized = false ; 
  G4RunManager::SetUserInitialization( obj ); 
  ///
  set_krn_Is_Initialized ( false ) ; 
  set_run_Is_Initialized ( false ) ; 
  set_evt_Is_Prepared    ( false ) ; 
  set_evt_Is_Processed   ( false ) ; 
  ///
  if( 0 != G4StateManager::GetStateManager() ) 
    { G4StateManager::GetStateManager()->SetNewState(G4State_PreInit); } 
  ///
  return StatusCode::SUCCESS ; 
};
// ============================================================================

// ============================================================================
/** declare the top level ("world") physical volume 
 *  @param obj pointer  to top level ("world") physical volume  
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4VPhysicalVolume              * obj ) 
{
  ///
  if( 0 != m_rootGeo ) 
    { Error("Attention!The existing geometry tree is replaced!!!"); }
  ///
  m_rootGeo = obj; 
  ///
  G4RunManager::geometryInitialized = false ; 
  ///
  set_krn_Is_Initialized ( false ) ; 
  set_run_Is_Initialized ( false ) ; 
  set_evt_Is_Prepared    ( false ) ; 
  set_evt_Is_Processed   ( false ) ; 
  ///
  if( 0 != G4StateManager::GetStateManager() ) 
    { G4StateManager::GetStateManager()->SetNewState(G4State_PreInit); } 
  ///
  return StatusCode::SUCCESS ; 
};
// ============================================================================

// ============================================================================
/** declare the Geant4 Physics List 
 *  @param obj pointer  to Geant4 Physics List  
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4VUserPhysicsList             * obj )
{
  ///
  if( 0 != G4RunManager::GetUserPhysicsList() ) 
    {
      G4VUserPhysicsList* prev = 
        G4RunManager::physicsList ;
      Warning("Existing "          + GiGaUtil::ObjTypeName( prev ) + 
              " is replaced with " + GiGaUtil::ObjTypeName( obj  ) );
      if( 0 != prev ) { delete prev ; }
      ///
    }  
  ///
  G4RunManager::physicsInitialized = false ; 
  //flr  G4RunManager::cutoffInitialized  = false ; 
  G4RunManager::SetUserInitialization( obj ); 
  ///
  set_krn_Is_Initialized ( false ) ; 
  set_run_Is_Initialized ( false ) ; 
  set_evt_Is_Prepared    ( false ) ; 
  set_evt_Is_Processed   ( false ) ; 
  ///
  if( 0 != G4StateManager::GetStateManager() ) 
    { G4StateManager::GetStateManager()->SetNewState(G4State_PreInit); } 
  ///
  return StatusCode::SUCCESS ; 
};
// ============================================================================

// ============================================================================
/** declare the GiGa Geometry source 
 *  @param obj pointer  to GiGaGeometry source  
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( IGiGaGeoSrc                    * obj )
{
  ///
  if( 0 != geoSrc() ) 
    {
      Warning("Existing "          + GiGaUtil::ObjTypeName( geoSrc() ) +
              " is replaced with " + GiGaUtil::ObjTypeName( obj      ) );
      if( 0 != geoSrc() ) { geoSrc()->release() ; m_geoSrc = 0 ; }
    }  
  ///
  G4RunManager::geometryInitialized = false ; 
  ///
  m_geoSrc = obj;
  if( 0 != geoSrc() ) { geoSrc()->addRef(); }
  ///
  set_krn_Is_Initialized ( false ) ; 
  set_run_Is_Initialized ( false ) ; 
  set_evt_Is_Prepared    ( false ) ; 
  set_evt_Is_Processed   ( false ) ; 
  ///
  if( 0 != G4StateManager::GetStateManager() ) 
    { G4StateManager::GetStateManager()->SetNewState(G4State_PreInit); } 
  ///
  return StatusCode::SUCCESS ; 
};
// ============================================================================

// ============================================================================
/** declare the Geant4 Run Action 
 *  @param obj pointer  to Geant4 Run action  
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4UserRunAction                * obj )
{
  ///
  if( 0 != G4RunManager::GetUserRunAction() ) 
    {
      G4UserRunAction* prev = 
        G4RunManager::userRunAction ;
      Warning("Existing "          + GiGaUtil::ObjTypeName( prev ) + 
              " is replaced with " + GiGaUtil::ObjTypeName( obj  ) );
      if( 0 != prev ) { delete prev ; }
      ///
    }  
  ///
  G4RunManager::SetUserAction( obj ); 
  return StatusCode::SUCCESS ; 
};
// ============================================================================

// ============================================================================
/** declare the Geant4 Event Action 
 *  @param obj pointer  to Geant4 Event  
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4UserEventAction              * obj )
{
  ///
  if( 0 != G4RunManager::GetUserRunAction() ) 
    {
      G4UserEventAction* prev = 
        G4RunManager::userEventAction ;
      Warning("Existing "          + GiGaUtil::ObjTypeName( prev ) + 
              " is replaced with " + GiGaUtil::ObjTypeName( obj  ) );
      if( 0 != prev ) { delete prev ; }
      ///
    }  
  ///
  G4RunManager::SetUserAction( obj ); 
  ///
  return StatusCode::SUCCESS ; 
};
// ============================================================================

// ============================================================================
/** declare the Geant4 Stacking Action 
 *  @param obj pointer  to Geant4 Stacking Action 
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4UserStackingAction           * obj )
{
  ///
  if( 0 != G4RunManager::GetUserStackingAction() ) 
    {
      G4UserStackingAction* prev = 
        G4RunManager::userStackingAction ;
      Warning("Existing "          + GiGaUtil::ObjTypeName( prev ) + 
              " is replaced with " + GiGaUtil::ObjTypeName( obj  ) );
      if( 0 != prev ) { delete prev ; }
      ///
    }  
  ///
  G4RunManager::SetUserAction( obj ); 
  ///
  return StatusCode::SUCCESS ; 
};
// ============================================================================

// ============================================================================
/** declare the Geant4 Stepping  Action 
 *  @param obj pointer  to Geant4 Stepping Action 
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4UserSteppingAction           * obj )
{
  ///
  if( 0 != G4RunManager::GetUserSteppingAction() ) 
    {
      G4UserSteppingAction* prev = 
        G4RunManager::userSteppingAction ;
      Warning("Existing "          + GiGaUtil::ObjTypeName( prev ) + 
              " is replaced with " + GiGaUtil::ObjTypeName( obj  ) );
      if( 0 != prev ) { delete prev ; }
      ///
    }  
  ///
  G4RunManager::SetUserAction( obj ); 
  ///
  return StatusCode::SUCCESS ; 
};
// ============================================================================

// ============================================================================
/** declare the Geant4 Tracking Action 
 *  @param obj pointer  to Geant4 Tracking Action 
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4UserTrackingAction           * obj )
{
  ///
  if( 0 != G4RunManager::GetUserTrackingAction() ) 
    {
      G4UserTrackingAction* prev = 
        G4RunManager::userTrackingAction ;
      Warning("Existing "          + GiGaUtil::ObjTypeName( prev ) + 
              " is replaced with " + GiGaUtil::ObjTypeName( obj  ) );
      if( 0 != prev ) { delete prev ; }
      ///
    }  
  ///
  G4RunManager::SetUserAction( obj ); 
  ///
  return StatusCode::SUCCESS ; 
};
// ============================================================================

// ============================================================================
/** declare the Geant4 User Interface session 
 *  @param obj pointer  to Geant4 User Interface session  
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGaRunManager::declare( G4UIsession                    * obj     ) 
{
  if(  0 != m_g4UIsession ) 
    {
      G4UIsession* prev = m_g4UIsession ;
      Warning("Existing "          + GiGaUtil::ObjTypeName( prev ) + 
              " is replaced with " + GiGaUtil::ObjTypeName( obj  ) );
      if( 0 != prev ) { delete prev ; }
      ///      
    }
  ///
  m_g4UIsession = obj ;
  m_uis_st = false;
  ///
  return StatusCode::SUCCESS;
  ///
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
