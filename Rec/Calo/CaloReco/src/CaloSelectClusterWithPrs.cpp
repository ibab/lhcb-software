#include "GaudiKernel/SystemOfUnits.h"
#include "CaloSelectClusterWithPrs.h"

DECLARE_TOOL_FACTORY( CaloSelectClusterWithPrs )

// ============================================================================
CaloSelectClusterWithPrs::CaloSelectClusterWithPrs
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool ( type , name , parent )
  , m_toPrs(0)
{
  //
  declareInterface<ICaloClusterSelector> ( this ) ;
  //
  declareProperty ("MinEnergy"        , m_cut = -10. *Gaudi::Units::MeV ) ;
  declareProperty ("MinMultiplicity"  , m_mult = 0. ) ;
  declareProperty ("Detector"         , m_det = "Ecal" );
}
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelectClusterWithPrs::~CaloSelectClusterWithPrs() {}

StatusCode CaloSelectClusterWithPrs::initialize (){  
  // initialize the base class 
  StatusCode sc = GaudiTool::initialize () ;
  //
  m_toPrs = tool<ICaloHypo2Calo>("CaloHypo2Calo", "CaloHypo2Prs");
  m_toPrs->setCalos(m_det,"Prs");
  return sc;
}

// ============================================================================
/** @brief "select"  method 
 *
 *  Cluster is considered to be "selected" if there are Spd/Prs hit in front 
 *
 */
// ============================================================================
bool CaloSelectClusterWithPrs::select( const LHCb::CaloCluster* cluster ) const{ 
return (*this) ( cluster ); 
}
// ============================================================================
bool CaloSelectClusterWithPrs::operator()( const LHCb::CaloCluster* cluster   ) const{
  // check the cluster 
  if ( 0 == cluster ) { Warning ( "CaloCluster* points to NULL!" ).ignore() ; return false ; }
  double ePrs = m_toPrs->energy( *cluster, "Prs");
  int mPrs = m_toPrs->multiplicity();
  if ( UNLIKELY(msgLevel( MSG::DEBUG) ))debug() << "Found " << mPrs << "Prs hits " 
                                      << " for a total energy of " << ePrs <<  endmsg;

  bool sel = (ePrs>m_cut) && (mPrs>m_mult) ;
  counter("selected clusters") += (int) sel;
  return sel;
}
