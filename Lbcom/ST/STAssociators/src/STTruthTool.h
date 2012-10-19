#ifndef STTRUTHTOOL_H
#define STTRUTHTOOL_H 1

/** @class STTruthTool STTruthTool.h
 *  
 *  Namespace for following links.....
 *  
 *  @author Matt Needham
 *  @date   26/04/2002
*/

#include <map>
#include "GaudiKernel/SystemOfUnits.h"

namespace LHCb{
  class MCParticle;
  class MCHit;
  class STDigit;
}

namespace STTruthTool{

  void associateToTruth(const LHCb::STDigit* aDigit,
                        std::map<const LHCb::MCHit*,double>& hitMap,
                        double& foundCharge);
  
  void associateToTruth(const LHCb::STDigit* aDigit,
                        std::map<const LHCb::MCParticle*,double>& particleMap);

  void removeDeltaRays(const std::map<const LHCb::MCHit*, double>& inputMap,
		       std::map<const LHCb::MCHit*,double>& hitMap);

  const static double drayTol = 0.02 * Gaudi::Units::mm;

}

#endif // STTRUTHTOOL_H
