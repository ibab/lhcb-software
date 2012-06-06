#include "VeloLiteTruthTool.h"

//=============================================================================
/// Associate VeloStripCluster to MCHit
//=============================================================================
StatusCode VeloLiteTruthTool::associateToTruth(
    const LHCb::VeloStripCluster* cluster,
    std::map<LHCb::MCHit*, double>& hitMap,
    SmartDataPtr<LHCb::MCVeloLiteDigits> mcdigits) {

  LHCb::MCVeloLiteDigits* pdigits = mcdigits;
  return associateToTruth(cluster, hitMap, pdigits);

}


//=============================================================================
/// Associate VeloStripCluster to MCHit
//=============================================================================
StatusCode VeloLiteTruthTool::associateToTruth(
    const LHCb::VeloStripCluster* cluster,
    std::map<LHCb::MCHit*, double>& hitMap,
    LHCb::MCVeloLiteDigits* mcdigits) {

  std::map<LHCb::MCHit*, double> tmpMap;
  std::vector<LHCb::VeloLiteChannelID> channels = cluster->channels();
  unsigned int nChannels = channels.size();  
  for (unsigned int i = 0; i < nChannels; ++i) {
    // Relate digit object with each strip
    LHCb::VeloLiteChannelID channel = channels[i];
    LHCb::MCVeloLiteDigit* digit = mcdigits->object(channel);
    if (digit) {
      // Get hits related with given digit
      SmartRefVector<LHCb::MCHit> hits = digit->mcHits();
      std::vector<double> charges = digit->mcHitsCharge();
      for (unsigned int j = 0; j < hits.size(); ++j) {
        tmpMap[hits[j]] += charges[j];
      } 
    }
  }
  removeDeltaRays(tmpMap, hitMap);
  if (hitMap.size() == 0) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Correctly assign delta rays to parent MCParticle
//=============================================================================
void VeloLiteTruthTool::removeDeltaRays(
    const std::map<LHCb::MCHit*, double>& inputMap,
    std::map<LHCb::MCHit*, double>& outputMap) {
  
  // Separate into delta ray and not
  std::map<LHCb::MCHit*, double> deltaRays;
  std::map<LHCb::MCHit*, double>::const_iterator it;
  for (it = inputMap.begin(); it != inputMap.end(); ++it) {
    const LHCb::MCHit* hit = it->first;
    if (!hit) continue;
    const LHCb::MCParticle* particle = hit->mcParticle();
    if (particle && particle->originVertex() != 0 && 
        particle->originVertex()->type() == LHCb::MCVertex::DeltaRay) {
      deltaRays.insert(*it); 
    } else {
      outputMap.insert(*it);
    } 
  }
 
  // For the delta rays add directly if they are far away from 
  // and not related to an existing MCHit, else merge with existing hit
  std::map<LHCb::MCHit*, double>::const_iterator itd;
  for (itd = deltaRays.begin(); itd != deltaRays.end(); ++itd) {
    std::map<LHCb::MCHit*, double>::iterator ito;
    for (ito = outputMap.begin(); ito != outputMap.end(); ++ito) {
      const LHCb::MCHit* hit = ito->first;
      if (!hit) continue;
      if (hit->mcParticle() == itd->first->mcParticle()) break;
    }
    if (ito == outputMap.end()) {
      // No reasonable candidate. Insert as separate particle.
      outputMap.insert(*itd);
    } else {
      // Merge as delta ray energy in same cluster, i. e. assign 
      // to MCHit of particle that made delta ray instead of delta ray itself
      outputMap[ito->first] += itd->second;
    }     
  }

}
