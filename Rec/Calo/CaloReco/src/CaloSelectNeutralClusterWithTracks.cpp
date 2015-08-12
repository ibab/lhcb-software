// ============================================================================
//CaloUtils
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloAlgUtils.h"
// local 
#include "CaloSelectNeutralClusterWithTracks.h"

// ============================================================================
/** @file 
 *
 *  implementation file for class CaloSelectNeutralClusterWithTracks
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 26 Apr 2002 
 */
// ============================================================================

DECLARE_TOOL_FACTORY( CaloSelectNeutralClusterWithTracks )

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
CaloSelectNeutralClusterWithTracks::CaloSelectNeutralClusterWithTracks
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool ( type , name , parent )
  , m_tableLocation (LHCb::CaloIdLocation::ClusterMatch ) 
  , m_chi2cut             ( -100 )
  , m_tables              (    NULL ) 
{
  //
  declareInterface<ICaloClusterSelector> ( this ) ;
  //
  declareProperty ("Table"      , m_tableLocation ) ;
  declareProperty ("MinChi2"    , m_chi2cut       ) ;
  m_tableLocation = LHCb::CaloAlgUtils::CaloIdLocation( "ClusterMatch", context() );
}
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelectNeutralClusterWithTracks::~CaloSelectNeutralClusterWithTracks() {}
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
CaloSelectNeutralClusterWithTracks::initialize ()
{  
  // initialize the base class 
  StatusCode sc = GaudiTool::initialize () ;
  if( sc.isFailure() )
  {return Error("Could not initialize the base class GaudiTool!",sc); }
  m_tables = tool<ICaloRelationsGetter>("CaloRelationsGetter","CaloRelationsGetter",this);  
  return StatusCode::SUCCESS ;
}

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
bool CaloSelectNeutralClusterWithTracks::select     
( const LHCb::CaloCluster* cluster ) const 
{ return (*this) ( cluster ); }
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
bool CaloSelectNeutralClusterWithTracks::operator() 
  ( const LHCb::CaloCluster* cluster   ) const 
{
  // check the cluster 
  if ( 0 == cluster ) { Warning ( "CaloCluster* points to NULL!" ).ignore() ; return false ; }
  
  // locate the table 
  LHCb::Calo2Track::IClusTrTable* table = m_tables->getClusTrTable( m_tableLocation );
  if( NULL == table )return true;

  // get all relations with WEIGHT = 'chi2' under the threshold value 
  const LHCb::Calo2Track::IClusTrTable::Range range = table -> relations ( cluster , m_chi2cut , false ) ;

  bool sel = range.empty() ? true : false;
  counter("selected clusters") += (int) sel;

  return  sel;
}
