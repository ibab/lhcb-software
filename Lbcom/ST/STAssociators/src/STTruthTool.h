// $Id: STTruthTool.h,v 1.2 2006-02-07 08:46:39 mneedham Exp $
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
class STDigit;
};

namespace STTruthTool{


  StatusCode associateToTruth(const LHCb::STDigit* aDigit,
                              std::map<const LHCb::MCHit*,double>& hitMap,
                              double& foundCharge);
  
  StatusCode associateToTruth(const LHCb::STDigit* aDigit,
                              std::map<const LHCb::MCParticle*,double>& particleMap);

}; // STTRUTHTOOL_H

#endif
