// $Id: AssociatorBase.cpp,v 1.1.1.1 2004-07-21 07:57:27 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================

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
  : GaudiTool ( type, name , parent )
  , m_location    ( "" )
  , m_builderType ( "" )
  , m_builderName ( "" )
  , m_algorithm   ( 0  )
  , m_object      ( 0  ) 
  , m_counter     ( 0  )
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
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure()     ) 
  { return Error("Could not initialize base class GaudiTool!"      , sc ); }
  
  // subscribe to the incident 
  if ( 0 == incSvc() ) { return Error ("IIncidentSvc* points to NULL" ) ; }
  incSvc()->addListener( this , IncidentType::EndEvent , 50  );
  
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
  // release the builder algorithm 
  if( 0 != m_algorithm ) 
  { 
    m_algorithm -> sysFinalize () ; 
    m_algorithm -> release     () ; 
    m_algorithm = 0 ; 
  }
  // finalize the base class 
  return GaudiTool::finalize() ;
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
  // (1) locate the object in TES 
  SmartDataPtr<IInterface>  object1( evtSvc() , location () );
  if( object1 ) 
  { 
    m_object =  object1 ; 
    addRef ( m_object ) ;
    if ( msgLevel( MSG::DEBUG ) ) 
    {
      Print( "Retrieved relation table is '" + location() + 
             "' (type '" + System::typeinfoName( typeid( *m_object ) ) +
             "'", StatusCode::SUCCESS , MSG::DEBUG  ); 
    }
    return StatusCode::SUCCESS ;
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
  if ( msgLevel( MSG::DEBUG ) ) 
  { Print( "Builded relation table is '" + location() + 
           "' (type '" + System::typeinfoName( typeid( *m_object ) ) +
           "'", StatusCode::SUCCESS , MSG::DEBUG );
  }
  
  return StatusCode::SUCCESS ;
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
  release ( m_object ) ; m_object = 0 ;
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
