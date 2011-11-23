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
#include "Event/MCHit.h"
#include "Event/VeloDigit.h"
#include "GaudiKernel/SmartDataPtr.h"
#include <map>

namespace LHCb 
{
  class MCHit;
  class InternalVeloCluster;
  class VeloDigit;
  class VeloCluster;
}

namespace VeloTruthTool{

  /// make link between InternalVeloCluster and MCHit
  StatusCode associateToTruth(
             const LHCb::InternalVeloCluster* aCluster,
             std::map<LHCb::MCHit*, double>& hitMap,
             SmartDataPtr<LHCb::MCVeloFEs> mcfes);

  /// make link between InternalVeloCluster and MCHit
  StatusCode associateToTruth(
             const LHCb::InternalVeloCluster* aCluster,
             std::map<LHCb::MCHit*, double>& hitMap,
             LHCb::MCVeloFEs* mcfes);

  /// make link between VeloCluster and MCHit
  StatusCode associateToTruth(
             const LHCb::VeloCluster* aCluster,
             std::map<LHCb::MCHit*, double>& hitMap,
             SmartDataPtr<LHCb::MCVeloFEs> mcfes);

  /// make link between VeloCluster and MCHit
  StatusCode associateToTruth(
             const LHCb::VeloCluster* aCluster,
             std::map<LHCb::MCHit*, double>& hitMap,
             LHCb::MCVeloFEs* mcfes);

  /// make link between VeloDigits and MCHits
  StatusCode associateToTruth(
             const LHCb::VeloDigit* aDigit,
             std::map<LHCb::MCHit*, double>& hitMap,
             SmartDataPtr<LHCb::MCVeloFEs> mcfes);

  /// make link between VeloDigits and MCHits
  StatusCode associateToTruth(
             const LHCb::VeloDigit* aDigit,
             std::map<LHCb::MCHit*, double>& hitMap,
             LHCb::MCVeloFEs* mcfes);

  /// correctly assign MCHits from delta rays to parent MCParticle
  void removeDeltaRays(const std::map<LHCb::MCHit*, double>& inputMap,
		       std::map<LHCb::MCHit*, double>& outputMap);
  
} // VELOTRUTHTOOL_H
#endif
