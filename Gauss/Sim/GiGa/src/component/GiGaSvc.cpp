/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#define GIGA_GIGASVC_CPP 1 
/// ===========================================================================
/// STD & STL 
#include <string>
#include <list>
#include <vector> 
#include <algorithm> 
/// GaudiKernel
#include    "GaudiKernel/ISvcLocator.h"
#include    "GaudiKernel/IMessageSvc.h"
#include    "GaudiKernel/IChronoStatSvc.h"
#include    "GaudiKernel/IObjManager.h"
#include    "GaudiKernel/SvcFactory.h"
#include    "GaudiKernel/MsgStream.h"
#include    "GaudiKernel/ParticleProperty.h"
#include    "GaudiKernel/Bootstrap.h"
#include    "GaudiKernel/MsgStream.h"
#include    "GaudiKernel/Stat.h"
// from GiGa
#include    "GiGa/IGiGaPhysList.h"
#include    "GiGa/IGiGaPhysListFactory.h"
#include    "GiGa/IGiGaStackAction.h"
#include    "GiGa/IGiGaStackActionFactory.h"
#include    "GiGa/IGiGaTrackAction.h"
#include    "GiGa/IGiGaTrackActionFactory.h"
#include    "GiGa/IGiGaStepAction.h"
#include    "GiGa/IGiGaStepActionFactory.h"
#include    "GiGa/IGiGaEventAction.h"
#include    "GiGa/IGiGaEventActionFactory.h"
#include    "GiGa/GiGaException.h"
#include    "GiGa/GiGaRunManager.h" 
#include    "GiGa/SplitTypeAndName.h"
// local 
#include    "GiGaSvc.h"
/// visualization stuff I hope that it is temporary!
#ifdef G4VIS_USE
#include    "GiGa/GiGaVisManager.h" 
#endif ///< G4VIS_USE

/// ===========================================================================
/**  implementation of general non-inline methods from class GiGaSvc
 *   @author Vanya Belyaev
 *   @date xx/xx/xxxx
 */
/// ===========================================================================


/// factory 
static const  SvcFactory<GiGaSvc>                   s_Factory ;
extern const ISvcFactory&          GiGaSvcFactory = s_Factory ;


/// ===========================================================================
/** standard constructor 
    @param name instrance name 
    @param svc  pointer to service locator 
*/
/// ===========================================================================
GiGaSvc::GiGaSvc( const std::string& name, ISvcLocator* svcloc )
  : Service( name , svcloc )
  ///
  , m_GiGaRunManager       (   0              )
  , m_chronoSvc            (   0              ) 
  ///
  , m_objMgr               ( 0                ) 
  , m_objMgrName           ( "ApplicationMgr" )
  ///
  , m_startUIcommands      (                  )
  , m_startOfEvtUIcommands (                  )
  , m_endOfEvtUIcommands   (                  )
  , m_startOfRunUIcommands (                  )
  , m_endOfRunUIcommands   (                  )
  , m_endUIcommands        (                  )
  ///
  , m_GiGaPhysList         (                  )
  ///
  , m_GiGaStackAction      (                  )
  , m_GiGaTrackAction      (                  )
  , m_GiGaStepAction       (                  )
  , m_GiGaEventAction      (                  )
  ///
  , m_UseVisManager        ( false            )
  ///
{
  /// Geant4 commands to be executed by G4UImanager 
  declareProperty( "StartUIcommands"        , m_startUIcommands      ) ;
  declareProperty( "StartOfEventUIcommands" , m_startOfEvtUIcommands ) ;
  declareProperty( "EndOfEventUIcommands"   , m_endOfEvtUIcommands   ) ;
  declareProperty( "StartOfRunUIcommands"   , m_startOfRunUIcommands ) ;
  declareProperty( "EndOfRunUIcommands"     , m_endOfRunUIcommands   ) ;
  declareProperty( "EndUIcommands"          , m_endUIcommands        ) ;
  /// list of User Intreface sessions 
  declareProperty( "UIsessions"             , m_UIsessions           ) ;
  /// name of object manager 
  declareProperty( "ObjectManager"          , m_objMgrName           ) ; 
  /// type and name of Physics List object 
  declareProperty( "PhysicsList"            , m_GiGaPhysList         ) ;
  /// type and Name of Stacking Action object 
  declareProperty( "StackingAction"         , m_GiGaStackAction      ) ;
  /// type and Name of Tracking Action object 
  declareProperty( "TrackingAction"         , m_GiGaTrackAction      ) ;
  /// type and Name of Stepping Action object 
  declareProperty( "SteppingAction"         , m_GiGaStepAction       ) ;
  /// type and Name of Event Action object 
  declareProperty( "EventAction"            , m_GiGaEventAction      ) ;
  /// flag for creation of Visualization Manager 
  declareProperty( "UseVisManager"          , m_UseVisManager        ) ;
  ///
};

/// ===========================================================================
/// destructor
/// ===========================================================================
GiGaSvc::~GiGaSvc() 
{ 
if( 0 != m_GiGaRunManager  ) 
{ delete m_GiGaRunManager  ; m_GiGaRunManager  = 0 ; } 
};

/// ===========================================================================
/** query interface 
 *  @param   id   InterfaceID 
 *  @param   ppI  placeholder for returned interface
 *  @return status code
 */
/// ===========================================================================
StatusCode GiGaSvc::queryInterface( const IID& id , void** ppI  ) 
{
  if       ( 0 == ppI  )               
    { return StatusCode::FAILURE             ; }       ///< RETURN !!!  
  else if  ( IID_IGiGaSvc      == id )
    {  *ppI = (IGiGaSvc*)       this         ; } 
  else if  ( IID_IGiGaSetUpSvc == id ) 
    {  *ppI = (IGiGaSetUpSvc*)  this         ; } 
  else                                 
    { return Service::queryInterface(id,ppI) ; }       ///<  RETURN !!!
  ///
  addRef(); 
  ///
  return StatusCode::SUCCESS;
};

/// ===========================================================================
/** service initialization 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::initialize()
{
  /// initialize the base class 
  {
    StatusCode sc = Service::initialize(); 
    if( sc.isFailure() ) 
      { return Error("Unable to initialize the base class Service ", sc ); } 
  }
  ///
  setProperties(); 
  ///
  MsgStream log( msgSvc() , name() );
  ///
  /// locate  services, 
  {
    StatusCode sc = 
      serviceLocator()-> service( "ChronoStatSvc" , m_chronoSvc ); 
    if( sc.isFailure()   ) 
      { return Error("Unable to locate Chrono & Stat Service", sc ); } 
    if( 0 == chronoSvc() ) 
      { return Error("Unable to locate Chrono & Stat Service"     ); } 
    chronoSvc()->addRef();
  }
  ///
  {
    StatusCode sc = 
      serviceLocator()-> service( m_objMgrName , m_objMgr ); 
    if( sc.isFailure() ) 
      { return Error("Unable to locate IObjManager="+m_objMgrName, sc ); } 
    if( 0 == objMgr () ) 
      { return Error("Unable to locate IObjManager="+m_objMgrName     ); } 
    objMgr()->addRef();
  }
  ///
  {
    StatusCode sc = createGiGaRunManager(); 
    if( sc.isFailure() ) 
      { return Error("Unable to create GiGaRunManager ", sc ); }
  } 
  /// try to locate Physics List Object and make it known for GiGa 
  if( !m_GiGaPhysList.empty() )
    {
      IGiGaPhysList* PL   = 0 ;
      StatusCode sc = physList ( m_GiGaPhysList , PL );
      if( sc.isFailure() ) 
        { return Error(" Unable to instantiate Physics List Object " + 
                       m_GiGaPhysList, sc );} 
      if( 0 == PL        ) 
        { return Error(" Unable to instantiate Physics List Object " + 
                       m_GiGaPhysList     );} 
      ///
      try   { *this << PL ; } 
      catch ( const GaudiException& Excpt ) 
        { return Exception( "PhysicsList" , Excpt ) ; } 
      catch ( const std::exception& Excpt ) 
        { return Exception( "PhysicsList" , Excpt ) ; } 
      catch(...)                            
        { return Exception( "PhysicsList"         ) ; } 
      ///
      Print("Used Phisics List Object is " + 
            System::typeinfoName( typeid( *PL ) ) + "/"+PL->name() );
    }
  else { Warning(std::string("Physics List Object is not required") + 
                 " to be loaded It could be dangerous!") ; } 
  ///
  /// try to locate Stacking Action Object and make it known for GiGa 
  if( !m_GiGaStackAction.empty() )
    {
      IGiGaStackAction* SA   = 0 ;
      StatusCode sc = stackAction ( m_GiGaStackAction , SA );
      if( sc.isFailure() ) 
        { return Error(" Unable to instantiate Stacking Action Object " + 
                       m_GiGaStackAction, sc );} 
      if( 0 == SA        ) 
        { return Error(" Unable to instantiate Stacking Action Object " + 
                       m_GiGaStackAction     );} 
      ///
      try   { *this << SA ; } 
      catch ( const GaudiException& Excpt )
        { return Exception( "StackingAction" , Excpt ) ; } 
      catch ( const std::exception& Excpt ) 
        { return Exception( "StackingAction" , Excpt ) ; } 
      catch(...)                            
        { return Exception( "StackingAction"         ) ; } 
      ///
      Print("Used Stacking Action Object is " + 
            System::typeinfoName( typeid( *SA ) ) + "/"+SA->name() );
    }
  else { Warning("Stacking Action Object is not required to be loaded") ; } 
  ///
  /// try to locate Tracking Action Object and make it known for GiGa 
  if( !m_GiGaTrackAction.empty() )
    {
      IGiGaTrackAction* TA   = 0 ;
      StatusCode sc = trackAction ( m_GiGaTrackAction , TA );
      if( sc.isFailure() ) 
        { return Error(" Unable to instantiate Tracking Action Object " + 
                       m_GiGaTrackAction, sc );} 
      if( 0 == TA        ) 
        { return Error(" Unable to instantiate Tracking Action Object " + 
                       m_GiGaTrackAction     );} 
      ///
      try   { *this << TA ; } 
      catch ( const GaudiException& Excpt ) 
        { return Exception( "TrackingAction" , Excpt ) ; } 
      catch ( const std::exception& Excpt ) 
        { return Exception( "TrackingAction" , Excpt ) ; } 
      catch(...)                            
        { return Exception( "TrackingAction"         ) ; } 
      ///
      Print("Used Tracking Action Object is " + 
            System::typeinfoName( typeid( *TA ) ) + "/"+TA->name() );
    }
  else { Warning("Tracking Action Object is not required to be loaded") ; } 
  ///
  /// try to locate Stepping Action Object and make it known for GiGa 
  if( !m_GiGaStepAction.empty() )
    {
      IGiGaStepAction* SA   = 0 ;
      StatusCode sc = stepAction  ( m_GiGaStepAction , SA );
      if( sc.isFailure() ) 
        { return Error(" Unable to instantiate Stepping Action Object " + 
                       m_GiGaStepAction, sc );} 
      if( 0 == SA        ) 
        { return Error(" Unable to instantiate Stepping Action Object " + 
                       m_GiGaStepAction     );} 
      ///
      try   { *this << SA ; } 
      catch ( const GaudiException& Excpt )
        { return Exception( "SteppingAction" , Excpt ) ; } 
      catch ( const std::exception& Excpt )
        { return Exception( "SteppingAction" , Excpt ) ; } 
      catch(...)                            
        { return Exception( "SteppingAction"         ) ; } 
      ///
      Print("Used Stepping Action Object is " + 
            System::typeinfoName( typeid( *SA ) ) + "/"+SA->name() );
    }
  else { Warning("Stepping Action Object is not required to be loaded") ; } 
  ///
  /// try to locate Event    Action Object and make it known for GiGa 
  if( !m_GiGaEventAction.empty() )
    {
      IGiGaEventAction* EA   = 0 ;
      StatusCode sc = eventAction  ( m_GiGaEventAction , EA );
      if( sc.isFailure() ) 
        { return Error(" Unable to instantiate Event Action Object " + 
                       m_GiGaStepAction, sc );} 
      if( 0 == EA        ) 
        { return Error(" Unable to instantiate Event Action Object " + 
                       m_GiGaStepAction     );} 
      ///
      try   { *this << EA ; } 
      catch ( const GaudiException& Excpt ) 
        { return Exception( "EventAction" , Excpt ) ; } 
      catch ( const std::exception& Excpt ) 
        { return Exception( "EventAction" , Excpt ) ; } 
      catch(...)                            
        { return Exception( "EventAction"         ) ; } 
      ///
      Print("Used Event Action Object is " + 
            System::typeinfoName( typeid( *EA ) ) + "/"+EA->name() );
    }
  else { Warning("Event Action Object is not required to be loaded") ; } 
  ///

  /// instantiate Visualisation Manager
  if( m_UseVisManager )
    {
      ///
#ifdef G4VIS_USE
      ///
      G4VisManager* VM = new GiGaVisManager(); 
      ///
      try{ *this << VM ; } 
      catch ( const GaudiException& Excpt )
        { return Exception( "VisManager" , Excpt ) ; } 
      catch ( const std::exception& Excpt ) 
        { return Exception( "VisManager" , Excpt ) ; } 
      catch(...)                            
        { return Exception( "VisManager"         ) ; }  
      ///
      Print( "Visualization manager is created=" + 
             System::typeinfoName( typeid( *VM  ) ) );
      ///
#else 
      ///
      Warning( "Visualization Manager could not be created" + 
               " due to absebce of G4VIS_USE flag!");
      ///
#endif 
      ///
    }
  else { Warning("Visualisation Manager is not required to be created!") ; } 
  ///
  return StatusCode::SUCCESS ; 
};

/// ===========================================================================
/** service finalization  
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::finalize()
{  
  const std::string Tag( name() + ".finalize()" ) ; 
  MsgStream log( msgSvc(), name() ); 
  /// finalize Run Manager 
  StatusCode sc ( StatusCode::SUCCESS ); 
  if( 0 != m_GiGaRunManager ) { sc = m_GiGaRunManager->finalizeRunManager() ;} 
  if( sc.isFailure() ) 
    { Error(" Error in ->finalizeRunManager() method!", sc ); } 
  /// release all used services 
  if( 0 != objMgr   () ) { objMgr   ()->release() ; m_objMgr    = 0 ; } 
  if( 0 != chronoSvc() ) { chronoSvc()->release() ; m_chronoSvc = 0 ; } 
  ///  
  {  
    StatusCode sc(StatusCode::FAILURE); 
    const std::string m1("::delete RunManager  "); 
    ___GIGA_TRY___                         
      { if( 0 != m_GiGaRunManager  ) 
        { delete m_GiGaRunManager ; m_GiGaRunManager = 0 ; } }
    ___GIGA_CATCH_PRINT_AND_RETURN___(Tag,m1,msgSvc(),chronoSvc(),sc);
  }
  ///  
  return Service::finalize();
};

/// ===========================================================================
/** create GiGa Run Manager 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::createGiGaRunManager() 
{
  ///
  if( 0 != m_GiGaRunManager ) { return StatusCode::SUCCESS; }     /// RETURN !!!
  ///
  Assert( 0 == G4RunManager::GetRunManager() , 
          "There exist another instance of G4RunManager!" ) ; 
  ///
  m_GiGaRunManager = 
    new  GiGaRunManager( name()+".Mgr" , serviceLocator() ); 
  ///
  Assert( 0 != m_GiGaRunManager              , 
          " Unable to create GiGaRunManager" ) ; 
  Assert( 0 != G4RunManager::GetRunManager() , 
          " Unable to create G4RunManager"   ) ; 
  ///
  m_GiGaRunManager->set_startUIcommands      ( m_startUIcommands      ) ; 
  m_GiGaRunManager->set_startOfEvtUIcommands ( m_startOfEvtUIcommands ) ; 
  m_GiGaRunManager->set_endOfEvtUIcommands   ( m_endOfEvtUIcommands   ) ; 
  m_GiGaRunManager->set_startOfRunUIcommands ( m_startOfRunUIcommands ) ; 
  m_GiGaRunManager->set_endOfRunUIcommands   ( m_endOfRunUIcommands   ) ; 
  m_GiGaRunManager->set_endUIcommands        ( m_endUIcommands        ) ; 
  ///
  m_GiGaRunManager->set_UIsessions           ( m_UIsessions           ) ; 
  ///
  return StatusCode::SUCCESS;                                     /// RETURN !!!
  ///
};

/// ===========================================================================
/** prepare the event 
 *  @param  vertex pointer to primary vertex 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::prepareTheEvent( G4PrimaryVertex * vertex ) 
{
  const std::string Tag     ( name() + ".prepareTheEvent(G4PrimaryVertex*)" );
  const std::string method1 ( " createGiGaRunManager() " ) ; 
  const std::string method2 ( " GiGaRunManager::prepareTheEvent() " ) ; 
  MsgStream  log( msgSvc() , name() + "prepareTheEvent" ) ; 
  StatusCode sc( StatusCode::SUCCESS ) ; 
  ///
  ___GIGA_TRY___ 
    {
      if( 0 == m_GiGaRunManager )
        { 
          sc = createGiGaRunManager() ;   
          Assert( sc.isSuccess()        , 
                  " prepareTheEvent(): failure from createGiGaRunManager " , 
                  sc ) ; 
          Assert( 0 != m_GiGaRunManager ,
                  " prepareTheEvent(): unable to create GiGaRunManager   ") ; 
        }
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method1) ; 
  ///
  ___GIGA_TRY___ 
    {
      sc = m_GiGaRunManager->prepareTheEvent( vertex ) ; 
      Assert( sc.isSuccess() , "preparetheEvent() failure", sc ) ; 
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method2) ; 
  ///
  return StatusCode::SUCCESS; 
  ///
};

/// ===========================================================================
/** retrieve the event 
 *  @param  event pointer to event 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::retrieveTheEvent( const G4Event*& event) 
{
  const std::string Tag     ( name() + ".retrieveTheEvent(const G4Event*&)" );
  const std::string method1 ( " createGiGaRunManager() " ) ; 
  const std::string method2 ( " GiGaRunManager::retrieveTheEvent() " ) ; 
  StatusCode sc( StatusCode::SUCCESS ) ; 
  MsgStream  log( msgSvc() , name() + ".retrieveTheEvent" ) ; 
  ___GIGA_TRY___ 
    { 
      if( 0 == m_GiGaRunManager )
        { 
          sc = createGiGaRunManager() ;   
          Assert( sc.isSuccess()        , std::string(" retrieveTheEvent:") + 
                  " failure from createGiGaRunManager " , sc     ) ; 
          Assert( 0 != m_GiGaRunManager , std::string(" retrieveTheEvent:") + 
                  " unable to create GiGaRunManager   "          ) ; 
        }
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method1) ; 
  ///
  ___GIGA_TRY___ 
    { 
      sc = m_GiGaRunManager->retrieveTheEvent( event ) ; 
      Assert( sc.isSuccess() , " retrieveTheEvent failure" ) ; 
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method2) ; 
  ///
  return StatusCode::SUCCESS; 
  ///
};

/// ===========================================================================
/** instantiate new physics list object using abstract factory technique 
 *  @param TypeName    "Type/Name" of physics list object 
 *  @param PL           reference to new phisics list object 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::physList( const std::string& TypeName , 
                              IGiGaPhysList*&    PL       )
{
  PL = 0 ; /// reset output value 
  if( 0 == objMgr()  ) 
    { return Error("RetrievePhysicsList:  IObjManager* points to NULL"); }
  std::string Type , Name ; 
  StatusCode sc = SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("RetrievePhysicsList: Physics List Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// locate the factory
  const IGiGaPhysListFactory* PLF = 0 ;  
  {
    bool exist = objMgr()->existsObjFactory( Type ); 
    if( !exist   ) 
      { return Error("RetrievePhysicsList:  Factory  for " + 
                     Type+" is not located") ; }  
    const IFactory* fac   = objMgr()->objFactory( Type );
    if( 0 == fac ) 
      { return Error("RetrievePhisicsList: IFactory* for " + 
                     Type+" points to NULL" ); }
    PLF = dynamic_cast<const IGiGaPhysListFactory*> ( fac ); 
    if( 0 == PLF ) 
      { return Error("RetrievePhysicsList: IGiGaPhysListFactory* for " + 
                     Type+" points to NULL" );}
  }
  ///
  PL = PLF->instantiate( Name , serviceLocator() ) ; 
  if( 0 == PL    ) 
    { return Error(std::string("RetrievePhysicsList: could not instantiate") + 
                   " IGiGaPhysList* Object "+Type+"/"+Name );} 
  ///
  PL->addRef(); 
  if( PL->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  PL->release(); delete PL ; PL = 0 ;  
  ///
  return Error(std::string("RetrievePhysicsList: could not initialize ") + 
               "IGiGaPhysList* Object "+Type+"/"+Name, sc) ;
  ///
};

/// ===========================================================================
/** instantiate new stacking action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of stacking action object 
 *  @param SA           reference to new stacking action object 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::stackAction( const std::string& TypeName , 
                                 IGiGaStackAction*& SA       )
{
  SA = 0 ; /// reset output value 
  if( 0 == objMgr()  ) 
    { return Error("RetrieveStackAction:  IObjManager* points to NULL"); }
  std::string Type , Name ; 
  StatusCode sc = SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("RetrieveStackAction: Stack Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// locate the factory
  const IGiGaStackActionFactory* SAF = 0 ;  
  {
    bool exist = objMgr()->existsObjFactory( Type ); 
    if( !exist   )
      { return Error("RetrieveStackAction:  Factory  for " + 
                     Type+" is not located") ; }  
    const IFactory* fac   = objMgr()->objFactory( Type );
    if( 0 == fac )
      { return Error("RetrieveStackAction: IFactory* for " + 
                     Type + " points to NULL" ); }
    SAF = dynamic_cast<const IGiGaStackActionFactory*> ( fac ); 
    if( 0 == SAF )
      { return Error("RetrieveStackAction: IGiGaStackActionFactory* for " + 
                     Type+" points to NULL" );}
  }
  ///
  SA = SAF->instantiate( Name , serviceLocator() ) ; 
  if( 0 == SA    ) 
    { return Error(std::string("RetrieveStackAction: could not instantiate") 
                   + " IGiGaStackAction* Object "+Type+"/"+Name );} 
  ///
  SA->addRef(); 
  if( SA->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  SA->release(); delete SA ; SA = 0 ;  
  ///
  return Error(std::string("RetrieveStackAction: could not initialize ") + 
               "IGiGaStackAction* Object "+Type+"/"+Name, sc) ;
  ///
};

/// ===========================================================================
/** instantiate new tracking action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of tracking action object 
 *  @param TA           reference to new tracking action object 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::trackAction( const std::string& TypeName , 
                                 IGiGaTrackAction*& TA       )
{
  TA = 0 ; /// reset output value 
  if( 0 == objMgr()  ) 
    { return Error("RetrieveTrackAction:  IObjManager* points to NULL"); }
  std::string Type , Name ; 
  StatusCode sc = SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("RetrieveTrackAction: Track Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// locate the factory
  const IGiGaTrackActionFactory* TAF = 0 ;  
  {
    bool exist = objMgr()->existsObjFactory( Type ); 
    if( !exist   ) 
      { return Error("RetrieveTrackAction:  Factory  for " + 
                     Type + " is not located") ; }  
    const IFactory* fac   = objMgr()->objFactory( Type );
    if( 0 == fac ) 
      { return Error("RetrieveTrackAction: IFactory* for " + 
                     Type+" points to NULL" ); }
    TAF = dynamic_cast<const IGiGaTrackActionFactory*> ( fac ); 
    if( 0 == TAF ) 
      { return Error("RetrieveTrackAction: IGiGaTrackActionFactory* for " + 
                     Type+" points to NULL" );}
  }
  ///
  TA = TAF->instantiate( Name , serviceLocator() ) ; 
  if( 0 == TA    ) 
    { return Error(std::string("RetrieveTrackAction: could not instantiate ") 
                   + "IGiGaTrackAction* Object "+Type+"/"+Name );} 
  ///
  TA->addRef(); 
  if( TA->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  TA->release(); delete TA ; TA = 0 ;  
  ///
  return Error(std::string("RetrieveTrackAction: could not initialize ") +
               " IGiGaTrackAction* Object "+Type+"/"+Name, sc) ;
  ///
};

/// ===========================================================================
/** instantiate new stepping action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of stepping action object 
 *  @param SA           reference to new stepping action object 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::stepAction( const std::string& TypeName , 
                                IGiGaStepAction*&  SA       )
{
  SA = 0 ; /// reset output value 
  if( 0 == objMgr()  ) 
    { return Error("RetrieveStepAction:  IObjManager* points to NULL"); }
  std::string Type , Name ; 
  StatusCode sc = SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("RetrieveStepAction: Stepping Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// locate the factory
  const IGiGaStepActionFactory* SAF = 0 ;  
  {
    bool exist = objMgr()->existsObjFactory( Type ); 
    if( !exist   ) 
      { return Error("RetrieveStepAction:  Factory  for " + 
                     Type+" is not located") ; }  
    const IFactory* fac   = objMgr()->objFactory( Type );
    if( 0 == fac )
      { return Error("RetrieveStepAction: IFactory* for " + 
                     Type+" points to NULL" ); }
    SAF = dynamic_cast<const IGiGaStepActionFactory*> ( fac ); 
    if( 0 == SAF ) 
      { return Error("RetrieveStepAction: IGiGaStepActionFactory* for " + 
                     Type+" points to NULL" );}
  }
  ///
  SA = SAF->instantiate( Name , serviceLocator() ) ; 
  if( 0 == SA    ) 
    { return Error(std::string("RetrieveStepAction: could not instantiate ") +
                   "IGiGaStepAction* Object "+Type+"/"+Name );} 
  ///
  SA->addRef(); 
  if( SA->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  SA->release(); delete SA ; SA = 0 ;  
  ///
  return Error(std::string("RetrieveStepAction: could not initialize ") + 
               "IGiGaStepAction* Object "+Type+"/"+Name, sc) ;
  ///
};

/// ===========================================================================
/** instantiate new event action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of event  action object 
 *  @param EA           reference to new event action object 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSvc::eventAction( const std::string& TypeName , 
                                 IGiGaEventAction*& EA       )
{
  EA = 0 ; /// reset output value 
  if( 0 == objMgr()  ) 
    { return Error("RetrieveEventAction:  IObjManager* points to NULL"); }
  std::string Type , Name ; 
  StatusCode sc = SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("RetrieveEventAction: Stepping Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// locate the factory
  const IGiGaEventActionFactory* EAF = 0 ;  
  {
    bool exist = objMgr()->existsObjFactory( Type ); 
    if( !exist   )
      { return Error("RetrieveEventAction:  Factory  for " + 
                     Type+" is not located") ; }  
    const IFactory* fac   = objMgr()->objFactory( Type );
    if( 0 == fac )
      { return Error("RetrieveEventAction: IFactory* for " + 
                     Type+" points to NULL" ); }
    EAF = dynamic_cast<const IGiGaEventActionFactory*> ( fac ); 
    if( 0 == EAF )
      { return Error(std::string("RetrieveEventAction:")+
                     " IGiGaEventActionFactory* for " + 
                     Type+" points to NULL" );}
  }
  ///
  EA = EAF->instantiate( Name , serviceLocator() ) ; 
  if( 0 == EA    ) 
    { return Error(std::string("RetrieveEventAction: could not instantiate ") + 
                   "IGiGaEventAction* Object "+Type+"/"+Name );} 
  ///
  EA->addRef(); 
  if( EA->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  EA->release(); delete EA ; EA = 0 ;  
  ///
  return Error(std::string("RetrieveEventAction: could not initialize ") + 
               "IGiGaEventAction* Object "+Type+"/"+Name, sc) ;
  ///
};

/// ===========================================================================























