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




