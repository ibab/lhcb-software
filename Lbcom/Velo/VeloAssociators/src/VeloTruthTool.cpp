#include "CLHEP/Geometry/Point3D.h"

#include "VeloTruthTool.h"
#include "Event/VeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCVeloHit.h"
#include "Event/MCParticle.h"
#include "VeloDet/DeVelo.h"
#include "VeloKernel/VeloSimParams.h"

#include <map>
//-----------------------------------------------------------------------------
// Implementation file for namespace : VeloTruthTool
//
// 21/05/2002 : Chris Parkes
//-----------------------------------------------------------------------------
StatusCode VeloTruthTool::associateToTruth(const VeloCluster* aCluster,
                                           MCVeloHit*& aHit,
                                           double& purity,
                                           SmartDataPtr<MCVeloFEs> mcfes){
  // make link to truth to MCHit

  std::map<MCVeloHit*,double> hitMap;
  double totEnergy = 0.;

  int NStrips=aCluster->size(); // number of strips in cluster
  for (int iStrip=0; iStrip<NStrips; iStrip++){

    // get MCVeloFE for strip
    VeloChannelID channelID = aCluster->channelID(iStrip);
    MCVeloFE* anFE = mcfes->object(channelID);
    if (NULL!=anFE){
      // from FE get hits
      SmartRefVector<MCVeloHit> aHitVector =  anFE->mcVeloHits();
      MCVeloHit* aHit=NULL;
      if (aHitVector.size()==1) {aHit=(*(aHitVector.begin()));}
      else {
        // following needs code not yet committed
        // to attempt to solve ambiguities between MCVeloHits and FEs
        // currently just take first hit
        if (aHitVector.size()!=0) aHit=(*(aHitVector.begin()));

        // rare case of two MCHits contributing to the same strip
        // pick closest hit at centre of silicon and assign full signal to this
        // double distance=9999.;
        // for( SmartRefVector<MCVeloHit>::iterator hitIt = aHitVector.begin();
        //      hitIt< aHitVector.end(); hitIt++){
        //   HepPoint3D point=(*hitIt)->entry()+(*hitIt)->exit();
        //   point/=2;
        //   double fraction,pitch; bool valid;
        //   VeloChannelID chan=deVelo->channelID(point,fraction,pitch,valid);
        //   int Idiff=deVelo->neighbour(anFE->key(),chan,valid);
        //   if ((float(Idiff)+fraction) < distance) {
        //     distance=float(Idiff)+fraction;
        //     aHit=(*hitIt);
        //   }
        // } // hit loop
      } // end else

      if (NULL!=aHit){
        // update map for this hit
        double depEnergy = anFE->addedSignal();
        hitMap[aHit] += depEnergy;
        totEnergy += depEnergy;
      }
    } // next FE
  } //next strip

  // check total signal is significant - otherwise return no link
  if (totEnergy<VeloSimParams::threshold) return StatusCode::FAILURE;

  // iterate over map and find best match
  std::map<MCVeloHit*,double>::iterator iterMap;
  std::pair<MCVeloHit*,double> bestMatch(0,0.);
  for (iterMap = hitMap.begin(); iterMap != hitMap.end(); ++iterMap){
    if ((*iterMap).second>bestMatch.second) {
      bestMatch = *iterMap;
    }
  } //iterMap

  // estimate purity
  aHit = bestMatch.first;
  purity = fabs(bestMatch.second/totEnergy);

  return StatusCode::SUCCESS;

}

StatusCode VeloTruthTool::associateToTruth(const VeloCluster* aCluster,
                                           MCParticle*& aParticle,
                                           double& purity,
                                           SmartDataPtr<MCVeloFEs> mcfes){
  // make truth link to MCParticle
  StatusCode sc = StatusCode::SUCCESS;

  // first to hit
  MCVeloHit* aHit = 0;
  sc = VeloTruthTool::associateToTruth(aCluster,aHit,purity,mcfes);

  if ((sc.isSuccess())&&(0 != aHit)){
    aParticle = aHit->mcParticle();
  }

  return sc;
}

