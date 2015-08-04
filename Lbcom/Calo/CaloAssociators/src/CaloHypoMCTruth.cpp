// ============================================================================
// Include files 
#include "Relations/RelationWeighted1D.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/CaloHypo.h"
#include "Event/MCParticle.h"
#include "Linker/LinkerWithKey.h"
#include "CaloHypoMCTruth.h"

/** @class CaloHypoMCTruth CaloHypoMCTruth.cpp
 *
 *  Simple algorithm to build Linkers for CaloHypo -> MCParticle relations 
 *
 *
 *  @author Olivier Deschamps odescham@in2p3.fr
 *  @date 2012-0§-24 
 */

CaloHypoMCTruth::~CaloHypoMCTruth(){}
CaloHypoMCTruth::CaloHypoMCTruth( const std::string& name ,ISvcLocator*       pSvc ) 
  : GaudiAlgorithm ( name , pSvc )
{
  declareProperty ( "Cluster2MCTable" , m_cluster2MCLoc = "Relations/" + LHCb::CaloClusterLocation::Default ) ;
  declareProperty ( "CaloHypos"       , m_inputs ) ;

  using namespace LHCb::CaloHypoLocation;
  m_inputs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , Photons      ) ) ;
  m_inputs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , Electrons    ) ) ;
  m_inputs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , MergedPi0s   ) ) ;
  m_inputs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , SplitPhotons ) ) ;

}
// ============================================================================

// ============================================================================
// declare algorithm factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY(CaloHypoMCTruth)
// ============================================================================

// ============================================================================
/// algorithm execution 
// ============================================================================
StatusCode CaloHypoMCTruth::execute    (){
  //--- load cluster->MC
  const LHCb::Calo2MC::IClusterTable* cluster2MC  = getIfExists<LHCb::Calo2MC::IClusterTable>( m_cluster2MCLoc );
  if( NULL == cluster2MC ) 
    return Warning(" Cluster <-> MC relation table not found at " + m_cluster2MCLoc , StatusCode::FAILURE);
  
  
  
  for(std::vector<std::string>::iterator c = m_inputs.begin() ; m_inputs.end() != c ; ++c){
    std::string container = *c;
    LHCb::Calo2MC::HypoLink linker ( eventSvc () , msgSvc () , container );
    int links=0;
    const LHCb::CaloHypos* hypos = getIfExists<LHCb::CaloHypos>(container);
    if( NULL == hypos ){
      counter("No container " + container)+=1;
      continue;
    }
    for( LHCb::CaloHypos::const_iterator h = hypos->begin() ;hypos->end()!=h ; ++h){
      const LHCb::CaloHypo* hypo = *h;
      const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo( hypo );
      LHCb::Calo2MC::IClusterTable::Range cluster2MCRange = cluster2MC->relations( cluster );
      int hlinks=0;
      for( LHCb::Calo2MC::IClusterTable::Range::const_iterator ir = cluster2MCRange.begin();cluster2MCRange.end()!=ir;ir++){
        linker.link( hypo , ir->to() , ir->weight() );
        hlinks++;
        links++;
      }   
      counter("#links/hypo " + container ) += hlinks;
    }
    counter ("#links " + container) += links ;
  }
  return StatusCode::SUCCESS ;
  
}
// ============================================================================
