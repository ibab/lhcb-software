// $Id: VeloPixMCLinkTool.cpp,v 1.1.1.1 2009-12-04 14:34:46 marcin Exp $
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Event
#include "Event/VeloPixDigit.h"
#include "Event/MCVeloPixDigit.h"
#include "Event/MCVeloPixDeposit.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/LineTypes.h"
#include "GaudiKernel/Point3DTypes.h"
// Local
#include "VeloPixMCLinkTool.h"

using namespace LHCb;


void VeloPixMCLinkTool::associateToTruth(const VeloPixDigit* aDigit,
                                         const MCVeloPixDigits* digitsMC,
                                         std::map<const MCHit*,double>& hitMap,
                                         double& foundCharge)
{
  // Make link to truth to MCHit from Digit
  const MCVeloPixDigit* mcDigit = new MCVeloPixDigit();
  MCVeloPixDigits::const_iterator iterMC = digitsMC->begin();
  for(; iterMC != digitsMC->end(); ++iterMC) {
    if((*iterMC)->channelID() == aDigit->key()) mcDigit = *iterMC;
  }
  if(0 != mcDigit) {
    // Link to deposits
    std::map<const MCHit*,double> tempMap;
    SmartRefVector<MCVeloPixDeposit> depCont = mcDigit->mcDeposit();
    SmartRefVector<MCVeloPixDeposit>::iterator iterDep = depCont.begin();
    while(iterDep != depCont.end()) {
      const MCHit* aHit = (*iterDep)->mcHit(); 
      foundCharge += (*iterDep)->depositedCharge(); 
      tempMap[aHit] += (*iterDep)->depositedCharge();  
      ++iterDep;
    }
    // Clean out the delta-rays
    removeDeltaRays(tempMap,hitMap);
    double foundCharge2 = 0.0;
    std::map<const MCHit*, double>::const_iterator iter = tempMap.begin();
    for(; iter != tempMap.end(); ++iter){
      foundCharge2 += iter->second;   
    }
  }
}


void VeloPixMCLinkTool::removeDeltaRays(
                        const std::map<const MCHit*, double>& inputMap,
                        std::map<const MCHit*,double>& hitMap) {
  // Separate into delta ray and not
  std::map<const MCHit*, double> deltaRays;
  std::map<const MCHit*, double>::const_iterator iter = inputMap.begin();
  for(; iter != inputMap.end(); ++iter) {
    const MCHit* hit = iter->first;
    if(hit != 0) {
      const MCParticle* aParticle = hit->mcParticle();
      if(aParticle && aParticle->originVertex() !=0 && 
         aParticle->originVertex()->type() == LHCb::MCVertex::DeltaRay) {
        deltaRays.insert(*iter); 
      } 
      else {
        hitMap.insert(*iter);
      } 
    }
    else {
      hitMap.insert(*iter);
    }
  } 
  // For the delta rays add directly if they are far away from 
  // and not related to an existing MCHit, else merge with existing hit
  std::map<const MCHit*, double>::const_iterator drays = deltaRays.begin();
  for(; drays != deltaRays.end(); ++drays) {
    std::map<const MCHit*, double>::iterator iter2 = hitMap.begin();
    for(; iter2 != hitMap.end() ; ++iter2) {
      const MCHit* aHit = iter2->first;
      if(aHit == 0) continue;
      if(aHit->mcParticle() == drays->first->mcParticle()) break;
    }
    if(iter2 == hitMap.end()) {
      // No reasonable candidate --> insert
      hitMap.insert(*drays);
    } else {
      using namespace Gaudi::Math;
      Gaudi::XYZPoint dPoint = drays->first->midPoint();
      XYZLine hitLine = XYZLine(iter2->first->entry(),iter2->first->exit());
      const double distToLine = impactParameter(dPoint,hitLine);
      if(fabs(distToLine) < VeloPixMCLinkTool::drayTol) {
	// It's close, merge
        hitMap[iter2->first] += drays->second;
      } else {
        hitMap.insert(*drays);
      }
    } 
  }
}


void VeloPixMCLinkTool::associateToTruth(const VeloPixDigit* aDigit,
                        const MCVeloPixDigits* digitsMC,
                        std::map<const MCParticle*,double>& particleMap)
{
  // Make truth link to MCParticle from Digit
  std::map<const MCHit*,double> hitMap;
  double foundCharge = 0.0;
  VeloPixMCLinkTool::associateToTruth(aDigit,digitsMC,hitMap,foundCharge);
  double conversion = 2100.0;
  hitMap[0] += (aDigit->ToTValue() * conversion - foundCharge);
  if(!(hitMap.empty())) {
    std::map<const MCHit*,double>::const_iterator iterMap = hitMap.begin();
    while(iterMap != hitMap.end()) {
      const MCHit* aHit = (*iterMap).first;
      double charge = (*iterMap).second;
      const MCParticle* aParticle = 0;
      if(0 != aHit){
        aParticle = aHit->mcParticle();
      }
      particleMap[aParticle] += charge;
      ++iterMap;
    }
  }
}
