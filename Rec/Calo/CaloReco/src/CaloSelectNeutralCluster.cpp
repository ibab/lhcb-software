// $Id: CaloSelectNeutralCluster.cpp,v 1.7 2006-11-28 13:15:16 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
//CaloUtils
#include "CaloUtils/Calo2Track.h"
// local 
#include "CaloSelectNeutralCluster.h"

// ============================================================================
/** @file 
 *
 *  implementation file for class CaloSelectNeutralCluster
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 26 Apr 2002 
 */
// ============================================================================

DECLARE_TOOL_FACTORY( CaloSelectNeutralCluster );

// ============================================================================
/** Standard constructor
 *  @see  GaudiTool
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
  : GaudiTool ( type , name , parent )
  , m_tableLocation (LHCb::CaloIdLocation::ClusterMatch ) 
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
 *  @see GaudiTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode 
CaloSelectNeutralCluster::initialize ()
{  
  // initialize the base class 
  StatusCode sc = GaudiTool::initialize () ;
  if( sc.isFailure() )
  {return Error("Could not initialize the base class GaudiTool!",sc); }
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
( const LHCb::CaloCluster* cluster ) const 
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
  ( const LHCb::CaloCluster* cluster   ) const 
{
  // check the cluster 
  if ( 0 == cluster ) { Warning ( "CaloCluster* points to NULL!" ) ; return false ; }
  
  // locate the table (if needed) 
  if ( 0 == m_table ) { m_table = get<LHCb::Calo2Track::IClusTrTable>( m_tableLocation ) ; }
  
  // get all relations with WEIGHT = 'chi2' under the threshold value 
  const LHCb::Calo2Track::IClusTrTable::Range range = m_table -> relations ( cluster , m_chi2cut , false ) ;

  return range.empty() ? true : false ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
