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
#include "GaudiKernel/SmartRef.h"
#include <map>

class MCVeloHit;
class MCParticle;
class VeloCluster;

namespace VeloTruthTool{

  /// make link between VeloCluster and MCVeloHit
  StatusCode associateToTruth(const VeloCluster* aCluster,
                              std::map<MCVeloHit*,double>& hitMap,
                              SmartDataPtr<MCVeloFEs> mcfes);

}; // VELOTRUTHTOOL_H

#endif
