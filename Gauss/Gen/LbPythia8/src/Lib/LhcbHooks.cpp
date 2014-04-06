// Local.
#include "LbPythia8/LhcbHooks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LhcbHooks
//
// 2014-04-06 : Philip Ilten
//-----------------------------------------------------------------------------

namespace Pythia8 {
  double LhcbHooks::multiplySigmaBy(const SigmaProcess* sigmaProcessPtr,
				    const PhaseSpace* phaseSpacePtr, bool) {
    
    // Initialize
    if (!isInit) {
      pT20 = pow2(2.85);
      double alphaSvalue = settingsPtr->parm("SigmaProcess:alphaSvalue");
      int    alphaSorder = settingsPtr->mode("SigmaProcess:alphaSorder");
      alphaS.init(alphaSvalue, alphaSorder);
      // Use these lines for Pythia 8.185.
      //int    alphaSnfmax = settingsPtr->mode("StandardModel:alphaSnfmax");
      //alphaS.init(alphaSvalue, alphaSorder, alphaSnfmax, false);
      isInit = true;
    }
    
    // Calculate the weight.
    double wt = 1;
    
    // Charmonium and bottomonium weight.
    if (sigmaProcessPtr->code() > 400 && sigmaProcessPtr->code() < 600 &&
	sigmaProcessPtr->nFinal() == 2) {
      
      // pT scale of process. Weight pT^4 / (pT^2 + pT0^2)^2
      double pTHat     = phaseSpacePtr->pTHat();
      double pT2       = pTHat * pTHat;
      wt               = pow2( pT2 / (pT20 + pT2) );
      
      // Renormalization scale and assumed alpha_strong.
      double Q2RenOld  = sigmaProcessPtr->Q2Ren();
      double alphaSOld = sigmaProcessPtr->alphaSRen();
      
      // Reweight to new alpha_strong at new scale.
      double Q2RenNew  = pT20 + Q2RenOld;
      double alphaSNew = alphaS.alphaS(Q2RenNew);
      wt              *= pow2(alphaSNew / alphaSOld);
    }
    
    // Return weight.
    return wt;
  }
}
