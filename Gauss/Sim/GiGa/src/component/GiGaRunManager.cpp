// $Id: GiGaRunManager.cpp,v 1.10 2002-12-16 16:23:16 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2002/12/15 17:13:21  ibelyaev
//  bug fixes and improved control over G4 verbosity
//
// ============================================================================
#define GIGA_GIGARUNMANAGER_CPP 1 
// ============================================================================
// STD & STL 
#include <string> 
#include <typeinfo> 
// GaudiKernel
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/System.h"
#include  "GaudiKernel/IService.h" 
#include  "GaudiKernel/ISvcLocator.h" 
#include  "GaudiKernel/IMessageSvc.h" 
#include  "GaudiKernel/IChronoStatSvc.h" 
#include  "GaudiKernel/Chrono.h" 
#include  "GaudiKernel/ToolFactory.h" 
// GiGa 
#include  "GiGa/GiGaException.h"
#include  "GiGa/IGiGaGeoSrc.h" 
#include  "GiGa/GiGaUtil.h" 
#include  "GiGa/GiGaMACROs.h" 
// Local 
#include  "GiGaRunManager.h" 
// G4 
#include  "G4Timer.hh"
#include  "G4StateManager.hh"
#include  "G4UIsession.hh"
#include  "G4UImanager.hh"
// G4 
#include  "G4VUserPrimaryGeneratorAction.hh"
#include  "G4VUserDetectorConstruction.hh"
//

// ============================================================================
/**  @file
 *   implementation of class GiGaRunManager
 *   @author: Vanya Belyaev
 *   @date: xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** @var GiGaRunManagerFacrtory 
 *  static factory for creation of GiGaRunNAnager object
 */
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaRunManager ) ;
// ============================================================================

namespace GiGaRunManagerLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaRunManager> s_Counter ;
#endif   
};

// ============================================================================
/** standard onstructor 
 *  @see  GiGaBase 
 *  @see   AlgTool
 *  @param type type of the run manager object
 *  @param name name of the run manager object
 *  @param parent pointer to parent object  
 */
// ============================================================================
GiGaRunManager::GiGaRunManager
( const std::string&  type   , 
  const std::string&  name   , 
  const IInterface*   parent ) 
  : G4RunManager     (         )
  , GiGaBase         ( type , name , parent )
  , m_krn_st         (  false  ) 
  , m_run_st         (  false  ) 
  , m_pre_st         (  false  ) 
  , m_pro_st         (  false  ) 
  , m_uis_st         (  false  ) 
  , m_rootGeo        (    0    ) 
  , m_geoSrc         (    0    ) 
  , m_g4UIsession    (    0    ) 
  ///
  , m_delDetConstr   ( true    ) 
  , m_delPrimGen     ( true    )
  , m_delPhysList    ( true    )
  , m_delRunAction   ( false   ) 
  , m_delEventAction ( false   ) 
  , m_delStackAction ( false   ) 
  , m_delTrackAction ( false   ) 
  , m_delStepAction  ( false   ) 
  //
#ifdef GIGA_DEBUG 
  , m_verbosity      ( 100     )
#else 
  , m_verbosity      ( 0       )
#endif 
{ 
  ///
  declareInterface<IGiGaRunManager> ( this ) ; 
  ///
  declareProperty ( "DeleteDetectorConstruction" , m_delDetConstr   ) ;
  declareProperty ( "DeletePrimaryGenerator"     , m_delPrimGen     ) ;
  declareProperty ( "DeleteRunAction"            , m_delRunAction   ) ;
  declareProperty ( "DeleteEventAction"          , m_delEventAction ) ;
  declareProperty ( "DeleteStackAction"          , m_delStackAction ) ;
  declareProperty ( "DeleteTrackAction"          , m_delTrackAction ) ;
  declareProperty ( "DeleteStepAction"           , m_delStepAction  ) ;
  /// 
  declareProperty ( "Verbosity"                  , m_verbosity      ) ;
  ///
#ifdef GIGA_DEBUG
  GiGaRunManagerLocal::s_Counter.increment () ;
#endif 
};
// ============================================================================

// ============================================================================
// destructor 
// ============================================================================
GiGaRunManager::~GiGaRunManager() 
{
  // increase the verbosity level for DEBUG mode 
#ifdef GIGA_DEBUG 
  G4RunManager::SetVerboseLevel( 1000000 );
#endif 
  
  if( !m_delDetConstr   ) { G4RunManager::SetUserInitialization
                              ( ( G4VUserDetectorConstruction*   ) 0 ) ; }
  if( !m_delPrimGen     ) { G4RunManager::SetUserAction        
                              ( ( G4VUserPrimaryGeneratorAction* ) 0 ) ; }
  if( !m_delPhysList    ) { G4RunManager::SetUserInitialization
                              ( ( G4VUserPhysicsList*            ) 0 ) ; }
  if( !m_delRunAction   ) { G4RunManager::SetUserAction        
                              ( ( G4UserRunAction*               ) 0 ) ; }
  if( !m_delEventAction ) { G4RunManager::SetUserAction
                              ( ( G4UserEventAction*             ) 0 ) ; }
  if( !m_delStackAction ) { G4RunManager::SetUserAction
                              ( ( G4UserStackingAction*          ) 0 ) ; }
  if( !m_delTrackAction ) { G4RunManager::SetUserAction
                              ( ( G4UserTrackingAction*          ) 0 ) ; }
  if( !m_delStepAction  ) { G4RunManager::SetUserAction 
                              ( ( G4UserSteppingAction*          ) 0 ) ; }
#ifdef GIGA_DEBUG
    GiGaRunManagerLocal::s_Counter.decrement () ;
#endif
};
// ============================================================================

// ============================================================================
/** Retrieve the processed event 
 *  @param  event pointer to processed event  
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunManager::retrieveTheEvent( const G4Event*& event ) 
{
  event = 0 ; 
  try
    {
      StatusCode sc( StatusCode::SUCCESS ) ; 
      if( !evt_Is_Processed() ) { sc = processTheEvent()                  ; }
      if( sc.isFailure()      ) { return Exception("retrieveTheEvent()" ) ; }
    }
  catch( const GaudiException& Excp )
    { return Exception( "retrieveTheEvent()", Excp ); }
  catch( const std::exception& Excp ) 
    { return Exception( "retrieveTheEvent()", Excp ); }
  catch( ... ) 
    { return Exception( "retrieveTheEvent()"       ); }
  ///
  set_evt_Is_Prepared( false ) ; 
  event = G4RunManager::GetCurrentEvent() ;  
  ///
  return Print("Geant4 Event is retrieved with success" , 
               StatusCode::SUCCESS                      , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** Process the prepared event 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunManager::processTheEvent()
{
  ///
  Chrono    chrono ( chronoSvc () , name()+"::processTheEvent()" ) ; 
  ///
  set_evt_Is_Processed( false ) ;                      
  ///   
  try
    {
      StatusCode sc( StatusCode::SUCCESS ) ;
      if( !evt_Is_Prepared() ) { sc = prepareTheEvent() ; } 
      if( sc.isFailure() ) { return Exception("processTheEvent()") ;}
    }
  catch( const GaudiException& Excp )
    { return Exception( "processTheEvent()", Excp ); }
  catch( const std::exception& Excp ) 
    { return Exception( "processTheEvent()", Excp ); }
  catch( ... ) 
    { return Exception( "processTheEvent()"       ); }
  ///
  if( !evt_Is_Prepared() ) { return StatusCode::FAILURE ; }  /// RETURN !!! 
  ///  to be changed soon  
  if( G4RunManager::verboseLevel > 0    ) { (G4RunManager::timer)->Start() ; } 
  ///
  if( G4RunManager::GetCurrentEvent()->GetNumberOfPrimaryVertex() == 0 )
    { Warning(" Empty event to be processed "); } 
  else
    { 
      G4RunManager::eventManager->ProcessOneEvent( G4RunManager::currentEvent); 
      Print("Internal process one event", StatusCode::SUCCESS, MSG::DEBUG ); 
    } 
  ///  to be changed soon  
  if( G4RunManager::verboseLevel > 0 )
    {
      (G4RunManager::timer)->Stop();
      // G4cout << "Run terminated." << G4endl;
      // G4cout << "Run Summary"     << G4endl;
      if( G4RunManager::runAborted ) 
        { G4cout << " G4Run Aborted after " << 1 << " events processed."  ; }
      else                           
        { G4cout << " G4: Number of events processed : " << 1             ; }
      G4cout << " Timer: " << *(G4RunManager::timer) << G4endl;
    }
  ///
  set_evt_Is_Processed( true  ); 
  set_evt_Is_Prepared ( false ); 
  ///
  if( 0 != m_g4UIsession && !uis_Is_Started() )
    { 
      ///
      set_uis_Is_Started( true  ) ; 
      m_g4UIsession->SessionStart() ; 
      set_uis_Is_Started( false ) ; 
    } 
  ///
  return Print("Geant4 Event is processed successfully" , 
               StatusCode::SUCCESS                      , MSG::VERBOSE ) ;
};
// ============================================================================

// ============================================================================
/** Prepare the event 
 *  @param vertex pointer to (main) primary vertex 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunManager::prepareTheEvent( G4PrimaryVertex * vertex )
{
  ///
  set_evt_Is_Prepared ( false ); 
  const std::string Tag( name() + "::prepareTheEvent()" );
  const std::string method( " initializeRun() " ) ;
  ///
  try { if( !run_Is_Initialized() ) { initializeRun() ; } }
  catch( const GaudiException& Excp )
    { return Exception( "prepareTheEvent()", Excp ); }
  catch( const std::exception& Excp ) 
    { return Exception( "prepareTheEvent()", Excp ); }
  catch( ... ) 
    { return Exception( "prepareTheEvent()"       ); }
  //
  if( !run_Is_Initialized() ) 
    { return StatusCode::FAILURE ; }  ///< RETURN !!!   
  /// "clear" the previous event  
  if( evt_Is_Processed() )
    {
      if( 0 != G4RunManager::GetCurrentEvent() )
        {
          G4RunManager::StackPreviousEvent(G4RunManager::currentEvent);
          G4RunManager::currentEvent   =  0 ; 
          if( 0 != G4StateManager::GetStateManager() ) 
            { G4StateManager::GetStateManager() 
                ->SetNewState( G4State_GeomClosed ); } 
        }
      set_evt_Is_Processed( false );
    }
  ///
  if     ( 0 != G4RunManager::GetCurrentEvent() )
    { Print("Current G4Event is kept ",StatusCode::SUCCESS,MSG::VERBOSE );}
  else if( 0 != G4RunManager::userPrimaryGeneratorAction ) 
    {  
      G4RunManager::currentEvent = G4RunManager::GenerateEvent(0) ; 
      Print("New G4Event is GENERATED",StatusCode::SUCCESS,MSG::DEBUG);
    }
  else 
    {  
      G4RunManager::currentEvent = new G4Event; 
      Print("Empty G4Event is CREATED",StatusCode::SUCCESS,MSG::DEBUG);
    }
  ///
  if( 0 == G4RunManager::currentEvent ) 
    { return StatusCode::FAILURE; }  /// RETURN !!!  
  ///
  if( 0 !=  vertex ) 
    { G4RunManager::currentEvent->AddPrimaryVertex( vertex ); } 
  ///
  set_evt_Is_Prepared ( true   ); 
  ///
  return Print("Geant4 Event preparation is succesfull" , 
               StatusCode::SUCCESS                      , MSG::VERBOSE );
}; 

// ============================================================================
/** initialize the Geant4 Run
 *  @return status code 
 */
// ============================================================================
StatusCode  GiGaRunManager::initializeRun()
{
  ///
  if( run_Is_Initialized() ) 
    { 
      Print(" Current G4 Run is to be terminated "); 
      G4RunManager::RunTermination();
    }
  ///
  set_run_Is_Initialized( false );
  set_evt_Is_Prepared   ( false );
  set_evt_Is_Processed  ( false );
  ///
  const std::string method(" initializeKernel() ");
  ///
  try { if( !krn_Is_Initialized() ) { initializeKernel() ; } }
  catch( const GaudiException& Excp )
    { return Exception( "initializeRun()", Excp ); }
  catch( const std::exception& Excp ) 
    { return Exception( "initializeRun()", Excp ); }
  catch( ... ) 
    { return Exception( "initializeRun()"       ); }
  ///
  if( !krn_Is_Initialized() ) {  return StatusCode::FAILURE; }
  ///
  G4ApplicationState currentState = 
    G4StateManager::GetStateManager()->GetCurrentState();
  ///
  if( G4State_PreInit != currentState  && G4State_Idle != currentState ) 
    { return Exception("initializeRun(): wrong Geant4  state!"); }
  if( !G4RunManager::initializedAtLeastOnce            ) 
    { return Exception("initializeRun(): G4 kernel must be initialised!");}
  if( !G4RunManager::ConfirmBeamOnCondition()          )
    { return Exception("initializeRun(): no G4 Beam-On conditions!");}
  ///
  G4RunManager::RunInitialization();
  set_run_Is_Initialized( true );
  ///
  return Print("Geant4 Run is initialized  successfully" , 
               StatusCode::SUCCESS                       , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** initialize the Geant4 kernel
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunManager::initializeKernel() 
{
  ///
  if( krn_Is_Initialized() ) 
    { Warning("Try to reinitialize Geant4 Kernel "); } 
  //
  set_krn_Is_Initialized( false ); 
  set_run_Is_Initialized( false );
  set_evt_Is_Prepared   ( false );
  set_evt_Is_Processed  ( false );
  //
  G4ApplicationState    currentState = 
    G4StateManager::GetStateManager()->GetCurrentState();
  //  
  if( 0 != m_g4UIsession  ) 
    { Print("Geant4 User Interface        is     created " , 
            StatusCode::SUCCESS , MSG::DEBUG ) ; }
  else 
    { Print("Geant4 User Interface        is NOT created " , 
            StatusCode::SUCCESS , MSG::DEBUG ) ; }
  ///
  if( G4State_PreInit != currentState  && G4State_Idle !=  currentState  )
    { return Exception("initializeKernel(): Wronmg G4 state!");}
  if( !(G4RunManager::geometryInitialized ) && 
      ( 0 == G4RunManager::userDetector   ) && 
      ( 0 == m_rootGeo                    ) && 
      ( 0 == geoSrc()                     )    ) 
    { return Exception("initialzeKernel(): no any geometry sources!"); }
  if( !(G4RunManager::physicsInitialized ) &&  
      ( 0 == G4RunManager::physicsList   )     )
    { return Exception("initializeKernel(): no physics information!");}
  if( !(G4RunManager::cutoffInitialized  ) &&  
      ( 0 == G4RunManager::physicsList   )     )
    { return Exception("initializeKernel(): no cut-off information!");}
  ///
  G4RunManager::Initialize();
  ///
  set_krn_Is_Initialized( true );
  ///
  if( 0 != m_g4UIsession  && !uis_Is_Started()      )
    { 
      ///
      set_uis_Is_Started( true  ) ; 
      m_g4UIsession->SessionStart() ; 
      set_uis_Is_Started( false ) ; 
    } 
  ///
  return Print("Geant4 Kernel is initialized  successfully" , 
               StatusCode::SUCCESS                          , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalize run manager 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunManager::finalizeRunManager()
{
  ///
  if( 0 !=  G4RunManager::currentEvent )
    { G4RunManager::StackPreviousEvent(G4RunManager::currentEvent ) ; }
  G4RunManager::RunTermination(); 
  ///
  set_run_Is_Initialized( false ) ; 
  set_evt_Is_Prepared   ( false ) ; 
  set_evt_Is_Processed  ( false ) ; 
  ///
  return Print("Geant4 Run  is finalized  successfully" , 
               StatusCode::SUCCESS                      , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================


