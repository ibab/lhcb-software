#include "CLHEP/Geometry/Point3D.h"

#include "VeloTruthTool.h"
#include "Event/VeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCVeloHit.h"
#include "Event/MCParticle.h"
#include "VeloDet/DeVelo.h"
#include "VeloKernel/VeloSimParams.h"

#include <map>
#include <vector>
//-----------------------------------------------------------------------------
// Implementation file for namespace : VeloTruthTool
//
// 21/05/2002 : Chris Parkes
//-----------------------------------------------------------------------------
StatusCode VeloTruthTool::associateToTruth(const VeloCluster* aCluster,
                                           std::map<MCVeloHit*,double>& hitMap,
                                           SmartDataPtr<MCVeloFEs> mcfes){
  // make link to truth to MCHit

  int NStrips=aCluster->size(); // number of strips in cluster
  for (int iStrip=0; iStrip<NStrips; iStrip++){

    // get MCVeloFE for strip
    VeloChannelID channelID = aCluster->channelID(iStrip);
    MCVeloFE* anFE = mcfes->object(channelID);
    
    if (NULL!=anFE){
      // from FE get hits and their deposited charge
      SmartRefVector<MCVeloHit> aHitVector =  anFE->mcVeloHits();
      std::vector<double> aHitVectorCharge =  anFE->mcVeloHitsCharge();

      int size=aHitVector.size();
      for( int i=0; i<size;i++){
        hitMap[aHitVector[i]] += aHitVectorCharge[i];
      }

    } // next FE
  } //next strip

  // return no link
  if (hitMap.empty()) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;

}

StatusCode VeloTruthTool::associateToTruth(const VeloCluster* aCluster,
                          std::map<MCParticle*,double>& particleMap,
                                         SmartDataPtr<MCVeloFEs> mcfes){
  // make truth link to MCParticle
  StatusCode sc = StatusCode::SUCCESS;

  // first to hits
  std::map<MCVeloHit*,double> hitMap;
  sc = VeloTruthTool::associateToTruth(aCluster,hitMap,mcfes);

  // now to particles
  if (sc.isSuccess()&&(!hitMap.empty())){
    std::map<MCVeloHit*,double>::iterator iterMap;
    for (iterMap = hitMap.begin(); iterMap != hitMap.end(); iterMap++){
      SmartRef<MCVeloHit> aHit = (*iterMap).first;
      double charge = (*iterMap).second;
      SmartRef<MCParticle> aParticle = aHit->mcParticle();
      particleMap[aParticle] += charge;
    }
  }

  return sc;
}



