// $Id: GiGaSvc.cpp,v 1.14 2002-05-07 12:21:36 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
#include    "GaudiKernel/IToolSvc.h"
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
/**  @file 
 *   implementation of general non-inline methods from class GiGaSvc
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
  , m_toolSvc              (   0              ) 
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
  , m_errors     ()
  , m_warnings   ()
  , m_exceptions ()
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
{ if( 0 != visMgr() ) { delete m_visMgr ; m_visMgr = 0 ; } };

// ============================================================================
/** query interface 
 *  @param   id   InterfaceID 
 *  @param   ppI  placeholder for returned interface
 *  @return status code
 */
// ============================================================================
StatusCode 
GiGaSvc::queryInterface
( const IID& id , void** ppI  ) 
{
  if       ( 0 == ppI  )               
    { return StatusCode::FAILURE                    ; } // RETURN !!!  
  else if  ( IGiGaSvc::interfaceID()      == id   )
    {  *ppI = static_cast<IGiGaSvc*>      ( this  ) ; } 
  else if  ( IGiGaSetUpSvc::interfaceID() == id   ) 
    {  *ppI = static_cast<IGiGaSetUpSvc*> ( this  ) ; } 
  else { return Service::queryInterface( id , ppI ) ; } //  RETURN !!!
  //
  addRef(); 
  //
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** service initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode 
GiGaSvc::initialize()
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
  { /// print ALL properties 
    typedef std::vector<Property*> Properties;
    const Properties& properties = getProperties() ;
    MsgStream log( msgSvc() , name ()  );
    log << MSG::DEBUG 
        << " List of ALL properties of "
        << System::typeinfoName( typeid( *this ) ) << "/" 
        << name ()           << "   #properties = " 
        << properties.size() << endreq ;
    const int   buffer_size  = 256 ;
    char buffer[buffer_size]       ;
    for( Properties::const_reverse_iterator property 
           = properties.rbegin() ;
         properties.rend() != property ; ++property )  
      {
        std::fill( buffer , buffer + buffer_size , 0 );
        std::ostrstream ost ( buffer , buffer_size );
        (*property)->nameAndValueAsStream( ost );
        ost.freeze();
        log << MSG::DEBUG
            << "Property ['Name': Value] = " 
            << ost.str() << endreq ;
      }
  }  
  // locate  services, 
  if( 0 == svcLoc() ) { return Error("ISvcLocator* points to NULL!"); }
  //
  {
    StatusCode sc = 
      svcLoc()->service( "ChronoStatSvc" , m_chronoSvc , true ); 
    if( sc.isFailure()   ) 
      { return Error("Unable to locate Chrono & Stat Service", sc ); } 
    if( 0 == chronoSvc() ) 
      { return Error("Unable to locate Chrono & Stat Service"     ); } 
    chronoSvc()->addRef();
  }
  //
  {
    StatusCode sc = 
      svcLoc()->service( "ToolSvc" , m_toolSvc , true ); 
    if( sc.isFailure()   ) 
      { return Error("Unable to locate Tool Service", sc ); } 
    if( 0 == toolSvc() ) 
      { return Error("Unable to locate Tool Service"     ); } 
    toolSvc()->addRef();
  }
  //
  {
    StatusCode sc = 
      svcLoc()->service( m_objMgrName , m_objMgr ); 
    if( sc.isFailure() ) 
      { return Error("Unable to locate IObjManager="+m_objMgrName, sc ); } 
    if( 0 == objMgr () ) 
      { return Error("IObjManager* points to NULL  "+m_objMgrName     ); } 
    objMgr()->addRef();
  }
  //
  { // get GiGa Run Manager
    StatusCode sc = retrieveRunManager() ;
    if( sc.isFailure() ) 
      { return Error("Unable to create GiGaRunManager ", sc ); }
    if( 0 == runMgr () ) 
      { return Error("Unable to create GiGaRunManager ", sc ); }
  } 
  // try to locate Physics List Object and make it known for GiGa 
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
  // try to locate Stacking Action Object and make it known for GiGa 
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
  // try to locate Tracking Action Object and make it known for GiGa 
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
  // try to locate Stepping Action Object and make it known for GiGa 
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
  // try to locate Event    Action Object and make it known for GiGa 
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
  // try to locate Run Action Object and make it known for GiGa 
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
  // try to locate GiGa Geometry source  and make it known for GiGa 
  if( !m_geoSrcName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_geoSrcName , m_geoSrc , true ); 
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
  // UI session business 
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
  // instantiate Visualisation Manager
  if( m_UseVisManager )
    {
      ///
      if( 0 != G4VVisManager::GetConcreteInstance() )
        { Error("Visuaalization manager is already instantiated!");}
      G4VVisManager* m_visMgr = GiGa::createVisManager() ;
      if( 0 == m_visMgr ) 
        { Error("GiGa VisualisationManager is not created!");}
      //
    }
  else { Warning("Visualisation Manager is not required to be created!") ; } 
  //  
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
  Print("Finalization" , MSG::DEBUG , StatusCode::SUCCESS );
  // release Run Manager 
  StatusCode sc ( StatusCode::SUCCESS ); 
  if( 0 != runMgr   () ) { runMgr   ()->release() ; m_runMgr    = 0 ; } 
  // release all used services 
  if( 0 != objMgr   () ) { objMgr   ()->release() ; m_objMgr    = 0 ; } 
  if( 0 != toolSvc  () ) { toolSvc  ()->release() ; m_toolSvc   = 0 ; } 
  if( 0 != chronoSvc() ) { chronoSvc()->release() ; m_chronoSvc = 0 ; } 
  if( 0 != geoSrc   () ) { geoSrc   ()->release() ; m_geoSrc    = 0 ; } 
  // error printout 
  if( 0 != m_errors     .size() || 
      0 != m_warnings   .size() || 
      0 != m_exceptions .size()   ) 
    {      
      MsgStream log( msgSvc() , name() );
      // format printout 
      log << MSG::ALWAYS 
          << " Exceptions/Errors/Warnings statistics:  " 
          << m_exceptions .size () << "/"
          << m_errors     .size () << "/"
          << m_warnings   .size () << endreq ; 
      // print exceptions counter 
      for( Counter::const_iterator excp = m_exceptions.begin() ;
           excp != m_exceptions.end() ; ++excp )
        {
          log << MSG::ALWAYS 
              << " #EXCEPTIONS= " << excp->second  
              << " Message='"     << excp->first    << "'" << endreq ; 
        }  
      // print errors counter 
      for( Counter::const_iterator error = m_errors.begin() ;
           error != m_errors.end() ; ++error )
        {
          log << MSG::ALWAYS 
              << " #ERRORS    = " << error->second  
              << " Message='"     << error->first    << "'" << endreq ; 
        }  
      // print warnings
      for( Counter::const_iterator warning = m_warnings.begin() ;
           warning != m_warnings.end() ; ++warning )
        {
          log << MSG::ALWAYS 
              << " #WARNINGS  = " << warning->second  
              << " Message='"     << warning->first  << "'" << endreq ; 
        }  
    }
  m_errors      .clear();
  m_warnings    .clear();
  m_exceptions  .clear();
  ///  finalize the base class 
  return Service::finalize();
};
// ============================================================================

// ============================================================================
/** prepare the event 
 *  @param  vertex pointer to primary vertex 
 *  @return status code 
 */
// ============================================================================
StatusCode 
GiGaSvc::prepareTheEvent
( G4PrimaryVertex * vertex ) 
{
  try
    { 
      StatusCode sc( StatusCode::SUCCESS ) ; 
      if( 0 == runMgr () ) { sc = retrieveRunManager()       ; }
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

// ============================================================================
/** retrieve the event 
 *  @param  event pointer to event 
 *  @return status code 
 */
// ============================================================================
StatusCode 
GiGaSvc::retrieveTheEvent
( const G4Event*& event) 
{
  try
    { 
      StatusCode sc( StatusCode::SUCCESS ) ; 
      if( 0 == runMgr () ) { sc = retrieveRunManager()       ; }
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

// ============================================================================
/** retrieve IGiGaRunManager 
 *  @see IGiGaRunManager
 *  @return status code
 */
// ============================================================================
StatusCode GiGaSvc::retrieveRunManager() 
{
  if( 0 != runMgr  () ) { return StatusCode::SUCCESS ; }
  if( 0 == toolSvc () )
    { return Error("retrieveRunManager():: IToolSvc* points to NULL!"); }
  StatusCode sc = 
    toolSvc()->
    retrieveTool( "GiGaRunManager" , m_runMgrName , m_runMgr , this );
  if( sc.isFailure() ) 
    { return Error("Unable to retrieve GiGaRunManager ", sc ); }
  if( 0 == runMgr () ) 
    { return Error("Unable to retrieve GiGaRunManager ", sc ); }
  runMgr()->addRef();
  // 
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** instantiate new physics list object using abstract factory technique 
 *  @param TypeName    "Type/Name" of physics list object 
 *  @param PL           reference to new phisics list object 
 *  @return status code 
 */
// ============================================================================
StatusCode 
GiGaSvc::physList
( const std::string& TypeName , 
  IGiGaPhysList*&    PL       )
{
  PL = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = 
    GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("physicsList(): Physics List Type/Name=" + 
                   TypeName+" is unresolved!",sc) ; }
  sc = toolSvc() ->retrieveTool( Type , Name , PL , this );
  if( sc.isFailure() )
    { return Error("Could not retrieve IGiGaPhysList='" 
                   + Type + "'/'" + Name + "'" , sc ) ; }
  if( 0 == PL        )
    { return Error("Could not retrieve IGiGaPhysList='" 
                   + Type + "'/'" + Name + "'"      ) ; }
  //
  PL->addRef(); 
  // 
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** instantiate new stacking action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of stacking action object 
 *  @param SA           reference to new stacking action object 
 *  @return status code 
 */
// ============================================================================
StatusCode 
GiGaSvc::stackAction
( const std::string& TypeName , 
  IGiGaStackAction*& SA       )
{
  SA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("stackAction(): Stack Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  
  sc = toolSvc() ->retrieveTool( Type , Name , SA , this );
  if( sc.isFailure() )
    { return Error("Could not retrieve IGiGaStackAction='" 
                   + Type + "'/'" + Name + "'" , sc ) ; }
  if( 0 == SA        )
    { return Error("Could not retrieve IGiGaStackAction='" 
                   + Type + "'/'" + Name + "'"      ) ; }
  //
  SA->addRef(); 
  // 
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** instantiate new tracking action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of tracking action object 
 *  @param SA           reference to new tracking action object 
 *  @return status code 
 */
// ============================================================================
StatusCode 
GiGaSvc::trackAction
( const std::string& TypeName , 
  IGiGaTrackAction*& SA       )
{
  SA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("trackAction(): Track Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  
  sc = toolSvc() ->retrieveTool( Type , Name , SA , this );
  if( sc.isFailure() )
    { return Error("Could not retrieve IGiGaTrackAction='" 
                   + Type + "'/'" + Name + "'" , sc ) ; }
  if( 0 == SA        )
    { return Error("Could not retrieve IGiGaTrackAction='" 
                   + Type + "'/'" + Name + "'"      ) ; }
  //
  SA->addRef(); 
  // 
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** instantiate new stepping action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of stepping action object 
 *  @param SA           reference to new stepping action object 
 *  @return status code 
 */
// ============================================================================
StatusCode 
GiGaSvc::stepAction
( const std::string& TypeName , 
  IGiGaStepAction*&  SA       )
{
  SA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("stepAction(): Stepping Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  
  sc = toolSvc() ->retrieveTool( Type , Name , SA , this );
  if( sc.isFailure() )
    { return Error("Could not retrieve IGiGaStepAction='" 
                   + Type + "'/'" + Name + "'" , sc ) ; }
  if( 0 == SA        )
    { return Error("Could not retrieve IGiGaStepAction='" 
                   + Type + "'/'" + Name + "'"      ) ; }
  //
  SA->addRef(); 
  // 
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
/** instantiate new event action object using abstract factory technique 
 *  @param TypeName    "Type/Name" of event  action object 
 *  @param SA           reference to new event action object 
 *  @return status code 
 */
// ============================================================================
StatusCode 
GiGaSvc::eventAction
( const std::string& TypeName , 
  IGiGaEventAction*& SA       )
{
  SA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("eventAction(): Event Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  
  sc = toolSvc() ->retrieveTool( Type , Name , SA , this );
  if( sc.isFailure() )
    { return Error("Could not retrieve IGiGaEventAction='" 
                   + Type + "'/'" + Name + "'" , sc ) ; }
  if( 0 == SA        )
    { return Error("Could not retrieve IGiGaEventAction='" 
                   + Type + "'/'" + Name + "'"      ) ; }
  //
  SA->addRef(); 
  // 
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** instantiate new run action object using abstract factory technique 
 *  @param  TypeName    "Type/Name" of run  action object 
 *  @param  SA           reference to new run action object 
 *  @return status code 
 */
// ============================================================================
StatusCode 
GiGaSvc::runAction
( const std::string& TypeName , 
  IGiGaRunAction*&   SA       )
{
  SA = 0 ; /// reset output value 
  std::string Type , Name ; 
  StatusCode sc = GiGaUtil::SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) 
    { return Error("runAction(): Run Action Type/Name=" + 
                   TypeName+" is unresolved!",sc);}
  
  sc = toolSvc() ->retrieveTool( Type , Name , SA , this );
  if( sc.isFailure() )
    { return Error("Could not retrieve IGiGaRunAction='" 
                   + Type + "'/'" + Name + "'" , sc ) ; }
  if( 0 == SA        )
    { return Error("Could not retrieve IGiGaRunAction='" 
                   + Type + "'/'" + Name + "'"      ) ; }
  //
  SA->addRef(); 
  // 
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** printout 
 *  @param msg            message 
 *  @param lvl            printout level 
 *  @param sc             status code
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::Print
( const std::string& Message , 
  const MSG::Level & level   , 
  const StatusCode & Status ) const 
{ 
  MsgStream log( msgSvc() , name() ); 
  log << level << Message << endreq ; 
  return  Status; 
};
// ============================================================================

// ============================================================================
/** error printout 
 *  @param msg         error message 
 *  @param sc          error status code
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::Error
( const std::string& Message , 
  const StatusCode & Status ) const
{ 
  Stat stat( chronoSvc() , name() + ":Error" ); 
  // increase error counter 
  m_errors [ Message ] += 1;
  return  Print( Message , MSG::ERROR  , Status  ) ; 
};  
// ============================================================================

// ============================================================================
/** warning printout 
 *  @param msg         warning message 
 *  @param sc          warning status code
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::Warning
( const std::string& Message , 
  const StatusCode & Status ) const 
{ 
  Stat stat( chronoSvc() , name() + ":Warning" ); 
  // increase counter of warnings  
  m_errors [ Message ] += 1;
  return  Print( Message , MSG::WARNING , Status ) ; 
};  
// ============================================================================

// ============================================================================
/** exception
 *  @param Message        exception message 
 *  @param Excp           reference to "previous" exception
 *  @param level          exception printout level 
 *  @param Status         exception status code
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::Exception
( const std::string    & Message , 
  const GaudiException & Excp    ,
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Stat stat( chronoSvc()  , Excp.tag()  );
  Print ( "GaudiException: catch and re-throw " + Message , level , Status );
  // increase counter of exceptions  
  m_exceptions [ Message ] += 1;
  throw GiGaException( name() + "::" + Message , Excp , Status  );
  return  Status;
};  
// ============================================================================

// ============================================================================
/** exception
 *  @param Message        exception message 
 *  @param Excp           reference to "previous" exception
 *  @param level          exception printout level 
 *  @param Status         exception status code
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::Exception
( const std::string    & Message , 
  const std::exception & Excp    ,
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Stat stat( chronoSvc()  , Excp.what()  );
  Print ( "std::exception: catch and re-throw " + Message , level , Status );
  // increase counter of exceptions  
  m_exceptions [ Message ] += 1;
  throw GiGaException( name() + "::" + Message + " (" + 
                       Excp.what() + ")", Status  );
  return  Status;
};  
// ============================================================================

// ============================================================================
/** exception
 *  @param Message        exception message 
 *  @param level          exception printout level 
 *  @param Status         exception status code
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSvc::Exception
( const std::string    & Message , 
  const MSG::Level     & level   , 
  const StatusCode     & Status  )  const 
{
  Stat stat( chronoSvc()  , "*UNKNOWN Exception*"  );
  Print ( "GiGaException throw " + Message , level , Status );
  // increase counter of exceptions  
  m_exceptions [ Message ] += 1;
  throw GiGaException( name() + "::" + Message , Status );
  return  Status;
}; 
// ============================================================================


// ============================================================================
// The END 
// ============================================================================























