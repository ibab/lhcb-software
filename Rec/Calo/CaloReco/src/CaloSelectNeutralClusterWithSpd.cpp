#include "GaudiKernel/ToolFactory.h"
#include "CaloSelectNeutralClusterWithSpd.h"


DECLARE_TOOL_FACTORY( CaloSelectNeutralClusterWithSpd );

// ============================================================================
CaloSelectNeutralClusterWithSpd::CaloSelectNeutralClusterWithSpd
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool ( type , name , parent ) 
{
  //
  declareInterface<ICaloClusterSelector> ( this ) ;
  //
  declareProperty ("MinMultiplicity"    , m_cut=0       ) ;
};
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelectNeutralClusterWithSpd::~CaloSelectNeutralClusterWithSpd() {};

StatusCode CaloSelectNeutralClusterWithSpd::initialize (){  
  // initialize the base class 
  StatusCode sc = GaudiTool::initialize () ;  
  m_toSpd = tool<ICaloHypo2Calo>("CaloHypo2Calo", "CaloHypo2Spd");
  return sc ;

}; 

// ============================================================================
/** @brief "select"  method 
 *
 *  Cluster is considered to be "selected" if there are Spd/Prs hit in front 
 *
 */
// ============================================================================
bool CaloSelectNeutralClusterWithSpd::select( const LHCb::CaloCluster* cluster ) const{ 
return (*this) ( cluster ); 
};
// ============================================================================
bool CaloSelectNeutralClusterWithSpd::operator()( const LHCb::CaloCluster* cluster   ) const{
  // check the cluster 
  if ( 0 == cluster ) { Warning ( "CaloCluster* points to NULL!" ) ; return false ; }
  int nSpd = m_toSpd->multiplicity( *cluster, "Spd");
  if ( msgLevel( MSG::DEBUG) )debug() << "Found " << nSpd << " Spd hits" << endreq;
  return (nSpd>m_cut);
};
