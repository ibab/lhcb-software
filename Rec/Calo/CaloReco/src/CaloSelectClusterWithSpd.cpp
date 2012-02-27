// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "CaloSelectClusterWithSpd.h"
// ============================================================================
/** @file
 *  Implementation file for class CaloSelectClusterWithSpd
 *  @date 2009-07-18 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
// standard constructor 
// ============================================================================
CaloSelectClusterWithSpd::CaloSelectClusterWithSpd
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool ( type , name , parent )
  , m_toSpd(0)
{
  //
  declareInterface<ICaloClusterSelector> ( this ) ;
  //
  declareProperty ( "MinMultiplicity"    , m_cut=0        ) ;
  declareProperty ( "Detector"           , m_det = "Ecal" ) ;
}
// ============================================================================
// virtual destructor  
// ============================================================================
CaloSelectClusterWithSpd::~CaloSelectClusterWithSpd() {}
// ============================================================================
// initialization 
// ============================================================================
StatusCode CaloSelectClusterWithSpd::initialize ()
{
  // initialize the base class 
  StatusCode sc = GaudiTool::initialize () ;  
  m_toSpd = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Spd" , this );
  m_toSpd->setCalos( m_det,"Spd");
  return sc ;
}
// ============================================================================
// the main method 
// ============================================================================
int CaloSelectClusterWithSpd::n_hits( const LHCb::CaloCluster& cluster ) const
{ return m_toSpd->multiplicity ( cluster, "Spd" ) ; }

