#include "GaudiKernel/SystemOfUnits.h"
#include "CaloUtils/CaloMomentum.h"
#include "Event/CaloCluster.h"
#include "CaloSelectCluster.h"

DECLARE_TOOL_FACTORY( CaloSelectCluster )

// ============================================================================
CaloSelectCluster::CaloSelectCluster
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool ( type , name , parent ) 
{
  //
  declareInterface<ICaloClusterSelector> ( this ) ;
  //
  declareProperty ("MinEnergy"        , m_cut  = 0. ) ;
  declareProperty ("MinEt"            , m_etCut  = 0. ) ;
  declareProperty ("MaxDigits"        , m_mult = 9999 ) ;
  declareProperty ("MinDigits"        , m_multMin = -9999 ) ;
}
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelectCluster::~CaloSelectCluster() {}

StatusCode CaloSelectCluster::initialize (){  
  // initialize the base class 
  StatusCode sc = GaudiTool::initialize () ;

  return sc;
}

// ============================================================================
/** @brief "select"  method 
 *
 *  Cluster is considered to be "selected" if there are Spd/Prs hit in front 
 *
 */
// ============================================================================
bool CaloSelectCluster::select( const LHCb::CaloCluster* cluster ) const{ 
return (*this) ( cluster ); 
}
// ============================================================================
bool CaloSelectCluster::operator()( const LHCb::CaloCluster* cluster   ) const{
  // check the cluster 
  if ( 0 == cluster ) { Warning ( "CaloCluster* points to NULL!" ).ignore() ; return false ; }
  double e = cluster->e();
  LHCb::CaloMomentum moment = LHCb::CaloMomentum(cluster);
  double et = moment.momentum().Pt();
  int m = cluster->entries().size();
  if ( UNLIKELY(msgLevel( MSG::DEBUG) ))debug() << "Cluster has " << m << " entries " 
                                      << " for a total energy of " << e <<  "(Et = " << et << ")" << endmsg;

  bool sel =  (e>m_cut) && (m<m_mult) && (et>m_etCut) && (m>m_multMin);


  counter("selected clusters") += (int) sel;
  return sel ;
}
