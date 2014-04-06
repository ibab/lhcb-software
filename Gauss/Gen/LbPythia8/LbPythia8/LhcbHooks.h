#ifndef LBPYTHIA8_LHCBHOOKS_H 
#define LBPYTHIA8_LHCBHOOKS_H 1

#include "Pythia.h"

/** @class LhcbHooks LhcbHooks.h LbPythia8/LhcbHooks.h
 *
 *  Custom LHCb user hooks used to modify default Pythia 8 event generation.
 *
 *  @author Philip Ilten
 *  @date   2014-04-06
 */

namespace Pythia8 {
  class LhcbHooks : public UserHooks {
    
  public:
    
    // Constructor.
    LhcbHooks() {isInit = false;}
    
    // Modifiy cross-section.
    virtual bool canModifySigma() {return true;}
    
    // Cross-section weight.
    virtual double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr,
				   const PhaseSpace* phaseSpacePtr, bool);
    
  private:
    
    // Members.
    bool   isInit;
    double pT20;
    AlphaStrong alphaS;
  };
}

#endif // LBPYTHIA8_LHCBHOOKS_H
