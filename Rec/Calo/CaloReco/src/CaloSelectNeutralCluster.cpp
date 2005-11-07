// $Id: CaloSelectNeutralCluster.cpp,v 1.4 2005-11-07 12:12:43 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/12/10 17:12:29  ibelyaev
//  steps towards 'Fast' Calo recontruction
//
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelationWeighted.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
// ============================================================================
// local 
// ============================================================================
#include "CaloSelectNeutralCluster.h"
// ============================================================================

// ============================================================================
/** @file 
 *
 *  implementation file for class CaloSelectNeutralCluster
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 26 Apr 2002 
 */
// ============================================================================

// ============================================================================
/** @var CaloSelectNeutralClusterFactory 
 *  mandatory instantiation of tool factory
 */
// ============================================================================
static const ToolFactory<CaloSelectNeutralCluster>         s_Factory ;
const       IToolFactory&CaloSelectNeutralClusterFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see  CaloTool
 *  @see   AlgTool
 *  @see  IAlgTool 
 *  @param type tool type (?)
 *  @param name name of the concrete instance 
 *  @param parent pointer to the parent 
 */
// ============================================================================
CaloSelectNeutralCluster::CaloSelectNeutralCluster
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type , name , parent )
  , m_tableLocation ("Rec/Calo/PhotonMatch") 
  , m_table              (    0 ) 
  , m_chi2cut             ( -100 )
{
  //
  declareInterface<ICaloClusterSelector> ( this ) ;
  declareInterface<IIncidentListener>    ( this ) ;
  //
  declareProperty ("Table"      , m_tableLocation ) ;
  declareProperty ("Cut"        , m_chi2cut       ) ;
};
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelectNeutralCluster::~CaloSelectNeutralCluster() {};
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode 
CaloSelectNeutralCluster::initialize ()
{  
  // initialize the base class 
  StatusCode sc = CaloTool::initialize () ;
  if( sc.isFailure() )
  {return Error("Could not initialize the base class CaloTool!",sc); }
  // 
  // subscribe the incident 
  IIncidentSvc* iSvc = incSvc() ;
  if ( 0 == iSvc ) { return Error("IIncidentSvc* point to NULL!") ; }
  // 
  iSvc -> addListener ( this , IncidentType::EndEvent   , 10 ) ;
  
  return StatusCode::SUCCESS ;
}; 
// ============================================================================

// ============================================================================
/** handle the incident 
 *  @param inc incident 
 */
// ============================================================================
void CaloSelectNeutralCluster::handle ( const Incident& /* inc */ ) 
{ m_table = 0 ; };
// ============================================================================

// ============================================================================
/** @brief "select"  method 
 *
 *  Cluster is considered to be "selected" 
 *  if there are no reconstructed tracks with 
 *  chi2 value for 2D-matching under the threshold value 
 *
 *  @see ICaloClusterSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelectNeutralCluster::select     
( const CaloCluster* cluster ) const 
{ return (*this) ( cluster ); };
// ============================================================================

// ============================================================================
/** @brief "select"  method (functor interface)
 *
 *  Cluster is considered to be "selected" 
 *  if there are no reconstructed tracks with 
 *  chi2 value for 2D-matching under the threshold value 
 *
 *  @see ICaloClusterSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelectNeutralCluster::operator() 
  ( const CaloCluster* cluster   ) const 
{
  // check the cluster 
  if ( 0 == cluster ) { Warning ( "CaloCluster* points to NULL!" ) ; return false ; }
  
  // locate the table (if needed) 
  if ( 0 == m_table ) { m_table = get<ITable>( m_tableLocation ) ; }
  
  // get all relations with WEIGHT = 'chi2' under the threshold value 
  const ITable::Range range = m_table -> relations ( cluster , m_chi2cut , false ) ;

  return range.empty() ? true : false ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
