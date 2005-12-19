// $Id: STTruthTool.h,v 1.1.1.1 2005-12-19 15:42:42 mneedham Exp $
#ifndef STTRUTHTOOL_H
#define STTRUTHTOOL_H 1

/** @class STTruthTool STTruthTool.h
 *  
 *  Namespace for following links.....
 *  
 *  @author Matt Needham
 *  @date   26/04/2002
*/

#include "GaudiKernel/StatusCode.h"
#include <map>


namespace LHCb{
class MCHit;
class MCParticle;
class STCluster;
class STDigit;
};

namespace STTruthTool{

  StatusCode associateToTruth(const LHCb::STCluster* aCluster,
                              std::map<const LHCb::MCHit*,double>& hitMap);
  
  StatusCode associateToTruth(const LHCb::STCluster* aCluster,
                              std::map<const LHCb::MCParticle*,double>& particleMap);

  StatusCode associateToTruth(const LHCb::STDigit* aDigit,
                              std::map<const LHCb::MCHit*,double>& hitMap,
                              double& foundCharge);
  
  StatusCode associateToTruth(const LHCb::STDigit* aDigit,
                              std::map<const LHCb::MCParticle*,double>& particleMap);

}; // STTRUTHTOOL_H

#endif
