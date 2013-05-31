// $Id: GiGa.cpp,v 1.18 2009-12-17 11:00:12 marcocle Exp $ 
#define GIGA_GIGASVC_CPP 1 

// Include files 
// from STD & STL 
#include <string>
#include <list>
#include <vector> 
#include <algorithm> 

// from Gaudi
#include    "GaudiKernel/ISvcLocator.h"
#include    "GaudiKernel/IMessageSvc.h"
#include    "GaudiKernel/IChronoStatSvc.h"
#include    "GaudiKernel/IToolSvc.h"
#include    "GaudiKernel/SvcFactory.h"
#include    "GaudiKernel/MsgStream.h"
#include    "GaudiKernel/Bootstrap.h"
#include    "GaudiKernel/MsgStream.h"
#include    "GaudiKernel/Stat.h"
#include    "GaudiKernel/PropertyMgr.h"
#include    "GaudiKernel/IRndmGenSvc.h"

// from G4 
#include    "G4UIsession.hh"
#include    "G4VVisManager.hh"
#include    "G4ParticleTable.hh"
#include    "G4ParticlePropertyTable.hh"  

// from GiGa
#include    "GiGa/IGiGaPhysicsList.h"
#include    "GiGa/IGiGaStackAction.h"
#include    "GiGa/IGiGaTrackAction.h"
#include    "GiGa/IGiGaStepAction.h"
#include    "GiGa/IGiGaEventAction.h"
#include    "GiGa/IGiGaRunAction.h"
#include    "GiGa/IGiGaRunManager.h" 
#include    "GiGa/IGiGaGeoSrc.h" 
#include    "GiGa/IGiGaUIsession.h" 
#include    "GiGa/IGiGaVisManager.h" 
#include    "GiGa/GiGaException.h"
#include    "GiGa/GiGaUtil.h"

// local 
#include    "GiGa.h"

//-----------------------------------------------------------------------------
// Implementation of general non-inline methods from class GiGaSvc
//
// YYYY-MM-DD : I.Belyaev
// 
// Last modified 2006-07-21 : G.Corti 
//-----------------------------------------------------------------------------

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_SERVICE_FACTORY(GiGa)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGa::GiGa( const std::string& name, ISvcLocator* svcloc )
  : Service( name , svcloc )
  , m_chronoSvc( 0 ) 
  , m_toolSvc( 0 ) 
  , m_runMgr( 0 )
  , m_geoSrc( 0 ) 
  , m_GiGaPhysList( 0 )
  , m_GiGaStackAction( 0 )
  , m_GiGaTrackAction( 0 )
  , m_GiGaStepAction( 0 )
  , m_GiGaEventAction( 0 )
  , m_GiGaRunAction( 0 )
  , m_uiSession( 0 )
  , m_visManager( 0 )
  , m_rndmSvc( 0 )
  , m_errors()
  , m_warnings()
  , m_exceptions()
{
  /// name of runmanager  
  declareProperty( "RunManager",          
                   m_runMgrName = "GiGaRunManager/GiGaMgr" ); 
  /// name of geometry source 
  declareProperty( "GeometrySource",      m_geoSrcName = "GiGaGeo" ); 
  /// type and name of Physics List object 
  declareProperty( "PhysicsList", 
                   m_GiGaPhysListName = "GiGaPhysListModular/ModularPL" );
  /// type and Name of Stacking Action object 
  declareProperty( "StackingAction", m_GiGaStackActionName = ""  ) ;
  /// type and Name of Tracking Action object 
  declareProperty( "TrackingAction", 
                   m_GiGaTrackActionName = "GiGaTrackActionSequence/TrackSeq" );
  /// type and Name of Stepping Action object 
  declareProperty( "SteppingAction", 
                   m_GiGaStepActionName = "GiGaStepActionSequence/StepSeq" );
  /// type and Name of Event Action object 
  declareProperty( "EventAction", 
                   m_GiGaEventActionName = "GiGaEventActionSequence/EventSeq" );
  /// type and Name of Run Action object 
  declareProperty( "RunAction", 
                   m_GiGaRunActionName =  "GiGaRunActionSequence/RunSeq" );
  /// User Interface Sessions 
  declareProperty( "UIsession",           m_uiSessionName = "" );
  /// Visual Manager  
  declareProperty( "VisManager",          m_visManagerName = "" );  
  /// Random Numbers Service   
  declareProperty( "RandomNumberService", m_rndmSvcName = "RndmGenSvc" );  
  /// Control print out of G4 particles list
  declareProperty( "PrintG4Particles",    m_printParticles = false );
}

//=============================================================================
// Destructor
//=============================================================================
GiGa::~GiGa(){}

//=============================================================================
// query interface 
//=============================================================================
StatusCode GiGa::queryInterface( const InterfaceID& id , void** ppI ) 
{
  if ( 0 == ppI  ) { 
    return StatusCode::FAILURE;                   //  RETURN !!!
  } else if ( IGiGaSvc::interfaceID() == id ) {
    *ppI = static_cast<IGiGaSvc*>( this ); 
  } else if ( IGiGaSetUpSvc::interfaceID() == id ) {
    *ppI = static_cast<IGiGaSetUpSvc*>( this );
  } 
  else { 
    return Service::queryInterface( id , ppI );   //  RETURN !!!
  } 

  addRef(); 

  return StatusCode::SUCCESS;
}

//=============================================================================
// service initialization 
//=============================================================================
StatusCode GiGa::initialize() 
{
  // initialize the base class 
  StatusCode sc = Service::initialize(); 
  if( sc.isFailure() ) { 
    return Error("Unable to initialize the base class Service ", sc ); 
  }

  setProperties(); 

  /// print ALL properties 
  typedef std::vector<Property*> Properties;
  const Properties& properties = getProperties() ;
  MsgStream msg( msgSvc(), name () );
  msg << MSG::DEBUG 
      << " List of ALL properties of "
      << System::typeinfoName( typeid( *this ) ) << "/" << this->name()
      << "   #properties = " << properties.size() << endmsg;
  for( Properties::const_reverse_iterator property 
         = properties.rbegin() ;
       properties.rend() != property ; ++property ) {
    msg << MSG::DEBUG
        << "Property ['Name': Value] = " 
        << (**property) << endmsg;
  }

  // locate  services, 
  if( 0 == svcLoc() ) { 
    return Error("ISvcLocator* points to NULL!"); 
  }
  // ChronoStatSvc
  { 
    StatusCode sc = svcLoc()->service( "ChronoStatSvc" , m_chronoSvc , true );
    if( sc.isFailure() ) { 
      return Error("Unable to locate Chrono & Stat Service", sc ); 
    } 
    if( 0 == chronoSvc() ) { 
      return Error("Unable to locate Chrono & Stat Service" );
    }
  }
  // ToolSvc
  {
    StatusCode sc = svcLoc()->service( "ToolSvc" , m_toolSvc , true ); 
    if( sc.isFailure() ) { 
      return Error("Unable to locate Tool Service", sc ); 
    } 
    if( 0 == toolSvc() ) { 
      return Error("Unable to locate Tool Service" ); 
    }
  }
  
  // get GiGa Run Manager
  m_runMgr = tool( m_runMgrName , m_runMgr , this );
  if( 0 == m_runMgr ) { 
    return Error("Unable to create/locate GiGaRunManager " );
  }

  // try to locate Physics List Object and make it known for GiGa 
  if( !m_GiGaPhysListName.empty() ) 
    { 
      m_GiGaPhysList = tool( m_GiGaPhysListName , m_GiGaPhysList , this );
      if( 0 == m_GiGaPhysList ) 
        { return Error("Unable to create Physics List" ) ; }
      *this << m_GiGaPhysList -> physicsList() ;
      Print("Used Physics List Object is " 
            + GiGaUtil::ObjTypeName( m_GiGaPhysList ) 
            + "/" + m_GiGaPhysList -> name() );
    }
  else { Warning("Physics List is not required to be loaded!") ; }
  
  // try to locate Stacking Action Object and make it known for GiGa 
  if( !m_GiGaStackActionName.empty() )
    {
      m_GiGaStackAction = 
        tool( m_GiGaStackActionName , m_GiGaStackAction , this );
      if( 0 == m_GiGaStackAction ) 
        { return Error("Unable to create Stacking Action" ) ; }
      *this << m_GiGaStackAction ;
      Print("Used Stacking Action Object is " 
            + GiGaUtil::ObjTypeName( m_GiGaStackAction ) 
            + "/" + m_GiGaStackAction -> name() );
    }
  else { Print("Stacking Action Object is not required to be loaded") ; } 

  // try to locate Tracking Action Object and make it known for GiGa 
  if( !m_GiGaTrackActionName.empty() )
    {
      m_GiGaTrackAction = 
        tool( m_GiGaTrackActionName , m_GiGaTrackAction , this );
      if( 0 == m_GiGaTrackAction ) 
        { return Error("Unable to create Tracking Action"  ) ; }
      *this << m_GiGaTrackAction  ;
      Print("Used Tracking Action Object is " 
            + GiGaUtil::ObjTypeName( m_GiGaTrackAction ) 
            + "/" + m_GiGaTrackAction -> name() );
    }
  else { Print("Tracking Action Object is not required to be loaded") ; } 
  
  // try to locate Stepping Action Object and make it known for GiGa 
  if( !m_GiGaStepActionName.empty() )
    {
      m_GiGaStepAction = 
        tool( m_GiGaStepActionName , m_GiGaStepAction , this );
      if( 0 == m_GiGaStepAction ) 
        { return Error("Unable to create Stepping Action"  ) ; }
      *this << m_GiGaStepAction ;
      Print("Used  Stepping Action Object is " 
            + GiGaUtil::ObjTypeName( m_GiGaStepAction ) 
            + "/" + m_GiGaStepAction -> name() );
    }
  else { Print("Stepping Action Object is not required to be loaded") ; } 

  // try to locate Event    Action Object and make it known for GiGa 
  if( !m_GiGaEventActionName.empty() )
    {
      m_GiGaEventAction = 
        tool( m_GiGaEventActionName , m_GiGaEventAction , this );
      if( 0 == m_GiGaEventAction ) 
        { return Error("Unable to create Event Action" ) ; }
      *this << m_GiGaEventAction ;
      Print("Used  Event Action Object is " 
            + GiGaUtil::ObjTypeName( m_GiGaEventAction ) 
            + "/" + m_GiGaEventAction -> name() );
    }
  else { Print("Event Action Object is not required to be loaded") ; } 

  // try to locate Run Action Object and make it known for GiGa 
  if( !m_GiGaRunActionName.empty() )
    {
      m_GiGaRunAction = 
        tool( m_GiGaRunActionName , m_GiGaRunAction , this );
      if( 0 == m_GiGaRunAction ) 
        { return Error("Unable to create Run Action"  ) ; }
      *this << m_GiGaRunAction ;
      Print("Used  Run Action Object is " 
            + GiGaUtil::ObjTypeName( m_GiGaRunAction ) 
            + "/" + m_GiGaRunAction -> name() );
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
      if( 0 != runMgr() ){ runMgr()->declare( geoSrc() ); }
    }
  else { Print("GiGa Geometry Source is not required to be loaded") ; }
  
  // try to locate User Interface Object and make it known for GiGa 
  if( !m_uiSessionName.empty() ) 
    {
      m_uiSession = tool( m_uiSessionName , m_uiSession , this );
      if( 0 == m_uiSession ) 
        { return Error("Unable to create UI session "  ) ; }
      // transfer the UI session to the  Run Manager 
      runMgr()->declare( m_uiSession -> session() );
      Print("Used  UI session object is " 
            + GiGaUtil::ObjTypeName( m_uiSession  ) 
            + "/" + m_uiSession -> name() );
    }
  else { Print("GiGa User Interface session is not required to be launched");} 
  
  // instantiate Visualisation Manager
  if( !m_visManagerName.empty() )
    {
      m_visManager = tool( m_visManagerName , m_visManager , this );
      if( 0 == m_visManager ) 
        { return Error("Unable to create Visualization Manager " ) ; }
      Print("Used  Visualization manager object is " 
            + GiGaUtil::ObjTypeName( m_visManager  ) 
            + "/" + m_visManager -> name() );
    }
  else { Print("Visualisation Manager is not required to be created.") ; } 

  // Locate random number service 
  if( !m_rndmSvcName.empty() )
    {
      StatusCode sc = 
        svcLoc()->service( m_rndmSvcName , m_rndmSvc , true ); 
      if( sc.isFailure()   ) 
        { return Error("Unable to locate Random Number service '"
                       + m_rndmSvcName + "'" , sc ) ; } 
      if( 0 == rndmSvc() ) 
        { return Error("Unable to locate Random Number service '"
                       + m_rndmSvcName + "'" , sc ) ; }
    }
    
  /// Dump all particles known to Geant4 
  if( m_printParticles ) {
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    particleTable->DumpTable("all");
  }

  return StatusCode::SUCCESS ; 
}

//=============================================================================
// service finalization  
//=============================================================================
StatusCode GiGa::finalize()
{  
  Print("Finalization" , MSG::DEBUG , StatusCode::SUCCESS );

  if (0 != toolSvc() &&
      SmartIF<IService>(toolSvc())->FSMState() >= Gaudi::StateMachine::INITIALIZED) {
    if(0 != m_visManager) {
      toolSvc()->releaseTool(m_visManager);
    }
    m_visManager = 0;

    if(0 != m_uiSession) {
      toolSvc()->releaseTool(m_uiSession);
    }
    m_uiSession = 0; 
    
    if(0 != m_GiGaRunAction) {
      toolSvc()->releaseTool(m_GiGaRunAction);
    }
    m_GiGaRunAction = 0; 
    
    if(0 != m_GiGaEventAction) {
      toolSvc()->releaseTool(m_GiGaEventAction);
    }
    m_GiGaEventAction = 0;
    
    if(0 != m_GiGaStepAction) {
      toolSvc()->releaseTool(m_GiGaStepAction);
    }
    m_GiGaStepAction = 0;

    if (0 != m_GiGaTrackAction) {
      toolSvc()->releaseTool(m_GiGaTrackAction);
    }
    m_GiGaTrackAction = 0;
    
    if (0 != m_GiGaStackAction) {
      toolSvc()->releaseTool(m_GiGaStackAction);
    }
    m_GiGaStackAction = 0;
    
    if (0 != m_GiGaPhysList) {
      toolSvc()->releaseTool(m_GiGaPhysList);
    }
    m_GiGaPhysList = 0;
  }
  
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
  
  // release the run manager (almost the last)
  if( 0 != runMgr   ()  && 0 != toolSvc() ) 
    { toolSvc() -> releaseTool( runMgr() ) ; m_runMgr    = 0 ; } 
  // release all used services 
  if( 0 != rndmSvc  ()  ) { rndmSvc   () -> release () ; m_rndmSvc    = 0 ; } 
  if( 0 != toolSvc  ()  ) { toolSvc   () -> release () ; m_toolSvc    = 0 ; } 
  if( 0 != chronoSvc()  ) { chronoSvc () -> release () ; m_chronoSvc  = 0 ; }
  if( 0 != geoSrc   ()  ) { geoSrc    () -> release () ; m_geoSrc     = 0 ; } 

  
  ///  finalize the base class 
  return Service::finalize();
}

//=============================================================================
// prepare the event 
//=============================================================================
StatusCode GiGa::prepareTheEvent( G4PrimaryVertex * vertex ) 
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
}


//=============================================================================
// retrieve the event 
//=============================================================================
StatusCode GiGa::retrieveTheEvent( const G4Event*& event) 
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
}


//=============================================================================
// retrieve IGiGaRunManager 
//=============================================================================
StatusCode GiGa::retrieveRunManager() 
{
  if( 0 != runMgr  () ) { return StatusCode::SUCCESS ; }
  if( 0 == toolSvc () )
    { return Error("retrieveRunManager():: IToolSvc* points to NULL!"); }
  m_runMgr = tool( m_runMgrName , m_runMgr , this ) ;
  if( 0 == runMgr () ) { return Error("Unable to retrieve GiGaRunManager " ); }
  // 
  return StatusCode::SUCCESS ;
}


//=============================================================================
// printout 
//=============================================================================
StatusCode GiGa::Print( const std::string& Message , 
                        const MSG::Level & level   , 
                        const StatusCode & Status ) const 
{ 
  MsgStream log( msgSvc() , name() ); 
  log << level << Message << endreq ; 
  return  Status; 
}


//=============================================================================
// error printout 
//=============================================================================
StatusCode GiGa::Error( const std::string& Message , 
                        const StatusCode & Status ) const
{ 
  Stat stat( chronoSvc() , name() + ":Error" ); 
  // increase error counter 
  m_errors [ Message ] += 1;
  return  Print( Message , MSG::ERROR  , Status  ) ; 
}  

//=============================================================================
// warning printout 
//=============================================================================
StatusCode GiGa::Warning( const std::string& Message , 
                          const StatusCode & Status ) const 
{ 
  Stat stat( chronoSvc() , name() + ":Warning" ); 
  // increase counter of warnings  
  m_warnings [ Message ] += 1 ;
  return  Print( Message , MSG::WARNING , Status ) ; 
}


//=============================================================================
// exception
//=============================================================================
StatusCode GiGa::Exception( const std::string    & Message , 
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
}  


//=============================================================================
// exception
//=============================================================================
StatusCode GiGa::Exception( const std::string    & Message , 
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
}  


//=============================================================================
// exception
//=============================================================================
StatusCode GiGa::Exception( const std::string    & Message , 
                            const MSG::Level     & level   , 
                            const StatusCode     & Status  )  const 
{
  Stat stat( chronoSvc()  , "*UNKNOWN Exception*"  );
  Print ( "GiGaException throw " + Message , level , Status );
  // increase counter of exceptions  
  m_exceptions [ Message ] += 1;
  throw GiGaException( name() + "::" + Message , Status );
  return  Status;
} 


//=============================================================================























