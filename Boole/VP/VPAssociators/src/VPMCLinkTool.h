// $Id: VPMCLinkTool.h,v 1.1.1.1 2009-12-04 14:34:46 marcin Exp $
#ifndef VPMCLINKTOOL_H
#define VPMCLINKTOOL_H 1

/** @class VPMCLinkTool VPMCLinkTool.h
 *  @Based on ST code
 *  
 *  @author Marcin Kucharczyk
 *  @date   06/11/2009
*/

#include <map>
#include "GaudiKernel/SystemOfUnits.h"
#include "Event/MCVPDigit.h"

namespace LHCb {
  class MCParticle;
  class MCHit;
  class VPDigit;
};

namespace VPMCLinkTool {
  void associateToTruth(const LHCb::VPDigit* aDigit,
                        const LHCb::MCVPDigits* digitsMC,
                        std::map<const LHCb::MCHit*,double>& hitMap,
                        double& foundCharge);
  void removeDeltaRays(const std::map<const LHCb::MCHit*, double>& inputMap,
		                   std::map<const LHCb::MCHit*,double>& hitMap);
  void associateToTruth(const LHCb::VPDigit* aDigit,
                        const LHCb::MCVPDigits* digitsMC,
                        std::map<const LHCb::MCParticle*,double>& particleMap);
  const static double drayTol = 0.02 * Gaudi::Units::mm;
}; 

#endif // VPMCLINKTOOL_H
