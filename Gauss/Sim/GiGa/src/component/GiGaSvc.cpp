// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.10  2001/08/12 15:42:54  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.9  2001/08/01 09:42:24  ibelyaev
/// redesign and reimplementation of GiGaRunManager class
///
/// Revision 1.8  2001/07/27 14:29:01  ibelyaev
/// bug fix
///
/// Revision 1.7  2001/07/25 17:18:09  ibelyaev
/// move all conversions from GiGa to GiGaCnv
///
/// Revision 1.6  2001/07/23 20:53:47  ibelyaev
/// reorganization of GiGaUtil namespace
///
/// Revision 1.5  2001/07/15 20:54:35  ibelyaev
/// package restructurisation
/// 
// ============================================================================
#define GIGA_GIGASVC_CPP 1 
// ============================================================================
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
/// G4 
#include    "G4UIsession.hh"
#include     "G4VVisManager.hh"
// from GiGa
#include    "GiGa/IGiGaPhysList.h"
#include    "GiGa/IGiGaStackAction.h"
#include    "GiGa/IGiGaTrackAction.h"
#include    "GiGa/IGiGaStepAction.h"
#include    "GiGa/IGiGaEventAction.h"
#include    "GiGa/IGiGaRunAction.h"
#include    "GiGa/IGiGaRunManager.h" 
#include    "GiGa/IGiGaGeoSrc.h" 
#include    "GiGa/GiGaException.h"
#include    "GiGa/GiGaUtil.h"
#include    "GiGa/GiGa.h"
// local 
#include    "GiGaSvc.h"

// ============================================================================
/**  implementation of general non-inline methods from class GiGaSvc
 *   @author Vanya Belyaev
 *   @date xx/xx/xxxx
 */
// ============================================================================


/// factory 
static const  SvcFactory<GiGaSvc>                   s_Factory ;
extern const ISvcFactory&          GiGaSvcFactory = s_Factory ;


// ============================================================================
/** standard constructor 
    @param name instrance name 
    @param svc  pointer to service locator 
*/
// ============================================================================
GiGaSvc::GiGaSvc( const std::string& name, ISvcLocator* svcloc )
  : Service( name , svcloc )
  ///
  , m_runMgr               (   0              )
  , m_visMgr               (   0              )
  , m_chronoSvc            (   0              ) 
  ///
  , m_objMgr               ( 0                ) 
  , m_geoSrc               ( 0                ) 
  ///
  , m_objMgrName           ( "ApplicationMgr" )
  , m_geoSrcName           ( "GiGaGeomCnvSvc" )
  , m_runMgrName           ( "GiGaMgr"        )
  ///
  , m_GiGaPhysList         (                  )
  ///
  , m_GiGaStackAction      (                  )
  , m_GiGaTrackAction      (                  )
  , m_GiGaStepAction       (                  )
  , m_GiGaEventAction      (                  )
  , m_GiGaRunAction        (                  )
  ///
  , m_UseVisManager        ( false            )
  ///
{
  /// list of User Intreface sessions 
  declareProperty( "UIsessions"             , m_UIsessions           ) ;
  /// name of object manager 
  declareProperty( "ObjectManager"          , m_objMgrName           ) ; 
  /// name of geometry source 
  declareProperty( "GeometrySource"         , m_geoSrcName           ) ; 
  /// name of runmanager  
  declareProperty( "RunManager"             , m_runMgrName           ) ; 
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
  /// type and Name of Run Action object 
  declareProperty( "RunAction"              , m_GiGaRunAction        ) ;
  /// flag for creation of Visualization Manager 
  declareProperty( "UseVisManager"          , m_UseVisManager        ) ;
  ///
};

// ============================================================================
/// destructor
// ============================================================================
GiGaSvc::~GiGaSvc() 
{ 
  if( 0 != runMgr()  ) 
    { 
      runMgr()->release() ; 
      delete m_runMgr ; 
      m_runMgr  = 0 ; 
    } 
  if( 0 != visMgr() ) { delete m_visMgr ; m_visMgr = 0 ; }
};

// ============================================================================
/** query interface 
 *  @param   id   InterfaceID 
 *  @param   ppI  placeholder for returned interface
 *  @return status code
 */
// ============================================================================
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

// ============================================================================
/** service initialization 
 *  @return status code 
 */
// ============================================================================
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
  if( 0 == svcLoc() ) { return Error("ISvcLocator* points to NULL!"); }
  
  /// locate  services, 
  {
    StatusCode sc = svcLoc()->service( "ChronoStatSvc" , m_chronoSvc ); 
    if( sc.isFailure()   ) 
      { return Error("Unable to locate Chrono & Stat Service", sc ); } 
    if( 0 == chronoSvc() ) 
      { return Error("Unable to locate Chrono & Stat Service"     ); } 
    chronoSvc()->addRef();
  }
  ///
  {
    StatusCode sc = svcLoc()->service( m_objMgrName , m_objMgr ); 
    if( sc.isFailure() ) 
      { return Error("Unable to locate IObjManager="+m_objMgrName, sc ); } 
    if( 0 == objMgr () ) 
      { return Error("IObjManager* points to NULL  "+m_objMgrName     ); } 
    objMgr()->addRef();
  }
  ///
  {
    StatusCode sc = createGiGaRunManager(); 
    if( sc.isFailure() ) 
      { return Error("Unable to create GiGaRunManager ", sc ); }
    if( 0 == runMgr () ) 
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
            GiGaUtil::ObjTypeName( PL ) + "/"+PL->name() );
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
            GiGaUtil::ObjTypeName( SA ) + "/"+SA->name() );
    }
  else { Print("Stacking Action Object is not required to be loaded") ; } 
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
            GiGaUtil::ObjTypeName( TA ) + "/"+TA->name() );
    }
  else { Print("Tracking Action Object is not required to be loaded") ; } 
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
            GiGaUtil::ObjTypeName( SA ) + "/"+SA->name() );
    }
  else { Print("Stepping Action Object is not required to be loaded") ; } 
  ///
  /// try to locate Event    Action Object and make it known for GiGa 
  if( !m_GiGaEventAction.empty() )
    {
      IGiGaEventAction* EA   = 0 ;
      StatusCode sc = eventAction  ( m_GiGaEventAction , EA );
      if( sc.isFailure() ) 
        { return Error(" Unable to instantiate Event Action Object " + 
                       m_GiGaEventAction, sc );} 
      if( 0 == EA        ) 
        { return Error(" Unable to instantiate Event Action Object " + 
                       m_GiGaEventAction     );} 
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
            GiGaUtil::ObjTypeName( EA ) + "/"+EA->name() );
    }
  else { Print("Event Action Object is not required to be loaded") ; } 
  ///
  /// try to locate Run Action Object and make it known for GiGa 
  if( !m_GiGaRunAction.empty() )
    {
      IGiGaRunAction* RA   = 0 ;
      StatusCode sc = runAction  ( m_GiGaRunAction , RA );
      if( sc.isFailure() ) 
        { return Error(" Unable to instantiate Run Action Object " + 
                       m_GiGaRunAction, sc );} 
      if( 0 == RA        ) 
        { return Error(" Unable to instantiate Run Action Object " + 
                       m_GiGaRunAction     );} 
      ///
      try   { *this << RA ; } 
      catch ( const GaudiException& Excpt ) 
        { return Exception( "runAction" , Excpt ) ; } 
      catch ( const std::exception& Excpt ) 
        { return Exception( "runAction" , Excpt ) ; } 
      catch(...)                            
        { return Exception( "runAction"         ) ; } 
      ///
      Print("Used Run Action Object is " + 
            GiGaUtil::ObjTypeName( RA ) + "/"+RA->name() );
    }
  else { Print("Run Action Object is not required to be loaded") ; }  

  /// try to locate GiGa Geometry source  and make it known for GiGa 
  if( !m_geoSrcName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_geoSrcName , m_geoSrc ); 
      if( sc.isFailure()   ) 
        { return Error("Unable to locate GiGa Geometry Source='" + 
                       m_geoSrcName + "'", sc ); } 
      if( 0 == geoSrc() ) 
        { return Error("Unable to locate GiGa Geometry Source='" + 
                       m_geoSrcName + "'"     ); } 
      geoSrc()->addRef();
      if( 0 != runMgr() ){ runMgr()->declare( geoSrc() ); }
    }
  else { Print("GiGa Geometry Source is not required to be loaded") ; } 
  ///
  
  /// UI session business 
  if( !m_UIsessions.empty() )
    {
      G4UIsession* session = 0 ;
      for( std::vector<std::string>::const_iterator iSes = 
             m_UIsessions.begin() ; m_UIsessions.end() != iSes ; ++iSes )
        {
          session = GiGa::createUIsession( *iSes );
          if( 0 != session )
            { 
              Print(" UI session created of type '" +
                    GiGaUtil::ObjTypeName( session) );
              runMgr()->declare( session );
              break;
            }
        }
    }
  /// instantiate Visualisation Manager
  if( m_UseVisManager )
    {
      ///
      if( 0 != G4VVisManager::GetConcreteInstance() )
        { Error("Visuaalization manager is already instantiated!");}
      G4VVisManager* m_visMgr = GiGa::createVisManager() ;
      if( 0 == m_visMgr ) 
        { Error("GiGa VisualisationManager is not created!");}
      ///
    }
  else { Warning("Visualisation Manager is not required to be created!") ; } 
  ///
  return StatusCode::SUCCESS ; 
};

// ============================================================================
/** service finalization  
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::finalize()
{  
  Print("finalization");
  /// finalize Run Manager 
  StatusCode sc ( StatusCode::SUCCESS ); 
  if( 0 != runMgr() ) { sc = runMgr()->finalize() ;} 
  if( sc.isFailure() ) 
    { Error("coudlnot finalize RunManager", sc ); } 
  /// release all used services 
  if( 0 != objMgr   () ) { objMgr   ()->release() ; m_objMgr    = 0 ; } 
  if( 0 != chronoSvc() ) { chronoSvc()->release() ; m_chronoSvc = 0 ; } 
  if( 0 != geoSrc   () ) { geoSrc   ()->release() ; m_geoSrc    = 0 ; } 
  /// 
  try
    {
      if( 0 != runMgr()  )
        { 
          runMgr()->release  ()   ;
          delete m_runMgr ;
          m_runMgr  = 0   ;
        }
    }
  catch ( const GaudiException& Excpt ) 
    { return Exception( "finalize" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { return Exception( "finalize" , Excpt ) ; } 
  catch(...)                            
    { return Exception( "finalize"         ) ; }
  ///  finalize the base class 
  return Service::finalize();
};

// ============================================================================
/** create GiGa Run Manager 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::createGiGaRunManager() 
{
  ///
  if( 0 != runMgr() ) { return StatusCode::SUCCESS; }     /// RETURN !!!
  ///
  m_runMgr = GiGa::createRunManager( m_runMgrName , serviceLocator() ); 
  ///
  if( 0 == runMgr() ) { return Error(" Unable to create GiGaRunManager"); }
  ///  
  return StatusCode::SUCCESS;                                     /// RETURN !!!
  ///
};

// ============================================================================
/** prepare the event 
 *  @param  vertex pointer to primary vertex 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::prepareTheEvent( G4PrimaryVertex * vertex ) 
{
  try
    { 
      StatusCode sc( StatusCode::SUCCESS ) ; 
      if( 0 == runMgr () ) { sc = createGiGaRunManager()     ; }
      if( sc.isFailure() ) { Exception("prepareTheEvent()" ) ; }
     sc = runMgr()->prepareTheEvent( vertex ) ; 
      if( sc.isFailure() ) { Exception("prepareTheEvent()" ) ; } 
    }
  catch ( const GaudiException& Excpt ) 
    { return Exception( "prepareTheEvent()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { return Exception( "prepareTheEvent()" , Excpt ) ; } 
  catch(...)                            
    { return Exception( "prepareTheEvent()"         ) ; }
  ///
  return StatusCode::SUCCESS; 
  ///
};

// ============================================================================
/** retrieve the event 
 *  @param  event pointer to event 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::retrieveTheEvent( const G4Event*& event) 
{
  try
    { 
      StatusCode sc( StatusCode::SUCCESS ) ; 
      if( 0 == runMgr () ) { sc = createGiGaRunManager()      ; }
      if( sc.isFailure() ) { Exception("retrieveTheEvent()" ) ; }
      sc = runMgr()->retrieveTheEvent( event ) ; 
      if( sc.isFailure() ) { Exception("retrieveTheEvent()" ) ; }
    }
  catch ( const GaudiException& Excpt ) 
    { return Exception( "retrieveTheEvent()" , Excpt ) ; } 
  catch ( const std::exception& Excpt ) 
    { return Exception( "retrieveTheEvent()" , Excpt ) ; } 
  catch(...)                            
    { return Exception( "retrieveTheEvent()"         ) ; }
  ///
  return StatusCode::SUCCESS; 
  ///
};

// ============================================================================
/** instantiate new physics list object using abstract factory technique 
 *  @param TypeName    "Type/Name" of physics list object 
 *  @param PL           reference to new phisics list object 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::physList( const std::string& TypeName , 
                              IGiGaPhysList*&    PL       )
{
  PL = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = 
    GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("physicsList(): Physics List Type/Name=" + 
                   TypeName+" is unresolved!",sc) ; }
  /// create the creator 
  GiGaUtil::PhysListCreator creator( objMgr()  , serviceLocator() );
  /// create the object 
  PL = creator( Type , Name );
  if( 0 == PL    ) 
    { return Error(std::string("physicsList(): could not instantiate") + 
                   " IGiGaPhysList* Object "+Type+"/"+Name );} 
  ///
  PL->addRef(); 
  if( PL->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  PL->release(); delete PL ; PL = 0 ;  
  ///
  return Error(std::string("physicsList(): could not initialize ") + 
               "IGiGaPhysList* Object "+Type+"/"+Name, sc) ;
  ///
};

// ============================================================================
/** instantiate new stacking action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of stacking action object 
 *  @param SA           reference to new stacking action object 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::stackAction( const std::string& TypeName , 
                                 IGiGaStackAction*& SA       )
{
  SA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = 
    GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("stackAction(): Stack Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// Create the creator
  GiGaUtil::StackActionCreator creator( objMgr() , serviceLocator() );
  /// creat the object 
  SA = creator( Type , Name ) ;
  if( 0 == SA    ) 
    { return Error(std::string("stackAction(): could not instantiate") 
                   + " IGiGaStackAction* Object "+Type+"/"+Name );} 
  ///
  SA->addRef(); 
  if( SA->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  SA->release(); delete SA ; SA = 0 ;  
  ///
  return Error(std::string("stackAction(): could not initialize ") + 
               "IGiGaStackAction* Object "+Type+"/"+Name, sc) ;
  ///
};

// ============================================================================
/** instantiate new tracking action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of tracking action object 
 *  @param TA           reference to new tracking action object 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::trackAction( const std::string& TypeName , 
                                 IGiGaTrackAction*& TA       )
{
  TA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = 
    GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("trackAction(): Track Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// create the creator 
  GiGaUtil::TrackActionCreator creator( objMgr() , serviceLocator() );
  /// create the object 
  TA = creator( Type , Name );
  if( 0 == TA    ) 
    { return Error(std::string("trackAction(): could not instantiate ") 
                   + "IGiGaTrackAction* Object "+Type+"/"+Name );} 
  ///
  TA->addRef(); 
  if( TA->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  TA->release(); delete TA ; TA = 0 ;  
  ///
  return Error(std::string("trackAction(): could not initialize ") +
               " IGiGaTrackAction* Object "+Type+"/"+Name, sc) ;
  ///
};

// ============================================================================
/** instantiate new stepping action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of stepping action object 
 *  @param SA           reference to new stepping action object 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::stepAction( const std::string& TypeName , 
                                IGiGaStepAction*&  SA       )
{
  SA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = 
    GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("stepAction(): Stepping Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// create the creator 
  GiGaUtil::StepActionCreator creator( objMgr() , serviceLocator() );
  /// create the object
  SA = creator( Type , Name );
  if( 0 == SA    ) 
    { return Error(std::string("stepAction(): could not instantiate ") +
                   "IGiGaStepAction* Object "+Type+"/"+Name );} 
  ///
  SA->addRef(); 
  if( SA->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  SA->release(); delete SA ; SA = 0 ;  
  ///
  return Error(std::string("stepAction(): could not initialize ") + 
               "IGiGaStepAction* Object "+Type+"/"+Name, sc) ;
  ///
};

// ============================================================================
/** instantiate new event action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of event  action object 
 *  @param EA           reference to new event action object 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::eventAction( const std::string& TypeName , 
                                 IGiGaEventAction*& EA       )
{
  EA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = 
    GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("eventAction(): Event Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// create the creator 
  GiGaUtil::EventActionCreator creator( objMgr() , serviceLocator() );
  /// create the object 
  EA = creator( Type , Name ) ;
  if( 0 == EA    ) 
    { return Error(std::string("eventAction(): could not instantiate ") + 
                   "IGiGaEventAction* Object "+Type+"/"+Name );} 
  ///
  EA->addRef(); 
  if( EA->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  EA->release(); delete EA ; EA = 0 ;  
  ///
  return Error(std::string("eventAction(): could not initialize ") + 
               "IGiGaEventAction* Object "+Type+"/"+Name, sc) ;
  ///
};

// ============================================================================
/** instantiate new run action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of run  action object 
 *  @param RA           reference to new run action object 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::runAction( const std::string& TypeName , 
                               IGiGaRunAction*& RA       )
{
  RA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = 
    GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("runAction(): Run Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  /// create the creator 
  GiGaUtil::RunActionCreator creator( objMgr() , serviceLocator() );
  /// create the object 
  RA = creator( Type , Name ) ;
  if( 0 == RA    ) 
    { return Error(std::string("runAction(): could not instantiate ") + 
                   "IGiGaRunAction* Object "+Type+"/"+Name );} 
  ///
  RA->addRef(); 
  if( RA->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  RA->release(); delete RA ; RA = 0 ;  
  ///
  return Error(std::string("runAction(): could not initialize ") + 
               "IGiGaRunAction* Object "+Type+"/"+Name, sc) ;
  ///
};

// ============================================================================























