// $Id: GiGaBase.cpp,v 1.15 2003-04-06 18:49:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.14  2002/12/07 14:27:51  ibelyaev
//  see $GIGAROOT/cmt/requirements file
//
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

namespace GiGaBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaBase> s_Counter ;
#endif   
};

// ============================================================================
/** standard constructor 
 *  @see AlgTool 
 *  @param type tool   type (?)  
 *  @param Name object name 
 *  @param parent point to parent object 
 */
// ============================================================================
GiGaBase::GiGaBase
( const std::string& type   ,
  const std::string& Name   ,
  const IInterface*  parent )
  : AlgTool( type , Name , parent )
  ///
  , m_gigaName   ( "GiGa"                ) 
  , m_setupName  ( "GiGa"                ) 
  , m_chronoName ( "ChronoStatSvc"       ) 
  , m_evtName    ( "EventDataSvc"        )
  , m_detName    ( "DetectorDataSvc"     )
  , m_incName    ( "IncidentSvc"         ) 
  ///
  , m_gigaSvc    ( 0                     ) 
  , m_setupSvc   ( 0                     ) 
  , m_chronoSvc  ( 0                     ) 
  , m_evtSvc     ( 0                     )
  , m_detSvc     ( 0                     ) 
  , m_incSvc     ( 0                     ) 
  ///
  , m_errors      ()
  , m_warnings    ()
  , m_exceptions  ()
{
  ///
  if( 0 == svcLoc() ) 
    { throw GiGaException("GiGaBase():: ISvcLocator* points to NULL!") ; }
  ///
  declareInterface<IGiGaInterface>    ( this );
  declareInterface<IIncidentListener> ( this );
  declareInterface<ISerialize>        ( this );
  declareInterface<IAlgTool>          ( this );
  ///
  declareProperty( "GiGaService"               ,  m_gigaName    ) ;
  declareProperty( "GiGaSetUpService"          ,  m_setupName   ) ;
  declareProperty( "ChronoStatService"         ,  m_chronoName  ) ;
  declareProperty( "EventDataProvider"         ,  m_evtName     ) ;
  declareProperty( "DetectorDataProvider"      ,  m_detName     ) ;
  declareProperty( "IncidentService"           ,  m_incName     ) ;
  ///
#ifdef GIGA_DEBUG
  GiGaBaseLocal::s_Counter.increment () ;
#endif 
};
// ============================================================================

// ============================================================================
// destructor 
// ============================================================================
GiGaBase::~GiGaBase() 
{
#ifdef GIGA_DEBUG
  GiGaBaseLocal::s_Counter.decrement () ;
#endif 
};
// ============================================================================

// ============================================================================
/** initialize the object
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::initialize() 
{
  // initialize the base class 
  StatusCode sc = AlgTool::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class AlgTool",sc);};
  // 
  if( 0 == svcLoc()     ) 
    { throw GiGaException("GiGaBase::ini ISvcLocator* points to NULL!");}
  //
  { // print ALL properties 
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
  if( !m_chronoName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_chronoName , m_chronoSvc , true );  
      if( sc.isFailure()    )
        { return Error("Couldn't locate IChronoStatSvc="+ m_chronoName , sc );}
      if( 0 == chronoSvc () ) 
        { return Error("Couldn't locate IChronoStatSvc="+ m_chronoName      );}
    }
  else { Warning("Chrono & Stat Service is not requested to be located"); }
  ///
  if( !m_evtName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_evtName , m_evtSvc , true ); 
      if( sc.isFailure   () ) 
        { return Error("Could not locate IDataProviderSvc="+ m_evtName , sc );}
      if( 0 == evtSvc    () )
        { return Error("Could not locate IDataProviderSvc="+ m_evtName      );}
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
    }
  else { Print("Incident Service is not requested to be located"); }
  ///
  if( !m_gigaName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_gigaName ,  m_gigaSvc  , true ); 
      if( sc.isFailure () ) 
        { return Error("Could not locate IGiGaSvc="+ m_gigaName , sc ) ; }
      if( 0 == gigaSvc () ) 
        { return Error("Could not locate IGiGaSvc="+ m_gigaName      ) ; }
    }
  else { Warning("GiGa Service is not requested to be located"); }
  ///
  if( !m_setupName.empty() )
    {
      StatusCode sc = svcLoc()->service( m_setupName , m_setupSvc ); 
      if( sc.isFailure   () ) 
        { return Error("Could not locate IGiGaSetUpSvc="+ m_setupName , sc ) ; }
      if( 0 == setupSvc  () ) 
        { return Error("Could not locate IGiGaSetUpSvc="+ m_setupName      ) ; }
    }
  else { Warning("GiGaSetUp Service is not requested to be located"); }
  ///
  return Print("GiGaBase initialized successfully" , 
               StatusCode::SUCCESS                 , MSG::VERBOSE ) ;
};
// ============================================================================

// ============================================================================
/** finalize the object 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::finalize()
{
  Print( "GiGaBase Finalization" , StatusCode::SUCCESS , MSG::VERBOSE ) ;
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
  m_errors      .clear () ;
  m_warnings    .clear () ;
  m_exceptions  .clear () ;
  // release services inreverse order !!!
  if( 0 != setupSvc  () ) { setupSvc  () -> release() ; m_setupSvc  = 0 ; } 
  if( 0 != gigaSvc   () ) { gigaSvc   () -> release() ; m_gigaSvc   = 0 ; } 
  if( 0 != incSvc    () ) { incSvc    () -> release() ; m_incSvc    = 0 ; } 
  if( 0 != detSvc    () ) { detSvc    () -> release() ; m_detSvc    = 0 ; } 
  if( 0 != evtSvc    () ) { evtSvc    () -> release() ; m_evtSvc    = 0 ; } 
  if( 0 != chronoSvc () ) { chronoSvc () -> release() ; m_chronoSvc = 0 ; } 
  // finalize the base class 
  return AlgTool::finalize() ;
  ///
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
  throw   GiGaException( type()+"/"+name()+"::" + msg , exc , sc );
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
  throw   GiGaException( type()+"/"+name()+"::" + msg + 
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
  throw   GiGaException( type() + "/" + name() + "::" + msg , sc );
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
      << type() 
      << ":: "   
      << Message 
      << endreq  ; 
  ///
  return  Status;
  ///
};
// ============================================================================

// ============================================================================
// serialize object for reading 
// ============================================================================
StreamBuffer& GiGaBase::serialize( StreamBuffer& S )       
{ return S ; } ;
// ============================================================================

// ============================================================================
/// serialize object for writing 
// ============================================================================
StreamBuffer& GiGaBase::serialize( StreamBuffer& S ) const 
{ return S ; } ;
// ============================================================================

// ============================================================================
/** handle the incident
 *  @param i reference to the incident
 */
// ============================================================================
void  GiGaBase::handle( const Incident& incident ) 
{ 
  Print(" Incident = '"  + incident .type   () + "' " + 
        " \tsource = '"  + incident .source () + "' " ); 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================











