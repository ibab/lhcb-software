
#include <string>
#include <list>
#include <vector> 
#include <algorithm> 


#include    "GaudiKernel/ISvcLocator.h"
#include    "GaudiKernel/IMessageSvc.h"
#include    "GaudiKernel/IChronoStatSvc.h"
#include    "GaudiKernel/IObjManager.h"
#include    "GaudiKernel/SvcFactory.h"
#include    "GaudiKernel/MsgStream.h"
#include    "GaudiKernel/ParticleProperty.h"

#include    "GaudiKernel/Bootstrap.h"

#include    "GiGa/IGiGaPhysList.h"
#include    "GiGa/IGiGaPhysListFactory.h"
#include    "GiGa/GiGaException.h"


// local 
#include    "GiGaRunManager.h" 
#include    "GiGaSvc.h"
#include    "SplitTypeAndName.h"

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
///    class GiGaSvc : implementation of abstract Interfaces IGiGaSvc           ///      
///                    (for event-by-event communications with Geant4)          ///
///                    and IGiGaSetUpSvc (for configuration of Geant4)          ///
///                                                                             ///
///    Author: Vanya Belyaev                                                    ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
static const  SvcFactory<GiGaSvc>                   s_factory ;                 ///
extern const ISvcFactory&          GiGaSvcFactory = s_factory ;                 ///


///////////////////////////////////////////////////////////////////////////////////
GiGaSvc::GiGaSvc( const std::string& name, ISvcLocator* svcloc )                   
  : Service( name , svcloc )
  , m_GiGaRunManager       (   0   )
  , m_chronoSvc            (   0   ) 
  ///
  , m_startUIcommands      (   )
  , m_startOfEvtUIcommands (   )
  , m_endOfEvtUIcommands   (   )
  , m_startOfRunUIcommands (   )
  , m_endOfRunUIcommands   (   )
  , m_endUIcommands        (   )
  ///
  , m_GiGaPhysList               (   )
  , m_objMgr               ( 0 ) 
  , m_objMgrName           ( "ApplicationMgr" )
{
  ///
  /// Geant4 commands to be executed by G4UImanager 
  /// 
  declareProperty( "StartUIcommands"        , m_startUIcommands      ) ;
  declareProperty( "StartOfEventUIcommands" , m_startOfEvtUIcommands ) ;
  declareProperty( "EndOfEventUIcommands"   , m_endOfEvtUIcommands   ) ;
  declareProperty( "StartOfRunUIcommands"   , m_startOfRunUIcommands ) ;
  declareProperty( "EndOfRunUIcommands"     , m_endOfRunUIcommands   ) ;
  declareProperty( "EndUIcommands"          , m_endUIcommands        ) ;
  ///
  declareProperty( "UIsessions"             , m_UIsessions           ) ;             
  ///
  declareProperty( "PhysicsList"            , m_GiGaPhysList                           ) ;
  ///
};
///////////////////////////////////////////////////////////////////////////////////
GiGaSvc::~GiGaSvc() 
{ if( 0 != m_GiGaRunManager  ) { delete m_GiGaRunManager  ; m_GiGaRunManager  = 0 ; } };
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::queryInterface( const IID& id , void** ppI  ) 
{
  if       ( 0 == ppI  )               { return StatusCode::FAILURE             ; }       ///  RETURN !!!  
  if       ( IID_IGiGaSvc      == id ) {  *ppI = (IGiGaSvc*)       this         ; } 
  else if  ( IID_IGiGaSetUpSvc == id ) {  *ppI = (IGiGaSetUpSvc*)  this         ; } 
  else                                 { return Service::queryInterface(id,ppI) ; }    
  addRef(); 
  return StatusCode::SUCCESS;
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::initialize()
{
  /// initialize the base class 
  {
    StatusCode sc = Service::initialize(); 
    if( sc.isFailure() ) { return Error("Unable to initialize the base class Service ", sc ); } 
  }
  ///
  setProperties(); 
  /// locate  services, 
  {
    StatusCode sc = serviceLocator()-> service( "ChronoStatSvc" , m_chronoSvc ); 
    if( sc.isFailure()   ) { return Error("Unable to locate Chrono & Stat Service", sc ); } 
    if( 0 == chronoSvc() ) { return Error("Unable to locate Chrono & Stat Service"     ); } 
    chronoSvc()->addRef();
  }
  ///
  {
    StatusCode sc = serviceLocator()-> service( m_objMgrName , m_objMgr ); 
    if( sc.isFailure() ) { return Error("Unable to locate IObjManager="+m_objMgrName, sc ); } 
    if( 0 == objMgr () ) { return Error("Unable to locate IObjManager="+m_objMgrName     ); } 
    objMgr()->addRef();
  }
  ///
  {
    StatusCode sc = createGiGaRunManager(); 
    if( sc.isFailure() ) { return Error("Unable to create GiGaRunManager ", sc ); }
  } 
  /// try to locate Physics List Object and make it known for GiGa 
  if( !m_GiGaPhysList.empty() )
    {
      IGiGaPhysList* PL   = 0 ;
      StatusCode sc = physList ( m_GiGaPhysList , PL );
      if( sc.isFailure() ) { return Error(" Unable to instantiate Physics List Object "+m_GiGaPhysList, sc );} 
      if( 0 == PL        ) { return Error(" Unable to instantiate Physics List Object "+m_GiGaPhysList     );} 
      ///
      *this << PL ;  /// 
      ///
      Print("Used Phisics List Object is "+System::typeinfoName( typeid( *PL ) ) + "/"+PL->name() );
    }
  else { Warning("Physics List Object is not required to be loaded. Dangerous! Check the configuration!") ; } 
  ///
  return StatusCode::SUCCESS ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::finalize()
{  
  const std::string Tag( name() + ".finalize()" ) ; 
  MsgStream log( msgSvc(), name() ); 
  /// finalize Run Manager 
  StatusCode sc ( StatusCode::SUCCESS ); 
  if( 0 != m_GiGaRunManager ) { sc = m_GiGaRunManager->finalizeRunManager() ;} 
  if( sc.isFailure() ) { Error(" Error in ->finalizeRunManager() method!", sc ); } 
  /// release all used services 
  if( 0 != objMgr   () ) { objMgr   ()->release() ; m_objMgr    = 0 ; } 
  if( 0 != chronoSvc() ) { chronoSvc()->release() ; m_chronoSvc = 0 ; } 
  ///  
  {  
    StatusCode sc(StatusCode::FAILURE); 
    const std::string m1("::delete RunManager  "); 
    ___GIGA_TRY___                         
      { if( 0 != m_GiGaRunManager  ) { delete m_GiGaRunManager ; m_GiGaRunManager = 0 ; } }
    ___GIGA_CATCH_PRINT_AND_RETURN___(Tag,m1,msgSvc(),chronoSvc(),sc);
  }
  ///  
  return Service::finalize();
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::createGiGaRunManager() 
{
  ///
  if( 0 != m_GiGaRunManager ) { return StatusCode::SUCCESS; }     /// RETURN !!!
  ///
  Assert( 0 == G4RunManager::GetRunManager() , 
	  "There exist another instance of G4RunManager!" ) ; 
  ///
  m_GiGaRunManager = new  GiGaRunManager( name()+".Manager" , serviceLocator() ); 
  ///
  Assert( 0 != m_GiGaRunManager              , " Unable to create GiGaRunManager" ) ; 
  Assert( 0 != G4RunManager::GetRunManager() , " Unable to create G4RunManager"   ) ; 
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
///////////////////////////////////////////////////////////////////////////////////
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
	  Assert( sc.isSuccess()        , " prepareTheEvent(): failure from createGiGaRunManager " , sc     ) ; 
	  Assert( 0 != m_GiGaRunManager , " prepareTheEvent(): unable to create GiGaRunManager   "          ) ; 
	}
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method1) ; 
  ///
  ___GIGA_TRY___ 
    {
      sc = m_GiGaRunManager->prepareTheEvent( vertex ) ; 
      Assert( sc.isSuccess() , "preparetheEvent() failure from GiGaRunManager::prepareTheEvent() ", sc ) ; 
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method2) ; 
  ///
  return StatusCode::SUCCESS; 
  ///
};
///////////////////////////////////////////////////////////////////////////////////
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
	  Assert( sc.isSuccess()        , " retrieveTheEvent: failure from createGiGaRunManager " , sc     ) ; 
	  Assert( 0 != m_GiGaRunManager , " retrieveTheEvent: unable to create GiGaRunManager   "          ) ; 
	}
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method1) ; 
  ///
  ___GIGA_TRY___ 
    { 
      sc = m_GiGaRunManager->retrieveTheEvent( event ) ; 
      Assert( sc.isSuccess() , " retrieveTheEvent: failure from GiGaRunManager::retrieveTheEvent" , sc ) ; 
    }
  ___GIGA_CATCH_AND_THROW___(Tag,method2) ; 
  ///
  return StatusCode::SUCCESS; 
  ///
};
///
StatusCode GiGaSvc::physList( const std::string& TypeName , IGiGaPhysList*& PL )
{
  PL = 0 ; /// reset output value 
  if( 0 == objMgr()  ) { return Error("RetrievePhysicsList:  IObjManager* pointd to NULL"); }
  std::string Type , Name ; 
  StatusCode sc = SplitTypeAndName( TypeName , Type , Name );
  if( sc.isFailure() ) { return Error("RetrievePhysicsList: Physics List Type/Name="+TypeName+" is unresolved!",sc);}
  /// locate the factory
  const IGiGaPhysListFactory* PLF = 0 ;  
  {
    bool exist = objMgr()->existsObjFactory( Type ); 
    if( !exist   ) { return Error("RetrievePhysicsList:  Factory  for "+Type+" is not located") ; }  
    const IFactory* fac   = objMgr()->objFactory( Type );
    if( 0 == fac ) { return Error("RetrievePhisicsList: IFactory* for "+Type+" points to NULL" ); }
    PLF = dynamic_cast<const IGiGaPhysListFactory*> ( fac ); 
    if( 0 == PLF ) { return Error("RetrievePhysicsList: IGiGaPhysListFactory* for "+Type+" points to NULL" );}
  }
  ///
  PL = PLF->instantiate( Name , serviceLocator() ) ; 
  if( 0 == PL        ) { return Error("RetrievePhysicsList: could not instantiate IGiGaPhysList* Object "+Type+"/"+Name );} 
  ///
  PL->addRef(); 
  if( PL->initialize().isSuccess() ) { return StatusCode::SUCCESS; } 
  //// 
  PL->release(); delete PL ; PL = 0 ;  
  ///
  return Error("RetrievePhysicsList: could not initialize IGiGaPhysList* Object "+Type+"/"+Name, sc) ;
  ///
};
///////////////////////////////////////////////////////////////////////////////////
























