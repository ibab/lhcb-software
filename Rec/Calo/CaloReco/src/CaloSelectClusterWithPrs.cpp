#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "CaloSelectClusterWithPrs.h"

DECLARE_TOOL_FACTORY( CaloSelectClusterWithPrs );

// ============================================================================
CaloSelectClusterWithPrs::CaloSelectClusterWithPrs
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool ( type , name , parent ) 
{
  //
  declareInterface<ICaloClusterSelector> ( this ) ;
  //
  declareProperty ("MinEnergy"        , m_cut = -10. *Gaudi::Units::MeV ) ;
  declareProperty ("MinMultiplicity"  , m_mult = 0. ) ;
  declareProperty ("Detector"           , m_det = "Ecal" );
};
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelectClusterWithPrs::~CaloSelectClusterWithPrs() {};

StatusCode CaloSelectClusterWithPrs::initialize (){  
  // initialize the base class 
  StatusCode sc = GaudiTool::initialize () ;
  //
  m_toPrs = tool<ICaloHypo2Calo>("CaloHypo2Calo", "CaloHypo2Prs");
  m_toPrs->setCalos(m_det,"Prs");
  return sc;
}; 

// ============================================================================
/** @brief "select"  method 
 *
 *  Cluster is considered to be "selected" if there are Spd/Prs hit in front 
 *
 */
// ============================================================================
bool CaloSelectClusterWithPrs::select( const LHCb::CaloCluster* cluster ) const{ 
return (*this) ( cluster ); 
};
// ============================================================================
bool CaloSelectClusterWithPrs::operator()( const LHCb::CaloCluster* cluster   ) const{
  // check the cluster 
  if ( 0 == cluster ) { Warning ( "CaloCluster* points to NULL!" ) ; return false ; }
  double ePrs = m_toPrs->energy( *cluster, "Prs");
  int mPrs = m_toPrs->multiplicity();
  if ( msgLevel( MSG::DEBUG) )debug() << "Found " << mPrs << "Prs hits " 
                                      << " for a total energy of " << ePrs <<  endreq;
  return (ePrs>m_cut) && (mPrs>m_mult);
};
