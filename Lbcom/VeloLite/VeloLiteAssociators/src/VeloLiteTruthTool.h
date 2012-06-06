#ifndef VELOLITE_TRUTH_TOOL_H
#define VELOLITE_TRUTH_TOOL_H 1

// STL
#include <map>
// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/StatusCode.h"
// Event/MCEvent
#include "Event/MCVeloLiteDigit.h"
#include "Event/MCHit.h"
// Event/DigiEvent
#include "Event/VeloStripCluster.h"

namespace VeloLiteTruthTool {

  /// Make link between VeloStripCluster and MCHit
  StatusCode associateToTruth(const LHCb::VeloStripCluster* cluster,
                              std::map<LHCb::MCHit*, double>& hitMap,
                              SmartDataPtr<LHCb::MCVeloLiteDigits> mcdigits);

  /// Make link between VeloStripCluster and MCHit
  StatusCode associateToTruth(const LHCb::VeloStripCluster* cluster,
                              std::map<LHCb::MCHit*, double>& hitMap,
                              LHCb::MCVeloLiteDigits* mcdigits);

  /// Correctly assign MCHits from delta rays to parent MCParticle
  void removeDeltaRays(const std::map<LHCb::MCHit*, double>& inputMap,
		       std::map<LHCb::MCHit*, double>& outputMap);
  
}; 
#endif
