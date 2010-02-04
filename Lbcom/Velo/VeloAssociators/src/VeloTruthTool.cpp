#include "VeloTruthTool.h"
#include "VeloEvent/InternalVeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCHit.h"
#include "Event/VeloCluster.h"

#include <map>
#include <vector>
//-----------------------------------------------------------------------------
// Implementation file for namespace : VeloTruthTool
//
// 21/05/2002 : Chris Parkes & Tomasz Szumlak
//-----------------------------------------------------------------------------

// //////////////////////////////////////// //
//  associate InternalVeloCluster -> MCHit
// //////////////////////////////////////// //
StatusCode VeloTruthTool::associateToTruth(
                          const LHCb::InternalVeloCluster* aCluster,
                          std::map<LHCb::MCHit*,double>& hitMap, 
                          SmartDataPtr<LHCb::MCVeloFEs> mcfes){
  LHCb::MCVeloFEs* mcFEsPtr = mcfes;
  //
  return (VeloTruthTool::associateToTruth( aCluster, hitMap, mcFEsPtr ));
}
// ////////////////////////////// //
//  associate VeloDigit -> MCHit
// ////////////////////////////// //
StatusCode VeloTruthTool::associateToTruth(
                          const LHCb::VeloDigit* aDigit,
                          std::map<LHCb::MCHit*, double>& hitMap,
                          SmartDataPtr<LHCb::MCVeloFEs> mcfes)
{
  LHCb::MCVeloFEs* mcFEsPtr=mcfes;
  //
  return ( associateToTruth(aDigit, hitMap, mcFEsPtr) );
}
// //////////////////////////////// //
//  associate VeloCluster -> MCHit
// //////////////////////////////// //
StatusCode VeloTruthTool::associateToTruth(
                          const LHCb::VeloCluster* aCluster,
                          std::map<LHCb::MCHit*, double>& hitMap,
                          SmartDataPtr<LHCb::MCVeloFEs> mcfes)
{
  LHCb::MCVeloFEs* mcFEsPtr=mcfes;
  //
  return ( VeloTruthTool::associateToTruth(aCluster, hitMap, mcFEsPtr) );
}
// //////////////////////////////////////// //
//  associate InternalVeloCluster -> MCHit 
// //////////////////////////////////////// //
StatusCode VeloTruthTool::associateToTruth(
                          const LHCb::InternalVeloCluster* aCluster,
                          std::map<LHCb::MCHit*,double>& hitMap,
                          LHCb::MCVeloFEs* mcfes){

  std::map<LHCb::MCHit*, double> tmpMap;
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
        tmpMap[aHitVector[i]] += aHitVectorCharge[i];
      }
    } // next FE
  } //next strip
  // return no link
  removeDeltaRays(tmpMap,hitMap);
  if (hitMap.empty()) return StatusCode::FAILURE;
  //
  return (StatusCode::SUCCESS);
}
// //////////////////////////////// //
//  associate VeloCluster -> MCHit
// //////////////////////////////// //
StatusCode VeloTruthTool::associateToTruth(
                          const LHCb::VeloCluster* aCluster,
                          std::map<LHCb::MCHit*, double>& hitMap,
                          LHCb::MCVeloFEs* mcfes)
{

  std::map<LHCb::MCHit*, double> tmpMap;
  // associate VeloCluster to the MCHit objects
  std::vector<LHCb::VeloChannelID> channels=aCluster->channels();
  int NChannels=channels.size();  
  for(int iChann=0; iChann<NChannels; iChann++){
    // relate FE object with each strip
    LHCb::VeloChannelID aChann=channels[iChann];
    LHCb::MCVeloFE* anFE=mcfes->object(aChann);
    //
    if(NULL!=anFE){
      // get hits related with given FE object
      SmartRefVector<LHCb::MCHit> aHitVec=anFE->mcHits();
      std::vector<double> aHitVecCharge=anFE->mcHitsCharge();
      int size=aHitVec.size();      
      for(int i=0; i<size; i++){
        tmpMap[aHitVec[i]]+=aHitVecCharge[i];
      }      
    }
  }
  removeDeltaRays(tmpMap,hitMap);
  if(hitMap.size()==0){
    return ( StatusCode::FAILURE );
  }
  //
  return ( StatusCode::SUCCESS );
}
// ////////////////////////////// //
//  associate VeloDigit -> MCHit
// ////////////////////////////// //
StatusCode VeloTruthTool::associateToTruth(
                         const LHCb::VeloDigit* aDigit,
                         std::map<LHCb::MCHit*, double>& hitMap,
                         LHCb::MCVeloFEs* mcfes)
{
  std::map<LHCb::MCHit*, double> tmpMap;
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
      tmpMap[assctHits[i]]+=assctHitsCharges[i];
    }
  }
  //
  removeDeltaRays(tmpMap,hitMap);
  if(hitMap.empty()){ return ( StatusCode::FAILURE ); }
  //
  return ( StatusCode::SUCCESS );
}
// ///////////////////////////////////////////////
// Correctly assign delta rays to parent MCParticle
// ///////////////////////////////////////////////
void VeloTruthTool::removeDeltaRays(const std::map<LHCb::MCHit*, double>& inputMap,
				    std::map<LHCb::MCHit*, double>& outputMap){
  
  // separate into delta ray and not
  std::map<LHCb::MCHit*, double> deltaRays;
  std::map<LHCb::MCHit*, double>::const_iterator iter = inputMap.begin();
  for (; iter != inputMap.end(); ++iter){
 
    const LHCb::MCHit* hit = iter->first;
    if (hit != 0){
      const LHCb::MCParticle* aParticle = hit->mcParticle();
      if (aParticle && aParticle->originVertex() !=0 && 
	  aParticle->originVertex()->type() == LHCb::MCVertex::DeltaRay){
	deltaRays.insert(*iter); 
      } else {
	outputMap.insert(*iter);
      } 
    } else {
      outputMap.insert(*iter); // non-existant MCHit (?!)
    }
  }  // iter 
 
  // for the delta rays add directly if they are far away from and not related to an existing 
  // MCHit, else merge with existing hit
  std::map<LHCb::MCHit*, double>::const_iterator drays = deltaRays.begin();
  for (; drays != deltaRays.end(); ++drays){
 
    std::map<LHCb::MCHit*, double>::iterator iter2 = outputMap.begin();
    for (; iter2 != outputMap.end() ; ++iter2){
      const LHCb::MCHit* aHit = iter2->first;
      if (aHit == 0) continue;
      if (aHit->mcParticle() == drays->first->mcParticle()) break;
    } // iter2
 
    if (iter2 == outputMap.end() ){
      // no reasonable candidate --> insert as seperate particle
      outputMap.insert(*drays);
    } else {
      // merge as delta ray energy in same cluster (or internal cluster, or digit) 
      // so assign to MCHit of particle that made delta ray instead of delta ray itself
      outputMap[iter2->first] += drays->second;
    }     
  } // delta rays

}
