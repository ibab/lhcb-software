// $Id: AssociatorBase.cpp,v 1.3 2004-11-22 16:49:55 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/11/19 15:01:22  ibelyaev
//  substitute explict algorithm management by 'Data-On-Demand' service
// 
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
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
// ============================================================================
// local
// ============================================================================
#include "Relations/AssociatorBase.h"
// ============================================================================


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
  { return Error ( "Could not initialize base class GaudiTool!" , sc ); }
  
  // subscribe to the incident 
  if ( 0 == incSvc() ) { return Error ("IIncidentSvc* points to NULL" ) ; }
  incSvc()->addListener( this , IncidentType::EndEvent , 50  );
  
  // NEW: issue the warning about the algorihtms 
  if ( !m_builderName.empty() || m_builderType.empty() ) 
  {
    std::string msg = 
      "Obsolete properties 'AlgorithmName' and 'AlgorithmType'." ;
    msg += "Use new 'Data-On-Demand' Service: \n\n"   ;
    msg += " \tDataOnDemandSvc.Algorithms +={\"DATA='" ;
    if ( std::string::npos == location().find("/Event/") ) { msg += "/Event/" ; }
    msg += location() ;
    msg += "' TYPE='" + m_builderType + "/"+m_builderName+"'\"}; \n\n // " ;
    Warning( msg ) ;
  };
  
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
{ return Warning ( "The obsolete method 'locateOrBuilt' is invoked" ) ; } ;

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
  if ( 0 != m_counter ) { Warning ( "Mismatch in addRef/release" ) ; }
} ;
// ============================================================================

// ============================================================================
/** locate the algorithm for building the relations
 *  @return status code 
 */
// ============================================================================
StatusCode Relations::AssociatorBase::locateAlgorithm() const
{ return Warning ( "The obsolete 'locateLAgorithm' method is invoked" ) ; };
// ============================================================================

// ============================================================================
/** accessor to relations builder,
 *  locate algorthm, if not yet done 
 *  @see IAlgorithm
 *  @see AssociatorBase 
 *  @return poinere to relations builder
 */
// ============================================================================
IAlgorithm* Relations::AssociatorBase::algorithm () const 
{
  Warning ( "The obsolete method 'algorithm' is invoked" ) ;
  return 0 ;
};
// ============================================================================

// ============================================================================
/** accessor to relations table via interface, 
 *  locate or build the table, if not yet done 
 *  @see IInterface 
 *  @see AssociatorBase 
 *  @return poinere to relations table 
 */
// ============================================================================
IInterface* Relations::AssociatorBase::object    () const 
{
  Warning ( "The obsolete method 'object' is invoked") ;
  return 0 ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
