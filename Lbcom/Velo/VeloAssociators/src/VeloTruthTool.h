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
#include "Event/InternalVeloCluster.h"
#include <map>

class MCHit;
class MCParticle;
class InternalVeloCluster;

namespace VeloTruthTool{

  /// make link between VeloCluster and MCVeloHit
  StatusCode associateToTruth(
             const LHCb::InternalVeloCluster* aCluster,
             std::map<LHCb::MCHit*,double>& hitMap,
             SmartDataPtr<LHCb::MCVeloFEs> mcfes);

  /// make link between VeloCluster and MCVeloHit
  StatusCode associateToTruth(
             const LHCb::InternalVeloCluster* aCluster,
             std::map<LHCb::MCHit*,double>& hitMap,
             LHCb::MCVeloFEs* mcfes);

}; // VELOTRUTHTOOL_H
#endif
