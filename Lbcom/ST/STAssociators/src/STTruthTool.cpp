
#include "STTruthTool.h"
#include "Event/STCluster.h"
#include "Event/STDigit.h"
#include "Event/MCSTDigit.h"
#include "Event/MCSTDeposit.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"

#include <map>

StatusCode STTruthTool::associateToTruth(const LHCb::STCluster* aCluster,
                                         std::map<const LHCb::MCHit*,double>& hitMap){
  // make link to truth  to MCHit from cluster
 
  double foundCharge = 0.;
  double totalCharge  =0.;

  SmartRefVector<LHCb::STDigit> digitCont = aCluster->digits();
  SmartRefVector<LHCb::STDigit>::iterator iterDigit = digitCont.begin();
  while ((iterDigit != digitCont.end())){

    STTruthTool::associateToTruth(*iterDigit,hitMap,foundCharge);    
    totalCharge += (*iterDigit)->depositedCharge();
    ++iterDigit;
  } // Digit

  // difference between depEnergy and total cluster charge is noise
  hitMap[0] += (totalCharge-foundCharge);

  return StatusCode::SUCCESS;
}

StatusCode STTruthTool::associateToTruth(const LHCb::STCluster* aCluster,
                                         std::map<const LHCb::MCParticle*,double>& particleMap){

  // make truth link to LHCb::MCParticle from cluster
  StatusCode sc = StatusCode::SUCCESS;    

  // first to hit
  std::map<const LHCb::MCHit*,double> hitMap;
  sc = STTruthTool::associateToTruth(aCluster,hitMap);
  
  if (sc.isSuccess()&&(hitMap.empty() == false)){
    std::map<const LHCb::MCHit*,double>::iterator iterMap = iterMap = hitMap.begin();
    while (iterMap != hitMap.end()){
      const LHCb::MCHit* aHit = (*iterMap).first;
      double charge = (*iterMap).second;
      const LHCb::MCParticle* aParticle = 0;
      if (0 != aHit){
        aParticle = aHit->mcParticle();
      }
      particleMap[aParticle] += charge;
      ++iterMap;
    }
  }

  return sc;
}

StatusCode STTruthTool::associateToTruth(const LHCb::STDigit* aDigit,
                                         std::map<const LHCb::MCHit*,double>& hitMap,
                                         double& foundCharge){
  // make link to truth  to LHCb::MCHit from Digit
  StatusCode sc = StatusCode::SUCCESS;  

  const LHCb::MCSTDigit* mcDigit = mcTruth<LHCb::MCSTDigit>(aDigit); 
  if (0 != mcDigit){

    // link to deposits
    SmartRefVector<LHCb::MCSTDeposit> depCont = mcDigit->mcDeposit();
    SmartRefVector<LHCb::MCSTDeposit>::iterator iterDep = depCont.begin();
    while (iterDep != depCont.end()){

    // get hit
    const LHCb::MCHit* aHit = (*iterDep)->mcHit(); 
    foundCharge += (*iterDep)->depositedCharge(); 
    hitMap[aHit] += (*iterDep)->depositedCharge();  
    ++iterDep;

    } // iterClus
  }

  return sc;
}

StatusCode STTruthTool::associateToTruth(const LHCb::STDigit* aDigit,
                                         std::map<const LHCb::MCParticle*,double>& particleMap){

  // make truth link to LHCb::MCParticle from Digit
  StatusCode sc = StatusCode::SUCCESS;    

  std::map<const LHCb::MCHit*,double> hitMap;
  double foundCharge = 0.0;
  sc = STTruthTool::associateToTruth(aDigit,hitMap,foundCharge);
  hitMap[0] += (aDigit->depositedCharge()-foundCharge);

  if (sc.isSuccess()&&(hitMap.empty() == false)){
    std::map<const LHCb::MCHit*,double>::const_iterator iterMap = iterMap = hitMap.begin();
    while (iterMap != hitMap.end()){
      const LHCb::MCHit* aHit = (*iterMap).first;
      double charge = (*iterMap).second;
      const LHCb::MCParticle* aParticle = 0;
      if (0 != aHit){
        aParticle = aHit->mcParticle();
      }
      particleMap[aParticle] += charge;
      ++iterMap;
    }
  }

  return sc;
}










