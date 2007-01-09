// $Id: STTruthTool.cpp,v 1.3 2007-01-09 15:05:00 jvantilb Exp $

// Event
#include "Event/STCluster.h"
#include "Event/STDigit.h"
#include "Event/MCSTDigit.h"
#include "Event/MCSTDeposit.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"

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
    SmartRefVector<MCSTDeposit> depCont = mcDigit->mcDeposit();
    SmartRefVector<MCSTDeposit>::iterator iterDep = depCont.begin();
    while (iterDep != depCont.end()){
      const MCHit* aHit = (*iterDep)->mcHit(); 
      foundCharge += (*iterDep)->depositedCharge(); 
      hitMap[aHit] += (*iterDep)->depositedCharge();  
      ++iterDep;
    }
  }
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










