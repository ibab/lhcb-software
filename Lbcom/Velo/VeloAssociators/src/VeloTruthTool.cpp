#include "VeloTruthTool.h"
#include "VeloEvent/InternalVeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCHit.h"

#include <map>
#include <vector>
//-----------------------------------------------------------------------------
// Implementation file for namespace : VeloTruthTool
//
// 21/05/2002 : Chris Parkes & Tomasz Szumlak
//-----------------------------------------------------------------------------
StatusCode VeloTruthTool::associateToTruth(
                          const LHCb::InternalVeloCluster* aCluster,
                          std::map<LHCb::MCHit*,double>& hitMap, 
                          SmartDataPtr<LHCb::MCVeloFEs> mcfes){
  LHCb::MCVeloFEs* mcFEsPtr = mcfes;
  //
  return (VeloTruthTool::associateToTruth( aCluster, hitMap, mcFEsPtr ));
}
//
StatusCode VeloTruthTool::associateToTruth(
                          const LHCb::VeloDigit* aDigit,
                          std::map<LHCb::MCHit*, double>& hitMap,
                          SmartDataPtr<LHCb::MCVeloFEs> mcfes)
{
  LHCb::MCVeloFEs* mcFEsPtr=mcfes;
  //
  return ( associateToTruth(aDigit, hitMap, mcFEsPtr) );
}
//
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
//
StatusCode VeloTruthTool::associateToTruth(
                         const LHCb::VeloDigit* aDigit,
                         std::map<LHCb::MCHit*, double>& hitMap,
                         LHCb::MCVeloFEs* mcfes)
{
  // associations between digits and MCVeloFE objects are type 1..1
  // for each strip one can have only one VeloDigit as well as only
  // one MCVeloFE object
  unsigned int strip=static_cast<unsigned int>(aDigit->strip());
  unsigned int sensor=static_cast<unsigned int>(aDigit->sensor());
  //
  LHCb::VeloChannelID myChannelID(sensor, strip);
  LHCb::MCVeloFE* aFE=mcfes->object(myChannelID);
  //
  if(aFE!=NULL){
    // get info about hit from MCVeloFE object
    SmartRefVector<LHCb::MCHit> assctHits=aFE->mcHits();
    std::vector<double> assctHitsCharges=aFE->mcHitsCharge();
    int size=assctHits.size();
    for(int i=0; i<size; i++){
      hitMap[assctHits[i]]+=assctHitsCharges[i];
    }
  }
  //
  if(hitMap.empty()){ return ( StatusCode::FAILURE ); }
  //
  return ( StatusCode::SUCCESS );
}

