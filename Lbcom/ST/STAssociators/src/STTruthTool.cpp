// $Id: STTruthTool.cpp,v 1.4 2009-02-02 15:37:40 mneedham Exp $

// Event
#include "Event/STCluster.h"
#include "Event/STDigit.h"
#include "Event/MCSTDigit.h"
#include "Event/MCSTDeposit.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/LineTypes.h"
#include "GaudiKernel/Point3DTypes.h"

// local
#include "STTruthTool.h"

using namespace LHCb;

void STTruthTool::associateToTruth(const STDigit* aDigit,
                                   std::map<const MCHit*,double>& hitMap,
                                   double& foundCharge)
{
  // make link to truth  to MCHit from Digit
  const MCSTDigit* mcDigit = mcTruth<MCSTDigit>(aDigit); 
  if (0 != mcDigit){
   
    // link to deposits
    std::map<const MCHit*,double> tempMap;
    SmartRefVector<MCSTDeposit> depCont = mcDigit->mcDeposit();
    SmartRefVector<MCSTDeposit>::iterator iterDep = depCont.begin();
    while (iterDep != depCont.end()){
      const MCHit* aHit = (*iterDep)->mcHit(); 
      foundCharge += (*iterDep)->depositedCharge(); 
      tempMap[aHit] += (*iterDep)->depositedCharge();  
      ++iterDep;
    }

    // clean out the delta-rays
    removeDeltaRays(tempMap, hitMap);
    double foundCharge2 = 0.0;
    std::map<const MCHit*, double>::const_iterator iter = tempMap.begin();
    for (; iter != tempMap.end(); ++iter){
      foundCharge2 += iter->second;   
    }  // iter  

  }
}

void STTruthTool::removeDeltaRays(const std::map<const MCHit*, double>& inputMap,
                                   std::map<const MCHit*,double>& hitMap){


  // separate into delta ray and not
  std::map<const MCHit*, double> deltaRays;
  std::map<const MCHit*, double>::const_iterator iter = inputMap.begin();
  for (; iter != inputMap.end(); ++iter){

    const MCHit* hit = iter->first;
    if (hit != 0){
      const MCParticle* aParticle = hit->mcParticle();
      if (aParticle && aParticle->originVertex() !=0 && 
          aParticle->originVertex()->type() == LHCb::MCVertex::DeltaRay){
        deltaRays.insert(*iter); 
      } 
      else {
        hitMap.insert(*iter);
      } 
    }
    else {
      hitMap.insert(*iter);
    }
  }  // iter 

  // for the delta rays add directly if they are far away from and not related to an existing 
  // MCHit, else merge with existing hit
  std::map<const MCHit*, double>::const_iterator drays = deltaRays.begin();
  for (; drays != deltaRays.end(); ++drays){

    std::map<const MCHit*, double>::iterator iter2 = hitMap.begin();
    for (; iter2 != hitMap.end() ; ++iter2){
      const MCHit* aHit = iter2->first;
      if (aHit == 0) continue;
      if (aHit->mcParticle() == drays->first->mcParticle()) break;
    } // iter2

    if (iter2 == hitMap.end() ){
       
      // no reasonable candidate --> insert
      hitMap.insert(*drays);
      }
    else {
      using namespace Gaudi::Math;
      Gaudi::XYZPoint dPoint = drays->first->midPoint();
      XYZLine hitLine = XYZLine(iter2->first->entry(), iter2->first->exit());
      const double distToLine = impactParameter(dPoint,hitLine);
      if (fabs(distToLine) < STTruthTool::drayTol){
	// its close, merge
        hitMap[iter2->first] += drays->second;
      }
      else {
        hitMap.insert(*drays);
      }
    } 

  } // delta rays

}

void STTruthTool::associateToTruth(const STDigit* aDigit,
                               std::map<const MCParticle*,double>& particleMap)
{
  // make truth link to MCParticle from Digit
  std::map<const MCHit*,double> hitMap;
  double foundCharge = 0.0;
  STTruthTool::associateToTruth(aDigit,hitMap,foundCharge);
  hitMap[0] += (aDigit->depositedCharge()-foundCharge);

  if ( !(hitMap.empty()) ) {
    std::map<const MCHit*,double>::const_iterator iterMap = hitMap.begin();
    while (iterMap != hitMap.end()){
      const MCHit* aHit = (*iterMap).first;
      double charge = (*iterMap).second;
      const MCParticle* aParticle = 0;
      if (0 != aHit){
        aParticle = aHit->mcParticle();
      }
      particleMap[aParticle] += charge;
      ++iterMap;
    }
  }
}










