// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/component/GiGaRunManager.cpp,v 1.2 2001-04-24 08:54:08 ibelyaev Exp $ 
//
#include <string> 
#include <typeinfo> 
//
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/System.h"
#include  "GaudiKernel/IService.h" 
#include  "GaudiKernel/ISvcLocator.h" 
#include  "GaudiKernel/IMessageSvc.h" 
#include  "GaudiKernel/IChronoStatSvc.h" 
#include  "GaudiKernel/Chrono.h" 
//
#include  "GiGa/GiGaException.h"
#include  "GiGa/IGiGaGeomCnvSvc.h" 
//
#include  "G4Timer.hh"
#include  "G4StateManager.hh"
#include  "G4VisManager.hh" 
#include  "G4UIsession.hh"
#include  "G4UImanager.hh"
//
#include  "G4VUserPrimaryGeneratorAction.hh"
#include  "G4VUserDetectorConstruction.hh"
//
#ifdef G4UI_USE_WO
#include "G4UIWo.hh"   
#endif // G4UI_USE_WO 
//
#ifdef G4UI_USE_GAG
#include "G4UIGAG.hh" 
#endif // G4UI_USE_GAG
//
#ifdef G4UI_USE_XM
#include "G4UIXm.hh"  
#endif // G4UI_USE_XM 
//
#ifdef G4UI_USE_XAW
#include "G4UIXaw.hh"  
#endif // G4UI_USE_XAW
//
#ifdef G4UI_USE_TERMINAL
#include "G4UIterminal.hh"             
#include "G4UItcsh.hh"             
#include "G4UIcsh.hh"             
#endif // G4UI_USE_TERMINAL 
//
#ifdef G4UI_USE
#include "G4UIterminal.hh"             
#include "G4UItcsh.hh"             
#include "G4UIcsh.hh"             
#endif // G4UI_USE
//

// these lines does not work on Win!!! why??? B.B. 
//#ifdef G4UI_USE_WIN32
//#include "G4Win32.hh"
//#endif

// local 
#include  "GiGaRunManager.h" 

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
///    class GiGaRunManager : an adaptation of geneal G4RunManager to Gaudi     ///      
///                                                                             ///
///    Author: Vanya Belyaev                                                    ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
static inline G4UImanager*    g4UImanager   () { return G4UImanager::GetUIpointer(); } ; 
//////////////////////////////////////////////////////////////////////////////////
static inline G4StateManager* g4StateManager() { return G4StateManager::GetStateManager(); } ; 
//////////////////////////////////////////////////////////////////////////////////
template <class TYPE> inline const std::string objType( TYPE* type)
{ 
  if( 0 == type ) { return "NONE" ; } 
  return System::typeinfoName( typeid( *type ) ) ;
};  
///////////////////////////////////////////////////////////////////////////////////
inline G4UImanager* operator<<( G4UImanager* ui , const std::string& cmd ) 
{ 
  if( 0 != ui ) { ui->ApplyCommand( cmd ) ; }  
  return  ui ; 
}; 
///////////////////////////////////////////////////////////////////////////////////
inline G4UImanager* operator<<( G4UImanager* ui , 
                                const GiGaRunManager::Strings& cmds ) 
{
  for( GiGaRunManager::Strings::const_iterator ci = cmds.begin() ; cmds.end() != ci ; ++ci )
    { if( 0 != ui ) { ui << *ci; } }  
  return  ui ;  
}; 
///////////////////////////////////////////////////////////////////////////////////
GiGaRunManager::GiGaRunManager( const std::string & Name   ,
                                ISvcLocator*        svc    ) 
  : G4RunManager   (         )
  , m_svcLoc       (   svc   )  
  , m_rootGeo      (    0    ) 
  , m_cnvSvc       (    0    ) 
  , m_g4UIsession  (    0    ) 
  , m_g4VisManager (    0    ) 
  , m_krn_st       (  false  ) 
  , m_run_st       (  false  ) 
  , m_pre_st       (  false  ) 
  , m_pro_st       (  false  ) 
  , m_vis_st       (  false  ) 
  , m_uis_st       (  false  ) 
  , m_name         ( Name    ) 
  , m_msgSvc       (    0    ) 
  , m_chronoSvc    (    0    ) 
  , m_s            (         ) 
  , m_sE           (         ) 
  , m_eE           (         ) 
  , m_sR           (         ) 
  , m_eR           (         ) 
  , m_e            (         ) 
{
  ///
  StatusCode sc; 
  {
    IService* iS = 0 ; 
    sc = svcLoc()->getService("MessageSvc" , iS ) ;
    if( sc.isSuccess() && 0 != iS ) { m_msgSvc = dynamic_cast<IMessageSvc*> ( iS ); }  
    if( 0 != msgSvc() ) { msgSvc()->addRef() ; } 
  };
  ///
  {
    IService* iS = 0 ; 
    sc = svcLoc()->getService("ChronoStatSvc" , iS ) ;
    if( sc.isSuccess() && 0 != iS ) { m_chronoSvc = dynamic_cast<IChronoStatSvc*> ( iS ); }  
    if( 0 != chronoSvc() ) { chronoSvc()->addRef() ; } 
  };
  ///
  {
    IService* iS = 0 ; 
    sc = svcLoc()->getService("CiGaGeomCnvSvc" , iS ) ;
    if( sc.isSuccess() && 0 != iS ) { m_cnvSvc = dynamic_cast<IGiGaGeomCnvSvc*> ( iS ); }  
    if( 0 != m_cnvSvc ) { m_cnvSvc ->addRef() ; } 
  };
  ///
};
///////////////////////////////////////////////////////////////////////////////////
GiGaRunManager::~GiGaRunManager()
{
  /// release services
  if( 0 != m_cnvSvc     ) { m_cnvSvc    ->release() ;  m_cnvSvc    = 0 ; } 
  if( 0 != chronoSvc () ) { chronoSvc ()->release() ;  m_chronoSvc = 0 ; } 
  if( 0 != msgSvc    () ) { msgSvc    ()->release() ;  m_msgSvc    = 0 ; } 
}
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::retrieveTheEvent( const G4Event*& event ) 
{
  ///
  event = 0 ; 
  ///
  const std::string Tag   ( name()+".retrieveTheEvent(const G4Event*&)"); 
  const std::string method( name()+".processTheEvent()"); 
  ///
  StatusCode sc( StatusCode::SUCCESS ) ; 
  /// 
  ___GIGA_TRY___
    {
      if( !evt_Is_Processed() ) { sc = processTheEvent() ; } 
      Assert( sc.isSuccess() , Tag + " failure from " + method , sc ) ; 
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method)           ;  /// ATTENTION !!!
  ///
  set_evt_Is_Prepared( false ) ; 
  event = G4RunManager::GetCurrentEvent() ;  
  ///
  return StatusCode::SUCCESS;
  ///
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::processTheEvent()
{
  ///
  const std::string Tag(name()+".processTheEvent()"); 
  const std::string method( " prepareTheEvent() " ); 
  ///
  MsgStream log    ( msgSvc    () , Tag ) ;
  Chrono    chrono ( chronoSvc () , Tag ) ; 
  ///
  set_evt_Is_Processed( false ) ;                      
  ///   
  StatusCode sc( StatusCode::SUCCESS ) ;
  ___GIGA_TRY___ 
    {
      if( !evt_Is_Prepared() ) { sc = prepareTheEvent() ; } 
      Assert( sc.isSuccess() , Tag + " failure from " + method , sc ) ;   
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method)           ;  /// ATTENTION !!!
  ///
  if( !evt_Is_Prepared() ) { return StatusCode::FAILURE ; }  /// RETURN !!! 
  ///  to be changed soon  
  ///  if( G4RunManager::pauseAtBeginOfEvent && 0 != g4StateManager() ) 
  ///  { g4StateManager()->Pause("BeginOfEvent") ; }
  if( G4RunManager::verboseLevel > 0    ) { (G4RunManager::timer)->Start() ; } 
  /// apply commands to UI manager 
  ///
  g4UImanager() << startOfEvtUIcommands();
  if( G4RunManager::GetCurrentEvent()->GetNumberOfPrimaryVertex() == 0 )
    { log << MSG::WARNING << " Empty event to be processed " << endreq; } 
  else
    { 
      G4RunManager::eventManager->ProcessOneEvent( G4RunManager::currentEvent); 
      log << MSG::DEBUG << " internal process one event " << endreq; 
      // G4RunManager::currentEvent->Draw();
      // G4RunManager::AnalyzeEvent( G4RunManager::currentEvent );
    } 
  /// apply commands to UI manager 
  g4UImanager() << endOfEvtUIcommands();
  ///  to be changed soon  
  ///  if( G4RunManager::pauseAtEndOfEvent && 0 != g4StateManager() ) 
  ///  { g4StateManager()->Pause("EndOfEvent"); }
  if( G4RunManager::verboseLevel > 0 )
    {
      (G4RunManager::timer)->Stop();
      G4cout << "Run terminated." << G4endl;
      G4cout << "Run Summary"     << G4endl;
      if( G4RunManager::runAborted ) { G4cout << "  Run Aborted after " << 1 << " events processed." << G4endl; }
      else                           { G4cout << "  Number of events processed : " << 1 << G4endl; }
      G4cout << "  "  << *(G4RunManager::timer) << G4endl;
    }
   ///
  set_evt_Is_Processed( true  ); 
  set_evt_Is_Prepared ( false ); 
  ///
  log << MSG::INFO << " Geant4 Event is processed successfully " << endreq; 
  ///
  return StatusCode::SUCCESS;
  ///
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::prepareTheEvent( G4PrimaryVertex * vertex )
{
  ///
  set_evt_Is_Prepared ( false ); 
  const std::string Tag( name() + ".prepareTheEvent()" );
  const std::string method( " initializeRun() " ) ;
  ///
  MsgStream log    ( msgSvc    () , Tag ) ;
  ///
  ___GIGA_TRY___ 
    { if( !run_Is_Initialized() ) { initializeRun() ; } }
  ___GIGA_CATCH_AND_THROW___(Tag,method)          ;   /// ATTENTION !!!
  //
  if( !run_Is_Initialized() ) { return StatusCode::FAILURE ; }  /// RETURN !!!   
  /// "clear" the previous event  
  if( evt_Is_Processed() )
    {
      if( 0 != G4RunManager::GetCurrentEvent() )
        {
          G4RunManager::StackPreviousEvent(G4RunManager::currentEvent);
          G4RunManager::currentEvent   =  0 ; 
          if( 0 != g4StateManager() ) { g4StateManager()->SetNewState( GeomClosed ); } 
        }
      set_evt_Is_Processed( false );
    }
  ///
  if     ( 0 != G4RunManager::GetCurrentEvent() )
    {  log << MSG::VERBOSE << " Current G4Event is kept  " << endreq;  }
  else if( 0 != G4RunManager::userPrimaryGeneratorAction ) 
    {  
      G4RunManager::currentEvent = G4RunManager::GenerateEvent(0) ; 
      log << MSG::DEBUG   << " New G4Event is GENERATED " << endreq;    
    }
  else 
    {  
      G4RunManager::currentEvent = new G4Event; 
      log << MSG::DEBUG   << " New G4Event (empty) is created " << endreq;    
    }
  ///
  if( 0 == G4RunManager::currentEvent ) { return StatusCode::FAILURE; }  /// RETURN !!!  
  ///
  if( 0 !=  vertex ) { G4RunManager::currentEvent->AddPrimaryVertex( vertex ); } 
  ///
  set_evt_Is_Prepared ( true   ); 
  ///
  log << MSG::VERBOSE << " Geant4 Event preparation is successfull " << endreq; 
  ///
  return StatusCode::SUCCESS;
  ///
}; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode  GiGaRunManager::initializeRun()
{
  ///
  ///
  const std::string Tag( name() + ".initialiseRun()" ); 
  MsgStream log    ( msgSvc    () , Tag ) ;
  ///
  if( run_Is_Initialized() ) 
    { 
      log << MSG::INFO << " Current G4 Run is to be terminated " << endreq; 
      G4RunManager::RunTermination();
      /// Apply commands to UI manager
      g4UImanager() << endOfRunUIcommands(); 
    }
  ///
  set_run_Is_Initialized( false );
  set_evt_Is_Prepared   ( false );
  set_evt_Is_Processed  ( false );
  ///
  const std::string method(" initializeKernel() ");
  ///
  ___GIGA_TRY___ 
    { if( !krn_Is_Initialized() ) { initializeKernel() ; }  }
  ___GIGA_CATCH_AND_THROW___(Tag,method)              ; /// ATTENTION !!!  
  ///
  if( !krn_Is_Initialized() )  {  return StatusCode::FAILURE; }      /// RETURN !!!
  ///
  G4ApplicationState currentState = G4StateManager::GetStateManager()->GetCurrentState();
  ///
  Assert( ( currentState == PreInit || currentState == Idle ) , 
          Tag +  " Wrong Geant4 State (must be PreInit or Idle)" ) ;  
  Assert( G4RunManager::initializedAtLeastOnce                , 
          Tag +  " Geant4 kernel must be initialised"            ) ; 
  Assert( G4RunManager::ConfirmBeamOnCondition()              ,            
          Tag + " Geant4 Beam-On conditions are not satisfied!"  ) ; 
  ///
  G4RunManager::RunInitialization();  
  set_run_Is_Initialized( true );
  ///
  log  << MSG::DEBUG << " Geant4 Run is initialized  successfully " << endreq;
  /// Apply commands to UI manager
  g4UImanager() << startOfRunUIcommands(); 
  //
  return StatusCode::SUCCESS;
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::initializeKernel() 
{
  ///
  const std::string Tag( name() + ".initializeKernel()" ); 
  //
  MsgStream log( msgSvc() , Tag );
  //
  if( krn_Is_Initialized() ) 
    { log << MSG::WARNING << " try to reinitialize Geant4 Kernel " << endreq; } 
  //
  set_krn_Is_Initialized( false ); 
  set_run_Is_Initialized( false );
  set_evt_Is_Prepared   ( false );
  set_evt_Is_Processed  ( false );
  //
  G4ApplicationState    currentState = G4StateManager::GetStateManager()->GetCurrentState();
  //  
  if( 0 != m_g4UIsession  ) 
    { log << MSG::VERBOSE << " Geant4 User Interface        is     created " << endreq; }
  else 
    { log << MSG::DEBUG   << " Geant4 User Interface        is NOT created " << endreq; }
  //
  if( 0 != m_g4VisManager )
    { log << MSG::VERBOSE << " Geant4 Visualization Manager is initialized " << endreq; }
  else
    { log << MSG::DEBUG   << " Geant4 Visualization Manager is NOT created " << endreq; }
  ////
  Assert( ( PreInit == currentState  || Idle ==  currentState ) , Tag + " Wrong curent state (must be PreInit or Idle)" ) ; 
  Assert( ( G4RunManager::geometryInitialized || 
            ( 0 != G4RunManager::userDetector ) || ( 0 != m_rootGeo ) || ( 0 != cnvSvc() ) ) , 
          Tag + " It is not possible to initialize the Detector!"   ) ; 
  Assert( ( G4RunManager::physicsInitialized  || ( 0 != G4RunManager::physicsList ) ) , 
          Tag + " It is not possible to initialize the Physics!"    ) ; 
  Assert( ( G4RunManager::cutoffInitialized  || ( 0 != G4RunManager::physicsList ) ) ,   
          Tag + " It is not possible to initialize the CutOff!"     ) ; 
  ///
  if( 0 == m_g4UIsession ) { createUIsession() ; } 
  ///
  G4RunManager::Initialize();
  ///
  if( 0 != m_g4VisManager && !vis_Is_Initialized() ) 
    { m_g4VisManager->Initialize()  ; set_vis_Is_Initialized( true ) ; }
  /// Apply commands to UI manager
  g4UImanager() << startUIcommands(); 
  /// 
  set_krn_Is_Initialized( true );
  ///
  log  << MSG::DEBUG << " Geant4 Kernel is initialized  successfully " << endreq;
  ///
  if( 0 != m_g4UIsession  && !uis_Is_Started()      )
    { 
      ///
// GG  On Nt  with session = terminal objType throws an exception!!!
//      log << MSG::INFO 
//          << " Start Geant4 User Session = " 
//          << objType(m_g4UIsession) 
//          << endreq ; 
      ///
      set_uis_Is_Started( true  ) ; 
      m_g4UIsession->SessionStart() ; 
      set_uis_Is_Started( false ) ; 
    } 
  ///
  return StatusCode::SUCCESS; 
  ///
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::finalizeRunManager()
{
  ///
  G4RunManager::RunTermination(); 
  ///
  /// Apply commands to Geant4 UI manager
  /// 
  g4UImanager() << endOfRunUIcommands() ;
  g4UImanager() << endUIcommands()      ;   
  //
  set_run_Is_Initialized( false ) ; 
  set_evt_Is_Prepared   ( false ) ; 
  set_evt_Is_Processed  ( false ) ; 
  ///
  return StatusCode::SUCCESS; 
  ///
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4VUserPrimaryGeneratorAction  * obj )  
{ 
  ///
  if( 0 != G4RunManager::GetUserPrimaryGeneratorAction() ) 
    {
      ///
      MsgStream log( msgSvc() , name() + ".declare(G4VUserPrimaryGeneratorAction)" ); 
      log << MSG::WARNING 
          << ( 0 != obj ? " replace " : " remove " )
          << " existing G4VUserPrimaryGeneratorAction=" 
          << objType( G4RunManager::GetUserPrimaryGeneratorAction() )
          << " by " 
          << objType( obj ) ;
      ///
      if( 0 != G4RunManager::userPrimaryGeneratorAction ) 
        { delete G4RunManager::userPrimaryGeneratorAction ; }
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
    { G4StateManager::GetStateManager()->SetNewState(PreInit); } 
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4VUserDetectorConstruction    * obj ) 
{
  ///
  MsgStream log( msgSvc() , name() + ".declare(G4VUserDetectorconstruction)" ); 
  ///
  Assert ( 0 == m_rootGeo                                    , 
           ".declare(G4VUserDetectorConstruction*)::attempt to replace the existing G4VPhysicalVolume=" 
           + objType( m_rootGeo ) 
           + " by " 
           + objType ( obj ) );  
  ///
  Assert ( 0 == G4RunManager::GetUserDetectorConstruction() , 
           ".declare(G4VUserDetectorConstruction*)::attempt to replace the existing G4VUserDetectorConstruction=" 
           + objType( G4RunManager::GetUserDetectorConstruction() ) 
           + " by " 
           + objType ( obj ) );  
  ///
  Assert( 0 != obj , ".declare(G4VUserDetectorconstruction*)::obj  points to NULL!" ); 
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
    { G4StateManager::GetStateManager()->SetNewState(PreInit); } 
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4VPhysicalVolume              * obj ) 
{
  ///
  MsgStream log( msgSvc() , name() + ".declare(G4VPhysicalVolume*)" ); 
  ///
  Assert ( 0 == m_rootGeo                                    , 
           ".declare(G4VPhysicalVolume*)::attempt to replace the existing G4VPhysicalVolume=" 
           + objType( m_rootGeo ) 
           + " by " 
           + objType ( obj ) );  
  ///
  Assert ( 0 == G4RunManager::GetUserDetectorConstruction() , 
           ".declare(G4VPhysicalVolume*)::attempt to replace the existing G4VUserDetectorConstruction=" 
           + objType( G4RunManager::GetUserDetectorConstruction() ) 
           + " by " 
           + objType ( obj ) );  
  ///
  Assert( 0 != obj , ".declare(G4PhysicalVolume*)::obj points to NULL!" ); 
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
    { G4StateManager::GetStateManager()->SetNewState(PreInit); } 
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4VUserPhysicsList             * obj )
{
  ///
  Assert ( 0 == G4RunManager::GetUserPhysicsList() , 
           ".declare(G4VUserPhysicsList*)::attempt to replace the existing G4VUserPhysicsList=" 
           + objType( G4RunManager::GetUserPhysicsList() ) 
           + " by " 
           + objType( obj )  ) ; 
  ///
  Assert( 0 != obj , 
          ".declare(G4VUserPhysicsList*)::obj points to NULL!" ); 
  ///
  G4RunManager::physicsInitialized = false ; 
  G4RunManager::cutoffInitialized  = false ; 
  G4RunManager::SetUserInitialization( obj ); 
  ///
  if( !G4RunManager::physicsInitialized ) { R4RunManager::InitializePhysics() ; } 
  if( !G4RunManager::cutoffInitialized  ) { R4RunManager::InitializeCutOff () ; } 
  ///
  set_krn_Is_Initialized ( false ) ; 
  set_run_Is_Initialized ( false ) ; 
  set_evt_Is_Prepared    ( false ) ; 
  set_evt_Is_Processed   ( false ) ; 
  ///
  if( 0 != G4StateManager::GetStateManager() ) 
    { G4StateManager::GetStateManager()->SetNewState(PreInit); } 
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4UserRunAction                * obj )
{
  ///
  Assert ( 0 == G4RunManager::GetUserRunAction() , 
           ".declare(G4UserRunAction*)::attempt to replace the existing G4UserRunAction=" 
           + objType( G4RunManager::GetUserRunAction() ) 
           + " by " 
           + objType( obj ) ) ; 
  ///
  G4RunManager::SetUserAction( obj ); 
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4UserEventAction              * obj )
{
  ///
  Assert ( 0 == G4RunManager::GetUserEventAction() , 
           ".declare(G4UserEventAction*)::attempt to replace the existing G4UserEventAction=" 
           + objType( G4RunManager::GetUserEventAction() )  
           + " by " 
           + objType( obj ) ) ; 
  ///
  G4RunManager::SetUserAction( obj ); 
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4UserStackingAction           * obj )
{
  ///
  Assert ( 0 == G4RunManager::GetUserStackingAction() , 
           ".declare(G4UserStackingAction*)::attempt to replace the existing G4UserStackingAction=" 
           + objType( G4RunManager::GetUserStackingAction() )  
           + " by " 
           + objType( obj ) );
  ///
  G4RunManager::SetUserAction( obj ); 
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4UserSteppingAction           * obj )
{
  ///
  Assert ( 0 == G4RunManager::GetUserSteppingAction() , 
           ".declare(G4UserSteppingAction*)::attempt to replace the existing G4UserSteppingAction=" 
           + objType( G4RunManager::GetUserSteppingAction() )
           + " by " 
           + objType( obj ) );
  ///
  G4RunManager::SetUserAction( obj ); 
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4UserTrackingAction           * obj )
{
  ///
  Assert ( 0 == G4RunManager::GetUserTrackingAction() , 
           ".declare(G4UserTrackingAction*)::attempt to replace the existing G4UserTrackingAction=" 
           + objType( G4RunManager::GetUserTrackingAction() )
           + " by " 
           + objType( obj ) );
  ///
  G4RunManager::SetUserAction( obj ); 
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaRunManager::declare( G4VisManager                   * obj )
{
  m_g4VisManager = obj ; 
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode  GiGaRunManager::createUIsession() 
{
  ///
  MsgStream log ( msgSvc() , name() + ".createUIsessions() " );
  ///
#ifndef G4UI_USE
  ///
  ///
  log << MSG::INFO 
      << "::createUIsessions():: " 
      << " No Geant4 UI sessions will be created! " << endreq;
  ///
  return StatusCode::SUCCESS ; 
  ///
#endif 
  ///
  ///
  if( 0 != m_g4UIsession ) { return StatusCode::FAILURE; } 
  ///
  if( uis_Is_Started()   ) { return StatusCode::FAILURE; } 
  ///
  set_uis_Is_Started ( false ) ; 
  ///
  if( UIsessions().empty() ) { return StatusCode::SUCCESS; } 
  ///
  {
    Strings::const_iterator ci = UIsessions().begin() ;
    while( UIsessions().end() != ci && 0 == m_g4UIsession )  
      {
        ////
        const std::string session ( *ci++ ); 
        ///
        log << MSG::VERBOSE << " Attempt to create UIsession of Type= " << session 
            << "\t" << "Argc="    << System::argc()
            << "\t" << "argv="    << System::argv()  << endreq; 
        
        log << MSG::VERBOSE << " Attempt to create UIsession of Type= " << session 
            << "\t" << "Argc="    << System::argc()
            << "\t" << "argv="    << System::argv()  
            << "\t" << "argv[0]=" << System::argv()[0] << endreq;
        ///
        if(       "Wo"        == session ) 
          { 
#ifdef G4UI_USE_WO
            m_g4UIsession = new G4UIWo  ( System::argc() , System::argv() ) ; 
#endif // G4UI_USE_WO
          } 
//  else if ( "G4UIWin32" == session )
//#ifdef G4UI_USE_WIN32
//    // GG  !!!!!!!!!
//    // constructor requires some arguments !!! 
//    m_g4UIsession = new G4UIWin32();
//#endif
//  }
        else if ( "GAG"       == session  )    
          {
#ifdef G4UI_USE_GAG
            m_g4UIsession = new G4UIGAG () ; 
#endif // G4UI_USE_GAG
          }
        else if ( "Xm"        == session  ) 
          {
#ifdef G4UI_USE_XM
            m_g4UIsession = new G4UIXm  ( System::argc() , System::argv() ) ; 
#endif // G4UI_USE_XM 
          }
        else if ( "Xaw"       == session  ) 
          {
#ifdef G4UI_USE_XAW
            m_g4UIsession = new G4UIXaw ( System::argc() , System::argv() ) ; 
#endif // G4UI_USE_XAW
          }
        else if ( "tcsh"  == session  ) 
          {
#ifdef G4UI_USE_TERMINAL
            m_g4UIsession = new G4UIterminal( new G4UIcsh ()  ) ;
#endif // G4UI_USE_TERMINAL 
          }
        else if ( "csh"  == session  ) 
          {
#ifdef G4UI_USE_TERMINAL
            m_g4UIsession = new G4UIterminal( new G4UIcsh ()  ) ;
#endif // G4UI_USE_TERMINAL 
          }
        else if ( "terminal"  == session  ) 
          {
#ifdef G4UI_USE_TERMINAL
            m_g4UIsession = new G4UIterminal () ;           
#endif // G4UI_USE_TERMINAL 
          }
      }
  }
  ///
  ///
  ///
#ifdef G4UI_USE
  if( 0 == m_g4UIsession ) { m_g4UIsession = new G4UIterminal () ; } 
#endif // G4UI_USE 
  ///
  ///
  if( 0 != m_g4UIsession ) 
    {
// GG  On Nt  with session = terminal objType throws an exception!!!
//      log << MSG::DEBUG << "::createUIsessions():: session=" 
//          << objType( m_g4UIsession ) << " is created " << endreq; 
    }
  else 
    {
      log << MSG::WARNING << "::createUIsessions():: session is not created" 
          << endreq; 
    }
  ///
  return ( 0 != m_g4UIsession)  ? StatusCode::SUCCESS : StatusCode::FAILURE ;   
  ///
}; 
///////////////////////////////////////////////////////////////////////////////////
void GiGaRunManager::BeamOn( int         n_event       ,                      
                             const char* macroFile ,                       
                             int         n_select      )
{
  ///
  const std::string Tag( name()+".beamOn()" ) ; 
  ///
  MsgStream log( msgSvc() , Tag ) ; 
  ///
  log << MSG::ERROR 
      << " The direct invocation of method: " << name() 
      << "::BeamOn() is *NOT RECOMMENDED* and could be dangerous  within GiGa environment!" << endreq; 
  
  log << MSG::WARNING 
      << "::BeamOn() avoid usage of /run/beamOn command in your macros! " << endreq;
  
  log << MSG::WARNING 
      << " the processEvent() method will be invoked on your own risk! " << endreq ;
  ///
  StatusCode sc( StatusCode::SUCCESS ) ;
  ///
  const std::string method(" ::processTheEvent() "); 
  ///
  ___GIGA_TRY___ 
    {
      ///
      if(  !evt_Is_Processed() ) { sc  = processTheEvent() ; } 
      ///
      Assert( sc.isSuccess() , "::BeamOn() method ", sc );
      ///
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method); 
  ///
};    
///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////
void GiGaRunManager::InitializeGeometry()
{
  ///
  MsgStream log( msgSvc() , name()+"::initializeGeometry()" ) ; 
  ///
  Assert( 0 != G4RunManager::userDetector || 0 != m_rootGeo || 0 != cnvSvc() , 
          "::InitializeGeometry(), no any geometry souces are available!" );  
  ///
  G4VPhysicalVolume* root = 0; 
  if      ( 0 != m_rootGeo                  ) 
    { 
      log << MSG::INFO << " Already converted geometry will be used! " << endreq; 
      root = m_rootGeo ;   
    } 
  else if ( 0 != G4RunManager::userDetector )
    {
      log << MSG::INFO  << " Geometry will be constructed using " << objType( G4RunManager::userDetector ) << endreq;
      root = G4RunManager::userDetector->Construct() ;
      log << MSG::DEBUG << " Geometry is      constructed using " << objType( G4RunManager::userDetector ) << endreq;
    }
  else if ( 0 != cnvSvc()                  )
    {
      log << MSG::INFO  << " Geometry will be converted using " << objType( cnvSvc() ) << endreq; 
      root = cnvSvc()->G4WorldPV(); 
      log << MSG::DEBUG << " Geometry is      converted using " << objType( cnvSvc() ) << endreq; 
    }
  else   
    { log << MSG::FATAL << " There are NO ANY sources of Geometry information! "<< endreq; }
  //
  Assert( 0 != root , "::InitializeGeometry(): root is not created");
  ///
  DefineWorldVolume( root ) ; 
  ///
  G4RunManager::geometryInitialized = true;
  ///
};
///////////////////////////////////////////////////////////////////////////////////
void GiGaRunManager::Initialize()
{
  ///
  StatusCode sc (StatusCode::SUCCESS ) ; 
  ///
  const std::string Tag    ( name()+"::Initialize()"  ); 
  const std::string method ( " ::initializeKernel() " ) ; 
  ///
  ___GIGA_TRY___ 
    {
      if(  !krn_Is_Initialized() ) { sc = initializeKernel() ; }   
      Assert( sc.isSuccess() , "::Initialize() method ", sc );
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method);  
  ///
};
///////////////////////////////////////////////////////////////////////////////////
