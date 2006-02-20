#include "VeloTruthTool.h"
#include "VeloEvent/InternalVeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCHit.h"

#include <map>
#include <vector>
//-----------------------------------------------------------------------------
// Implementation file for namespace : VeloTruthTool
//
// 21/05/2002 : Chris Parkes
//-----------------------------------------------------------------------------
StatusCode VeloTruthTool::associateToTruth(
                          const LHCb::InternalVeloCluster* aCluster,
                          std::map<LHCb::MCHit*,double>& hitMap, 
                          SmartDataPtr<LHCb::MCVeloFEs> mcfes){
  LHCb::MCVeloFEs* mcFEsPtr = mcfes;
  //
  return (VeloTruthTool::associateToTruth( aCluster, hitMap, mcFEsPtr ));
}

StatusCode VeloTruthTool::associateToTruth(
                          const LHCb::InternalVeloCluster* aCluster,
                          std::map<LHCb::MCHit*,double>& hitMap,
                          LHCb::MCVeloFEs* mcfes){
  // make link to truth to MCHit
  int NStrips=aCluster->size(); // number of strips in cluster
  for (int iStrip=0; iStrip<NStrips; iStrip++){
    // get MCVeloFE for strip
    LHCb::VeloChannelID channelID = aCluster->channelID(iStrip);
    LHCb::MCVeloFE* anFE = mcfes->object(channelID);
    //    
    if (NULL!=anFE){
      // from FE get hits and their deposited charge
      SmartRefVector<LHCb::MCHit> aHitVector =  anFE->mcHits();
      std::vector<double> aHitVectorCharge =  anFE->mcHitsCharge();
      int size=aHitVector.size();
      for( int i=0; i<size;i++){
        hitMap[aHitVector[i]] += aHitVectorCharge[i];
      }
    } // next FE
  } //next strip
  // return no link
  if (hitMap.empty()) return StatusCode::FAILURE;
  //
  return (StatusCode::SUCCESS);
}

