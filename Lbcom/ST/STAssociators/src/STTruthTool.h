// $Id: STTruthTool.h,v 1.3 2007-01-09 15:05:00 jvantilb Exp $
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

namespace LHCb{
  class MCParticle;
  class MCHit;
  class STDigit;
};

namespace STTruthTool{

  void associateToTruth(const LHCb::STDigit* aDigit,
                        std::map<const LHCb::MCHit*,double>& hitMap,
                        double& foundCharge);
  
  void associateToTruth(const LHCb::STDigit* aDigit,
                        std::map<const LHCb::MCParticle*,double>& particleMap);

}; 

#endif // STTRUTHTOOL_H
