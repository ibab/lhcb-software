// $Id: AssociatorBase.cpp,v 1.10 2003-06-25 14:59:02 ibelyaev Exp $
// $Id: AssociatorBase.cpp,v 1.10 2003-06-25 14:59:02 ibelyaev Exp $

// Include files

// from Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/System.h"

// local
#include "Relations/AssociatorBase.h"

#if defined (__GNUC__) && ( __GNUC__ <= 2 )
#include <strstream>
#else
#include <sstream>
#endif

// ============================================================================
/** @file AssociatorBase.cpp
 *
 *  Implementation file for class : AssociatorBase
 *
 *  @see  AssociatorBase 
 *  @see IAssociator
 *  @see IAssociatorWeighted 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 24/03/2002 
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param type   tool type (?) 
 *  @param name   tool name 
 *  @param parent tool parent
 */
// ============================================================================
Relations::AssociatorBase::AssociatorBase
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : AlgTool ( type, name , parent )
  , m_evtSvc      ( 0  ) 
  , m_toolSvc     ( 0  ) 
  , m_chronoSvc   ( 0  )
  , m_incSvc      ( 0  )
  , m_location    ( "" )
  , m_builderType ( "" )
  , m_builderName ( "" )
  , m_algorithm   ( 0  )
  , m_object      ( 0  ) 
  , m_counter     ( 0  )
  , m_errors      (    )
  , m_warnings    (    )
  , m_exceptions  (    )
{
  // interfaces 
  declareInterface<IIncidentListener> ( this );
  // properties 
  declareProperty  ( "Location"       , m_location     ) ;
  declareProperty  ( "AlgorithmType"  , m_builderType  ) ;  
  declareProperty  ( "AlgorithmName"  , m_builderName  ) ;  
};
// ============================================================================

// ============================================================================
/** destructor (virtual and protected)
 */
// ============================================================================
Relations::AssociatorBase::~AssociatorBase() {};
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @return status code 
 */
// ============================================================================
StatusCode Relations::AssociatorBase::initialize () 
{
  // initialize the base class ;
  StatusCode sc = AlgTool::initialize();
  if( sc.isFailure()     ) 
    { return Error("Could not initialize base class AlgTool!"      , sc ); }
  // locate Chrono & Stat service 
  sc = serviceLocator()->service ( "ChronoStatSvc" , m_chronoSvc   , true );  
  if( sc.isFailure()     ) 
    { return Error("Could not locate 'ChronoStatSvc'"              , sc ); }
  if( 0 == m_chronoSvc   ) 
    { return Error("Could not locate 'ChronoStatSvc'"                   ); }
  // locate the  event data service
  sc = serviceLocator()->service ( "EventDataSvc"  , m_evtSvc      , true );
  if( sc.isFailure()  ) 
    { return Error("Could not locate IDataProvider='EventDataSvc'" , sc ); }
  if( 0 == m_evtSvc   ) 
    { return Error("Could not locate IDataProvider='EventDataSvc'"      ); }
  // locate tool service
  sc = serviceLocator()->service ( "ToolSvc"       , m_toolSvc     , true );
  if( sc.isFailure()  ) 
    { return Error("Could not locate IToolSvc='ToolSvc'"           , sc ); }
  if( 0 == m_toolSvc   ) 
    { return Error("Could not locate IToolSvc='ToolSvc'"                ); }
  // locate incident service
  sc = serviceLocator()->service ( "IncidentSvc"   , m_incSvc      , true );
  if( sc.isFailure()  ) 
    { return Error("Could not locate IIncidentSvc='IncidentSvc'"   , sc ); }
  if( 0 == m_incSvc   ) 
    { return Error("Could not locate IIncidentSvc='IncidentSvc'"        ); }
  // subscribe to the incident 
  incSvc()->addListener( this , "EndEvent"   , 50  );
  // incSvc()->addListener( this , "BeginEvent" , 50  );
  //  
  /// print ALL properties 
  MsgStream log ( msgSvc () , name () );
  typedef std::vector<Property*> Properties;
  const Properties& properties = getProperties() ;
  log << MSG::DEBUG 
      << " List of ALL properties of "
      << type ()           << "/" 
      << name ()           << "   #properties = " 
      << properties.size() << endreq ;
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
  const int   buffer_size  = 256 ;
  char buffer[buffer_size]       ;
#endif
  for( Properties::const_reverse_iterator property = properties.rbegin() ;
       properties.rend() != property ; ++property )  
    {
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
      std::fill( buffer , buffer + buffer_size , 0 );
      std::ostrstream ost ( buffer , buffer_size );
#else
      std::ostringstream ost;
#endif
     (*property)->nameAndValueAsStream( ost );
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
      ost.freeze();
#endif
      log << MSG::DEBUG
          << "Property ['Name': Value] = " 
          << ost.str() << endreq ;
    }
  ///
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @return status code 
 */
// ============================================================================
StatusCode Relations::AssociatorBase::finalize () 
{
  // printout of error/warnings/exceptions
  
  // format printout 
  if( 0 != m_errors      .size () || 
      0 != m_warnings    .size () ||
      0 != m_exceptions  .size ()  ) 
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
           excp  != m_exceptions.end() ; ++excp  )
        {
          log << MSG::ALWAYS 
              << " #EXCEPTIONS= " << excp ->second  
              << " Message='"     << excp ->first    << "'" << endreq ; 
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
  
  // release the builder algorithm 
  if( 0 != m_algorithm ) 
    { 
      m_algorithm -> sysFinalize () ; 
      m_algorithm -> release     () ; 
      m_algorithm = 0 ; 
    }
  // release the services 
  if( 0 != m_toolSvc   ) { m_toolSvc   -> release () ; m_toolSvc   = 0 ; }
  if( 0 != m_chronoSvc ) { m_chronoSvc -> release () ; m_chronoSvc = 0 ; }
  if( 0 != m_evtSvc    ) { m_evtSvc    -> release () ; m_evtSvc    = 0 ; }
  if( 0 != m_incSvc    ) { m_incSvc    -> release () ; m_incSvc    = 0 ; }
  // finalize the base class 
  return AlgTool::finalize() ;
};
// ============================================================================

// ============================================================================
/** Print the error  message and return status code
 * @param msg    error message 
 *  @param st     status code 
 *  @return       status code 
 */
// ============================================================================
StatusCode Relations::AssociatorBase::Error     
( const std::string& msg , 
  const StatusCode & st  ) const 
{
  // increase local errors counter 
  m_errors [ msg ] += 1 ;
  // use global error counter 
  Stat stat( chronoSvc() , name()+":Error" ); 
  return Print( msg , st , MSG::ERROR ); 
};
// ============================================================================

// ============================================================================
/** Print the warning  message and return status code 
 *  @param msg    warning message 
 *  @param st     statsu code 
 *  @return       status code 
 */
// ============================================================================
StatusCode Relations::AssociatorBase::Warning   
( const std::string& msg , 
  const StatusCode & st  ) const 
{
  // increase local warnings  counter 
  m_warnings [ msg ] += 1 ;
  // use global warnings counter 
  Stat stat( chronoSvc() , name()+":Warning" ); 
  return Print( msg , st , MSG::WARNING ); 
};

// ============================================================================
/** Print the message and return status code 
 *  @param msg    warning message 
 *  @param st     status code 
 *  @param lvl    print level 
 *  @return       status code 
 */
// ============================================================================
StatusCode Relations::AssociatorBase::Print     
( const std::string& msg , 
  const StatusCode & st  ,
  const MSG::Level & lvl ) const 
{
  MsgStream log( msgSvc() , name() ); 
  log << lvl << type () << " "   << msg ;
  if      ( st.isSuccess  ()          ) { log << " \t SUCCESS "      << st ; }
  else if ( StatusCode::FAILURE == st ) { log << " \t FAILURE "      << st ; }
  else                                  { log << " \t StatusCode = " << st ; }
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
StatusCode Relations::AssociatorBase::Exception 
( const std::string    & msg ,
  const GaudiException & exc ,
  const MSG::Level     & lvl ,
  const StatusCode     & sc  ) const   
{ 
  // increase local counter of exceptions  
  m_exceptions[ msg ] += 1 ;
  // increase global exceptions counter 
  Stat stat( chronoSvc() , name()+":Exception" ); 
  Print( "Exception (re)throw: " + msg , sc , lvl ); 
  throw   GaudiException( name() + ":: " + msg , "*Relations*" , sc , exc );
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
StatusCode Relations::AssociatorBase::Exception 
( const std::string    & msg ,  
  const std::exception & exc , 
  const MSG::Level     & lvl ,
  const StatusCode     & sc  ) const   
{
  // increase local counter of exceptions  
  m_exceptions[ msg ] += 1 ;
  // increase global exceptions counter 
  Stat stat( chronoSvc() , name()+":Exception" ); 
  Print( "Exception (re)throw: " + msg , sc , lvl  ); 
  throw GaudiException( name() + ":: " + msg + 
                        "(" + exc.what() + ")" , "*Relations*", sc );
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
StatusCode Relations::AssociatorBase::Exception 
( const std::string    & msg ,  
  const MSG::Level     & lvl ,
  const StatusCode     & sc  ) const 
{ 
  // increase local counter of exceptions  
  m_exceptions[ msg ] += 1 ;
  // increase global exceptions counter 
  Stat stat( chronoSvc() , name()+":Exception" ); 
  Print( "Exception  throw: " + msg , sc , lvl  ); 
  throw  GaudiException( name() + ":: " + msg , "*Relations*" , sc );
  return  sc ;
};
// ============================================================================

// ============================================================================
/** The "base" method for access the relation data 
 *
 *  @attention it is virtual method ! One could 
 *  redefine it for "specific" behaviour 
 *
 *  'Default' behaviour:
 *  
 *   - locate relation table in Gaudi Event Transient Store 
 *   - call for Relation builder algorithm to build and 
 *     register  the relation table in Gaudi Event Transient Store
 *   - again locate relation table in Gaudi Event Transient Store 
 *
 *  @return status code 
 */
// ============================================================================
StatusCode Relations::AssociatorBase::locateOrBuild () const 
{ 
  // already exists?
  if( 0 != m_object     ) { return StatusCode::SUCCESS                    ; }
  // (1) locate the object in ETS 
  SmartDataPtr<IInterface>  object1( evtSvc() , location () );
  if( object1 ) 
    { 
      m_object =  object1 ; 
      addRef ( m_object ) ;
      return Print( "Retrieved relation table is '" + location() + 
                    "' (type '" + System::typeinfoName( typeid( *m_object ) ) +
                    "'", StatusCode::SUCCESS , MSG::VERBOSE  ); 
    }
  // (2) get the builder 
  if( 0 == algorithm () ) { return Error("'Builder' is invalid!"        ) ; }
  // (3) use builder to build relation tables
  StatusCode sc = algorithm() -> sysExecute() ;
  if( sc.isFailure   () ) { return Error("Error from 'Builder'!"  ,  sc ) ; }
  // (4) locate data in ETS again
  SmartDataPtr<IInterface> object2( evtSvc() , location () );
  if( !object2 ) { return Error("Data after 'Builder' are not available!" ) ; }
  m_object =  object2  ;
  addRef ( m_object ) ; // do we need this line ?
  //
  return Print( "Builded relation table is '" + location() + 
                "' (type '" + System::typeinfoName( typeid( *m_object ) ) +
                "'", StatusCode::SUCCESS , MSG::VERBOSE  ); 
};
// ============================================================================

// ============================================================================
/** handle the incident 
 *  @see IIncidentListener 
 *  @see Incident 
 *  @see incident incident to be handled 
 */
// ============================================================================
void Relations::AssociatorBase::handle
( const Incident& /* incident */ ) 
{ 
  release( m_object ) ; m_object = 0 ;
  if( 0 != m_counter ) { Warning ( "Mismatch in addRef/release" ) ; }
} ;
// ============================================================================

// ============================================================================
/** locate the algorithm for building the relations
 *  @return status code 
 */
// ============================================================================
StatusCode Relations::AssociatorBase::locateAlgorithm() const
{ 
  // check the existent algorithm 
  if( 0 != m_algorithm      ) { return StatusCode::SUCCESS     ; }
  if( m_builderType.empty() ) { return StatusCode::FAILURE     ; }
  if( m_builderName.empty() ) { m_builderName = m_builderType  ; }
  // Get the algorithm's manager 
  IAlgManager* algMgr = 0 ;
  StatusCode sc = serviceLocator()->
    getService( "" , IAlgManager::interfaceID() , (IInterface*&) algMgr );
  if( sc.isFailure() ) { return Error("Could not locate IAlgManager ", sc );}
  if( 0 == algMgr    ) { return Error("Could not locate IAlgManager "     );}
  // check the existence of the algorithm
  typedef std::list<IAlgorithm*> Algs;
  Algs& algs = algMgr->getAlgorithms() ;
  for( Algs::iterator ia = algs.begin() ; algs.end() != ia ; ++ia )
    {
      if( 0 == *ia                       ) { continue ; }
      if( (*ia)->name() != m_builderName ) { continue ; }
      // algorithm is found ! 
      m_algorithm = *ia ;
      m_algorithm -> addRef() ;
      return StatusCode::SUCCESS ;                         // RETURN ! 
    }
  // algorithm is nor found: try to create it! 
  sc = algMgr->createAlgorithm( m_builderType , m_builderName , m_algorithm );
  if( sc.isFailure()   ) { return Error("Could not create algorithm", sc ) ; }
  if( 0 == m_algorithm ) { return Error("Could not create algorithm"     ) ; }
  // add the reference to the new algorithm 
  m_algorithm -> addRef() ;
  // initialize the new algorithm
  /* If the algorithm has got a property "OutputTable",
     then set it to the Location property of the Associator */
  IProperty* prop = dynamic_cast<IProperty*>( m_algorithm );
  MsgStream log(msgSvc(), name());
  if( prop ) {
    sc = prop->setProperty( "OutputTable", location());
    if( sc.isSuccess() ) {
      log << MSG::DEBUG << "Property OutputTable set to "
          << location() << " in algo " << m_builderName << endreq;
    } else {
      sc = prop->setProperty( "Output", location());
      if( sc.isSuccess() ) {
        log << MSG::DEBUG << "Property Output set to "
            << location() << " in algo " << m_builderName << endreq;
      } else {
        log << MSG::DEBUG << "Property Output[Table] don't exist for algo "
            << m_builderName << endreq;
      }
    }
  } else {
    log << MSG::DEBUG << "Unable to get IProperty pointer for "
        << m_builderName << endreq;
  }
  
  // Now initialise the algorithm, hence jobOptions can supersede the above
  sc = m_algorithm->sysInitialize() ;
  if( sc.isFailure() ) { return Error( "Error from algorithm initialization!");}
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
