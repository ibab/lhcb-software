// $Id: GiGaBase.cpp,v 1.11 2002-05-01 18:23:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ===========================================================================
#define GIGA_GIGABASE_CPP 1 
// ============================================================================
/// includes 
///  STL
#include <string>
///  GaudiKernel
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISerialize.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/IChronoStatSvc.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/IMagneticFieldSvc.h" 
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IObjManager.h"
#include "GaudiKernel/IJobOptionsSvc.h"
///  GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Stat.h"
///  from GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/IGiGaSetUpSvc.h"
#include "GiGa/GiGaException.h"
#include "GiGa/GiGaUtil.h" 
#include "GiGa/GiGaBase.h" 

// ============================================================================
/** standard constructor 
 *  @param name object name 
 *  @param loc  pointer to servcie locator 
 */
// ============================================================================
GiGaBase::GiGaBase( const std::string& Name , ISvcLocator* svc )
  /// reference count
  : m_count      ( 0                     ) 
  /// name 
  , m_name       ( Name                  ) 
  ///
  , m_gigaName   ( "GiGaSvc"             ) 
  , m_setupName  ( "GiGaSvc"             ) 
  , m_msgName    ( "MessageSvc"          ) 
  , m_chronoName ( "ChronoStatSvc"       ) 
  , m_evtName    ( "EventDataSvc"        )
  , m_detName    ( "DetectorDataSvc"     )
  , m_incName    ( "IncidentSvc"         ) 
  , m_omName     ( "ApplicationMgr"      ) 
  , m_output     ( MSG::NIL              ) 
  ///
  , m_svcLoc     ( svc                   )   
  , m_propMgr    ( 0                     )  
  , m_gigaSvc    ( 0                     ) 
  , m_setupSvc   ( 0                     ) 
  , m_msgSvc     ( 0                     ) 
  , m_chronoSvc  ( 0                     ) 
  , m_evtSvc     ( 0                     )
  , m_detSvc     ( 0                     ) 
  , m_incSvc     ( 0                     ) 
  , m_objMgr     ( 0                     ) 
  ///
  , m_init       ( false                 ) 
  ///
  , m_errors      ()
  , m_warnings    ()
  , m_exceptions  ()
{
  ///
  if( 0 == svcLoc() ) 
    { throw GiGaException("GiGaBase():: ISvcLocator* points to NULL!") ; }
  ///
  m_propMgr = new PropertyMgr();
  ///  
  declareProperty( "OutputLevel"               ,  m_output      );
  declareProperty( "GiGaService"               ,  m_gigaName    );
  declareProperty( "GiGaSetUpService"          ,  m_setupName   );
  declareProperty( "MessageService"            ,  m_msgName     );
  declareProperty( "ChronoStatService"         ,  m_chronoName  );
  declareProperty( "EventDataProvider"         ,  m_evtName     );
  declareProperty( "DetectorDataProvider"      ,  m_detName     );
  declareProperty( "IncidentService"           ,  m_incName     );
  declareProperty( "ObjectManager"             ,  m_omName      );
  ///
};

// ============================================================================
/// destructor 
// ============================================================================
GiGaBase::~GiGaBase() 
{ 
  if( init()         ) { finalize() ; } 
  if( 0 != m_propMgr ) { delete m_propMgr ; m_propMgr = 0 ; } 
} 


// ============================================================================
/** set the type of the object
 *  @return object type 
 */
// ============================================================================
const std::string&  GiGaBase::setMyType() const
{
  return m_myType = GiGaUtil::ObjTypeName( this );
};

// ============================================================================
/** query the interface
 *  @param id unique interface identifier 
 *  @param I  placeholder for returning interface 
 *  @return status code 
 */ 
// ============================================================================
StatusCode GiGaBase::queryInterface(const InterfaceID& riid , void** ppI )
{
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if      ( IProperty::         interfaceID() == riid ) 
    { *ppI = static_cast<IProperty*>         ( this ) ; } 
  else if ( ISerialize::        interfaceID() == riid )
    { *ppI = static_cast<ISerialize*>        ( this ) ; } 
  else if ( IInterface::        interfaceID() == riid ) 
    { *ppI = static_cast<IInterface*>        ( this ) ; } 
  else if ( IIncidentListener:: interfaceID() == riid ) 
    { *ppI = static_cast<IIncidentListener*> ( this ) ; } 
  else                                                  
    { return StatusCode::FAILURE               ; }
  addRef();
  return StatusCode::SUCCESS;
};

// ============================================================================
/// Increment the reference count of Interface instance
// ============================================================================
unsigned long GiGaBase::addRef() { return ++m_count ; };

// ============================================================================
/// Release Interface instance
// ================================x============================================
unsigned long GiGaBase::release () { return 0 < m_count ? --m_count : 0 ; };

// ============================================================================
/** initialize the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::initialize() 
{
  ///
  if( 0 != msgSvc    () ) { msgSvc    ()->release() ; m_msgSvc    = 0 ; } 
  if( 0 != chronoSvc () ) { chronoSvc ()->release() ; m_chronoSvc = 0 ; } 
  if( 0 != gigaSvc   () ) { gigaSvc   ()->release() ; m_gigaSvc   = 0 ; } 
  if( 0 != setupSvc  () ) { setupSvc  ()->release() ; m_setupSvc  = 0 ; } 
  if( 0 != evtSvc    () ) { evtSvc    ()->release() ; m_evtSvc    = 0 ; } 
  if( 0 != detSvc    () ) { detSvc    ()->release() ; m_detSvc    = 0 ; } 
  if( 0 != incSvc    () ) { incSvc    ()->release() ; m_incSvc    = 0 ; } 
  if( 0 != objMgr    () ) { objMgr    ()->release() ; m_objMgr    = 0 ; } 
  ///
  if( 0 == svcLoc()     ) 
    { throw GiGaException("GiGaBase::ini ISvcLocator* points to NULL!");}
  ///
  {
    StatusCode st = setProperties(); 
    if( st.isFailure() ) 
      { return Error("GiGaBase::initialize could not set own properties") ; }
  }
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
  ///
  if( !m_msgName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_msgName , m_msgSvc , true );  
      if( sc.isFailure() ) 
        { return Error("Could not locate IMessageSvc="+ m_msgName , sc ) ; }
      if( 0 == msgSvc () ) 
        { return Error("Could not locate IMessageSvc="+ m_msgName      ) ; }    
      msgSvc()->addRef() ; 
      m_output = 
        m_output < (int) MSG::NIL   ? (int) MSG::NIL   : 
        m_output > (int) MSG::FATAL ? (int) MSG::FATAL : m_output ; 
      if( MSG::NIL != m_output ) 
        { msgSvc()->setOutputLevel( name(), m_output ); }
    }
  else { Warning("Message Service is not requested to be located"); }
  ///
  if( !m_chronoName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_chronoName , m_chronoSvc , true );  
      if( sc.isFailure()    )
        { return Error("Couldn't locate IChronoStatSvc="+ m_chronoName , sc );}
      if( 0 == chronoSvc () ) 
        { return Error("Couldn't locate IChronoStatSvc="+ m_chronoName      );}
      chronoSvc()->addRef() ; 
    }
  else { Warning("Chrono & Stat Service is not requested to be located"); }
  ///
  if( !m_gigaName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_gigaName ,  m_gigaSvc  , true ); 
      if( sc.isFailure () ) 
        { return Error("Could not locate IGiGaSvc="+ m_gigaName , sc ) ; }
      if( 0 == gigaSvc () ) 
        { return Error("Could not locate IGiGaSvc="+ m_gigaName      ) ; }
      gigaSvc()->addRef() ; 
    }
  else { Warning("GiGa Service is not requestedto be located"); }
  ///
  if( !m_setupName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_setupName , m_setupSvc ); 
      if( sc.isFailure   () ) 
        { return Error("Could not locate IGiGaSetUpSvc="+ m_setupName , sc ) ; }
      if( 0 == setupSvc  () ) 
        { return Error("Could not locate IGiGaSetUpSvc="+ m_setupName      ) ; }
      setupSvc()->addRef() ; 
    }
  else { Warning("GiGaSetUp Service is not requested to be located"); }
  ///
  if( !m_evtName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_evtName , m_evtSvc , true ); 
      if( sc.isFailure   () ) 
        { return Error("Could not locate IDataProviderSvc="+ m_evtName , sc );}
      if( 0 == evtSvc    () )
        { return Error("Could not locate IDataProviderSvc="+ m_evtName      );}
      evtSvc()->addRef() ; 
    }
  else { Warning("Event Data  Service is not requested to be located"); }
  ///
  if( !m_detName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_detName , m_detSvc , true ); 
      if( sc.isFailure   () ) 
        { return Error("Couldn't locate IDataProviderSvc="+ m_detName , sc );}
      if( 0 == detSvc    () ) 
        { return Error("Couldn't locate IDataProviderSvc="+ m_detName      );}
      detSvc()->addRef() ; 
    }
  else { Warning("Detector Data  Service is not requested to be located"); }
  ///
  if( !m_incName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_incName , m_incSvc , true ); 
      if( sc.isFailure   () )
        { return Error("Could not locate IIncidentSvc="+ m_incName , sc ) ; }
      if( 0 == incSvc    () ) 
        { return Error("Could not locate IIncidentSvc="+ m_incName      ) ; }
      incSvc()->addRef() ; 
    }
  else { Print("Incident Service is not requested to be located"); }
  ///
  if( !m_omName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_omName , m_objMgr ); 
      if( sc.isFailure   () ) 
        { return Error("Could not locate IObjManager="+ m_omName , sc );}
      if( 0 == objMgr    () )
        { return Error("Could not locate IObjManager="+ m_omName      );}
      objMgr()->addRef() ; 
    }
  else { Print("IObjManager is not required to be located"); }
  ///
  m_init = true ;
  ///
  { /// print ALL properties 
    typedef std::vector<Property*> Properties;
    const Properties& properties = getProperties() ;
    MsgStream log( msgSvc() , name () );
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
  ///
  Print("GiGaBase initialized successfully" , 
        StatusCode::SUCCESS , MSG::VERBOSE ) ;
  ///
  return StatusCode::SUCCESS;  
};
// ============================================================================

// ============================================================================
/** finalize the object 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::finalize()
{
  //
  Print( "GiGaBase finalization" , StatusCode::SUCCESS , MSG::DEBUG ) ;
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
  ///
  if ( init() )
    {
      /// reverse order !!!
      if( 0 != objMgr    () ) { objMgr    ()->release() ; m_objMgr    = 0 ; }
      if( 0 != incSvc    () ) { incSvc    ()->release() ; m_incSvc    = 0 ; } 
      if( 0 != detSvc    () ) { detSvc    ()->release() ; m_detSvc    = 0 ; } 
      if( 0 != evtSvc    () ) { evtSvc    ()->release() ; m_evtSvc    = 0 ; } 
      if( 0 != setupSvc  () ) { setupSvc  ()->release() ; m_setupSvc  = 0 ; } 
      if( 0 != gigaSvc   () ) { gigaSvc   ()->release() ; m_gigaSvc   = 0 ; } 
      if( 0 != chronoSvc () ) { chronoSvc ()->release() ; m_chronoSvc = 0 ; } 
      if( 0 != msgSvc    () ) { msgSvc    ()->release() ; m_msgSvc    = 0 ; } 
      ///
    }
  ///
  m_init = false ;
  ///
  return StatusCode::SUCCESS;
  ///
};
// ============================================================================

// ============================================================================
/** set own properties 
 *  @return status code
 */
// ============================================================================
StatusCode GiGaBase::setProperties() 
{
  ///
  if( 0 == svcLoc()    ) 
    { throw GiGaException("GiGaBase::setPro ICvsLocatort* point to NULL!") ; } 
  ///
  {
    IService* pS = 0 ; 
    StatusCode sc = svcLoc()->getService( "JobOptionsSvc" , pS );
    if( sc.isFailure() )
      { return Error("Could not locate JobOptionsSvc", sc          ) ; } 
    if( 0 == pS        ) 
      { return Error("Could not locate JobOptionsSvc"              ) ; }
    IJobOptionsSvc* jos = dynamic_cast<IJobOptionsSvc*> (pS); 
    if( 0 == jos       )
      { return Error("Could not cast IService* to JobOptionsSvc"   ) ; }
    jos->addRef  () ;
    jos->setMyProperties( name(), this );
    jos->release () ; 
  }
  ///
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** Print the error    message and return status code 
 *  @param mgs message to be printed 
 *  @param sc  status code 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::Error
( const std::string& Message , 
  const StatusCode & Status  ) const 
{
  Stat stat( chronoSvc() , name()+":Error" ); 
  /// increase the counter of errors  
  m_errors[ Message ] += 1 ;
  return Print( Message , Status , MSG::ERROR ); 
};  
// ============================================================================

// ============================================================================
/** Print the warning  message and return status code 
 *  @param mgs message to be printed 
 *  @param sc  status code 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::Warning
( const std::string& Message , 
  const StatusCode & Status  ) const 
{
  Stat stat( chronoSvc() , name()+":Warning" ); 
  /// increase the counter of warnings  
  m_warnings[ Message ] += 1 ;
  return  Print( Message , Status , MSG::WARNING );
};
// ============================================================================
  
// ============================================================================
/** re-throw the exception and print 
 *  @param msg exception message  
 *  @param exc previous exception 
 *  @param lvl print level 
 *  @param status code
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::Exception 
( const std::string    & msg ,  
  const GaudiException & exc , 
  const MSG::Level     & lvl ,
  const StatusCode     & sc  ) const 
{
  Stat stat( chronoSvc() , exc.tag() );
  Print( "GaudiException: catch and re-throw " + msg , sc , lvl );
  /// increase the exception counter 
  m_exceptions[ msg ] += 1 ;
  throw   GiGaException( myType()+"/"+name()+"::" + msg , exc , sc );
  return  sc;
};
// ============================================================================

// ============================================================================
/** re-throw the exception and print 
 *  @param msg exception message  
 *  @param exc previous exception 
 *  @param lvl print level 
 *  @param status code
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::Exception 
( const std::string    & msg ,  
  const std::exception & exc , 
  const MSG::Level     & lvl ,
  const StatusCode     & sc  ) const 
{
  Stat stat( chronoSvc() , "std::exception" );
  Print( "std::exception: catch and re-throw " + msg , sc , lvl );
  /// increase the exception counter 
  m_exceptions[ msg ] += 1 ;
  throw   GiGaException( myType()+"/"+name()+"::" + msg + 
                         " (" + exc.what() + ")" , sc );
  return  sc;
};
// ============================================================================

// ============================================================================
/** throw the exception and print 
 *  @param msg exception message  
 *  @param lvl print level 
 *  @param status code
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::Exception 
( const std::string    & msg ,  
  const MSG::Level     & lvl ,
  const StatusCode     & sc  ) const 
{
  Stat stat( chronoSvc() , "GiGaException" );
  Print( "GiGaException throw " + msg , sc , lvl );
  /// increase the exception counter 
  m_exceptions[ msg ] += 1 ;
  throw   GiGaException( msg , sc );
  return  sc;
};
// ============================================================================

// ============================================================================
/** Print the message and return status code 
 *  @param mgs message to be printed 
 *  @param sc  status code 
 *  @param lvl print level  
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::Print
( const std::string& Message , 
  const StatusCode & Status  , 
  const MSG::Level & level   ) const 
{
  ///
  MsgStream log( msgSvc() , name() ); 
  log << level 
      << myType() 
      << ":: "   
      << Message 
      << endreq  ; 
  ///
  return  Status;
  ///
};
// ============================================================================

// ============================================================================
/** set the property by property
 *  @param p property 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::setProperty  ( const Property    & p ) 
{ return m_propMgr->setProperty( p ) ; };
// ============================================================================

// ============================================================================
/** set the property from input stream
 *  @param s reference to input stream 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::setProperty  ( std::istream      & s )  
{ return m_propMgr->setProperty( s ) ; }
// ============================================================================

// ============================================================================
/** set the property from the string 
 *  @param n property name 
 *  @param s string property 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::setProperty  
( const std::string & n ,
  const std::string & v )
{ return m_propMgr->setProperty( n , v ) ; } ;
// ============================================================================

// ============================================================================
/** get the property by property
 *  @param p pointer to property 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::getProperty  (       Property    * p ) const 
{ return m_propMgr->getProperty( p ) ; };

// ============================================================================
/** get the property by name
 *  @param name property name 
 *  @return status code 
 */
// ============================================================================
const Property& GiGaBase::getProperty  ( const std::string & N ) const 
{ return m_propMgr->getProperty( N ) ; };
// ============================================================================

// ============================================================================ 
/** get the property by std::string
 *  @param s property name 
 *  @param n property string 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::getProperty  ( const std::string & n ,
                                    std::string & v ) const 
{ return m_propMgr->getProperty( n , v ) ; } ;
// ============================================================================

// ============================================================================
/** get list of all properties 
 *  @return list of all proeprties 
 */
// ============================================================================
const std::vector<Property*>& GiGaBase::getProperties() const 
{ return m_propMgr->getProperties()  ; };   
// ============================================================================

// ============================================================================
/// serialize object for reading 
// ============================================================================
StreamBuffer& GiGaBase::serialize( StreamBuffer& S )       
{ 
  ///
  if( 0 == m_propMgr ) { m_propMgr = new PropertyMgr() ;} 
  ///
  S >> m_name  
    >> m_gigaName 
    >> m_setupName
    >> m_msgName   
    >> m_evtName   
    >> m_detName    
    >> m_incName    
    >> m_omName    
    >> m_output ; 
  ///
  m_init = false; 
  ///
  return S;       
};  
// ============================================================================

// ============================================================================
/// serialize object for writing 
// ============================================================================
StreamBuffer& GiGaBase::serialize( StreamBuffer& S ) const 
{
  return S << m_name  
           << m_gigaName 
           << m_setupName
           << m_msgName   
           << m_evtName   
           << m_detName    
           << m_incName    
           << m_omName    
           << m_output ;   
};
// ============================================================================

// ============================================================================
/** handle the incident
 *  @param i reference to the incident
 */
// ============================================================================
void  GiGaBase::handle( const Incident& incident ) 
{ 
  Print("Incident='"  + incident .type   () + "'" + 
        " \tsource='" + incident .source () + "'" ); 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================











