// $Id: CaloTool.cpp,v 1.13 2002-12-01 14:17:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Stat.h"
// LHCbKernel
#include "Kernel/CaloPrint.h"
// CaloKernel
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloTool.h"

// ============================================================================
/** @file CaloTool.cpp
 * 
 *  Implementation file for class CaloTool
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 01/11/2001 
 */
// ============================================================================

// ============================================================================
/** @var  s_detSvcName 
 *  name of Detector Data Service 
 */
// ============================================================================
static const std::string s_detSvcName    ( "DetectorDataSvc" );
// ============================================================================
/** @var  s_evtSvcName 
 *  name of Event Data  Service 
 */
// ============================================================================
static const std::string s_evtSvcName    ( "EventDataSvc"    );
// ============================================================================
/** @var  s_chronoSvcName 
 *  name of Chrono & Stat Service 
 */
// ============================================================================
static const std::string s_chronoSvcName ( "ChronoStatSvc"   );
// ============================================================================
/** @var  s_toolSvcName 
 *  name of Tool Service 
 */
// ============================================================================
static const std::string s_toolSvcName   ( "ToolSvc"         );
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param type tool type (useless)
 *  @param name tool name
 *  @param parent pointer to parent object (service, algorithm or tool)  
 */
// ============================================================================
CaloTool::CaloTool
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : AlgTool      ( type , name , parent )
  , m_chronoSvc  ( 0  ) 
  , m_toolSvc    ( 0  )
  , m_svcLoc     ( 0  ) 
  , m_detSvc     ( 0  ) 
  , m_det        ( 0  ) 
  , m_detName    ( "" )
  , m_errors     (    )
  , m_warnings   (    )
  , m_exceptions (    )
{
  // check for service locator 
  if   ( 0 != serviceLocator() ) { m_svcLoc = serviceLocator    () ; }
  else                           { m_svcLoc = Gaudi::svcLocator () ; }
  if( 0 != svcLoc() )            { svcLoc()->addRef()              ; }
  //
  StringProperty detectorName( "Detector" , m_detName );
  if( 0 != parent ) 
    {    
      const IProperty* parentProperty =
        dynamic_cast<const IProperty*>( parent );
      if( 0 != parentProperty )
        { parentProperty->getProperty( &detectorName ); }
    }
  ///
  m_detName = detectorName.value();
  ///
  declareProperty( "Detector" , m_detName );
};
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTool::initialize ()
{
  ///
  { // initialize the base class 
    StatusCode sc = AlgTool::initialize() ;
    if( sc.isFailure() ) 
      { return Error("Could not initialize base class AlgTool",sc);}    
  }
  //
  { // set own properties 
    StatusCode sc = setProperties() ;
    if( sc.isFailure() ) 
      { return Error("Could not set own properties ",sc);} 
  }
  ///
  { // reset Chrono & Stat service 
    if( 0 != chronoSvc() ) { chronoSvc()->release() ; m_chronoSvc = 0 ; }
    StatusCode sc = 
      svcLoc()->service( s_chronoSvcName , m_chronoSvc , true );
    if( sc.isFailure() )
      { return Error("Could not locate IChronoStatSvc = '" + 
                     s_chronoSvcName+"'", sc ); };
    if( 0 != chronoSvc() ) { chronoSvc()->addRef() ; }
  }
  ///
  { // reset Tool  service 
    if( 0 != toolSvc()   ) { toolSvc()->release() ; m_toolSvc = 0 ; }
    StatusCode sc = 
      svcLoc()->service( s_toolSvcName       , m_toolSvc   , true );
    if( sc.isFailure() )
      { return Error("Could not locate IToolSvc = '" + 
                     s_toolSvcName + "'", sc ); };
    if( 0 != toolSvc()   ) { toolSvc()->addRef() ; }
  }
  ///
  { // locate detector data svc 
    if( 0 != detSvc()    ) { detSvc()->release() ; m_detSvc = 0 ; }
    StatusCode sc = 
      svcLoc()->service( s_detSvcName   , m_detSvc    , true );
    if( sc.isFailure() )
      { return Error("Could not locate IDataProviderSvc = '" +
                     s_detSvcName + "' ", sc ); };
    if( 0 != detSvc()   ) { detSvc()->addRef() ; }    
  }
  ///
  { // reset detector 
    if( 0 != det() ) { m_det = 0 ; }
    if( !detName().empty() && 0 == detSvc() )
      { return Error("Detector name is '"+detName()+"' ,but detSvc()==0!"); }
  }
  ///
  { // print ALL properties 
    MsgStream log( msgSvc() , name() );
    typedef std::vector<Property*> Properties;
    const Properties& properties = getProperties() ;
    log << MSG::DEBUG 
        << " List of ALL properties of "
        << type ()           << "/" 
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
  //
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTool::finalize   ()
{
  /// release services 
  if( 0 != toolSvc    () ) { toolSvc  ()->release() ; m_toolSvc   = 0 ; }
  if( 0 != chronoSvc  () ) { chronoSvc()->release() ; m_chronoSvc = 0 ; }
  if( 0 != detSvc     () ) { detSvc   ()->release() ; m_detSvc    = 0 ; }
  ///
  if( 0 != det        () ) { m_det = 0 ; }
  ///
  if( 0 != m_errors     .size () || 
      0 != m_warnings   .size () || 
      0 != m_exceptions .size () ) 
    {      
      MsgStream log( msgSvc() , name() );
      // format printout 
      CaloPrint print;
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
              << " #ERRORS   = " << print( excp ->second ) 
              << " Message='"    <<        excp ->first    << "'" << endreq ; 
        }  
      // print errors counter 
      for( Counter::const_iterator error = m_errors.begin() ;
           error != m_errors.end() ; ++error )
        {
          log << MSG::ALWAYS 
              << " #ERRORS   = " << print( error->second ) 
              << " Message='"    <<        error->first    << "'" << endreq ; 
        }  
      // print warnings
      for( Counter::const_iterator warning = m_warnings.begin() ;
           warning != m_warnings.end() ; ++warning )
        {
          log << MSG::ALWAYS 
              << " #WARNINGS = " << print( warning->second ) 
              << " Message='"    <<        warning->first  << "'" << endreq ; 
        }  
    }
  m_errors     .clear();
  m_warnings   .clear();
  m_exceptions .clear();
  // finalize the base class 
  return AlgTool::finalize() ;
};
// ============================================================================

// ============================================================================
/** destructor 
 */
// ============================================================================
CaloTool::~CaloTool(){};
// ============================================================================

// ============================================================================
/** query interface method  
 *  @param  id  unique interface ID 
 *  @param  pI  placeholder for interface 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTool::queryInterface ( const InterfaceID& id ,
                                      void**             pI )
{
  /// check the placeholder 
  if( 0 == pI ) { return StatusCode::FAILURE ; }
  ///
  if      ( id == IAlgTool::   interfaceID() ) 
    { *pI = static_cast<IAlgTool*>   (this); }
  else if ( id == IInterface:: interfaceID() )
    { *pI = static_cast<IInterface*> (this); }
  else 
    { return AlgTool::queryInterface( id , pI ) ; }
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
/** Print the error  message and return status code
 * @param msg    error message 
 *  @param st     status code 
 *  @return       status code 
 */
// ============================================================================
StatusCode CaloTool::Error     
( const std::string& msg , 
  const StatusCode & st  , 
  const size_t       mx  ) const 
{
  // increase global error counter 
  Stat stat( chronoSvc() , name()+":Error" ); 
  // increase local counter of errors  
  return ( ++m_errors[ msg ] < mx ) ? Print( msg , st , MSG::ERROR ) : st ;
};
// ============================================================================

// ============================================================================
/** Print the warning  message and return status code 
 *  @param msg    warning message 
 *  @param st     statsu code 
 *  @return       status code 
 */
// ============================================================================
StatusCode CaloTool::Warning   
( const std::string& msg , 
  const StatusCode & st  , 
  const size_t       mx  ) const 
{
  // increase global error counter 
  Stat stat( chronoSvc() , name()+":Warning" ); 
  // increase local counter of warnings  
  return ( ++m_warnings[ msg ] < mx ) ? Print( msg , st , MSG::WARNING ) : st ;
};
// ============================================================================

// ============================================================================
/** Print the message and return status code 
 *  @param msg    warning message 
 *  @param st     status code 
 *  @param lvl    print level 
 *  @return       status code 
 */
// ============================================================================
StatusCode CaloTool::Print     
( const std::string& msg , 
  const StatusCode & st  ,
  const MSG::Level & lvl ) const 
{
  MsgStream log( msgSvc() , name() ); 
  log << lvl 
      << type () 
      << " "   << msg ;
  ///
  if( st.isFailure() ) { log << " \tStatusCode=" << st ;}
  ///
  log << endreq ; 
  return  st;
};
// ============================================================================

// ============================================================================
/** Create and (re)-throw the exception  
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloTool::Exception 
( const std::string    & msg ,
  const GaudiException & exc ,
  const MSG::Level     & lvl ,
  const StatusCode     & sc  ) const   
{
  // increase local counter of exceptions  
  m_exceptions[ msg ] += 1 ;
  // increase global exceptions counter 
  Stat stat( chronoSvc() , name()+":Exception" ); 
  Print( "Exception (re)throw: " + msg , sc , lvl  ); 
  throw CaloException( name() + ":: " + msg , exc, sc );
  return  sc ;
};
// ============================================================================

// ============================================================================
/** Create and (re)-throw the exception  
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloTool::Exception 
( const std::string    & msg ,  
  const std::exception & exc , 
  const MSG::Level     & lvl ,
  const StatusCode     & sc  ) const   
{ 
  // increase local counter of exceptions  
  m_exceptions[ msg ] += 1 ;
  // increase global exceptions counter 
  Stat stat( chronoSvc() , name()+":Exception" ); 
  Print( "Exception  (re)throw: " + msg , sc , lvl  ); 
  throw CaloException( name() + ":: " + msg+"("+exc.what()+")", sc );
  return  sc ;
};
// ============================================================================

// ============================================================================
/** Create and throw the exception  
 *  @param msg    exception message 
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloTool::Exception 
( const std::string    & msg ,  
  const MSG::Level     & lvl ,
  const StatusCode     & sc  ) const 
{ 
  // increase local counter of exceptions  
  m_exceptions[ msg ] += 1 ;
  // increase global exceptions counter 
  Stat stat( chronoSvc() , name()+":Exception" ); 
  Print( "Exception  throw: " + msg , sc , lvl  ); 
  throw CaloException( name() + ":: " + msg , sc );
  return  sc ;
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================
