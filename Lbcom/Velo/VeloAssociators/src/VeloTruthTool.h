#ifndef VELOTRUTHTOOL_H
#define VELOTRUTHTOOL_H 1

/** @class VeloTruthTool VeloTruthTool.h
 *  
 *  Namespace for functions used in association
 *  
 *  @author Chris Parkes
 *  @date   21/05/2002
*/

#include "GaudiKernel/StatusCode.h"
#include "Event/MCVeloFE.h"
#include "GaudiKernel/SmartDataPtr.h"

class MCVeloHit;
class MCParticle;
class VeloCluster;

namespace VeloTruthTool{

  /// make link between VeloCluster and MCVeloHit
  StatusCode associateToTruth(const VeloCluster* aCluster,
                              MCVeloHit*& aHit,
			      double& purity,
                              SmartDataPtr<MCVeloFEs> mcfes);

  /// make link between VeloCluster and MCParticle
  StatusCode associateToTruth(const VeloCluster* aCluster,
                              MCParticle*& aParticle,
			      double& purity,
                              SmartDataPtr<MCVeloFEs> mcfes);
}; // VELOTRUTHTOOL_H

#endif
