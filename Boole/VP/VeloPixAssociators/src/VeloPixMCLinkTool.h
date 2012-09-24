// $Id: VeloPixMCLinkTool.h,v 1.1.1.1 2009-12-04 14:34:46 marcin Exp $
#ifndef VELOPIXMCLINKTOOL_H
#define VELOPIXMCLINKTOOL_H 1

/** @class VeloPixMCLinkTool VeloPixMCLinkTool.h
 *  @Based on ST code
 *  
 *  @author Marcin Kucharczyk
 *  @date   06/11/2009
*/

#include <map>
#include "GaudiKernel/SystemOfUnits.h"
#include "Event/MCVeloPixDigit.h"

namespace LHCb {
  class MCParticle;
  class MCHit;
  class VeloPixDigit;
};

namespace VeloPixMCLinkTool {
  void associateToTruth(const LHCb::VeloPixDigit* aDigit,
                        const LHCb::MCVeloPixDigits* digitsMC,
                        std::map<const LHCb::MCHit*,double>& hitMap,
                        double& foundCharge);
  void removeDeltaRays(const std::map<const LHCb::MCHit*, double>& inputMap,
		                   std::map<const LHCb::MCHit*,double>& hitMap);
  void associateToTruth(const LHCb::VeloPixDigit* aDigit,
                        const LHCb::MCVeloPixDigits* digitsMC,
                        std::map<const LHCb::MCParticle*,double>& particleMap);
  const static double drayTol = 0.02 * Gaudi::Units::mm;
}; 

#endif // VELOPIXMCLINKTOOL_H
