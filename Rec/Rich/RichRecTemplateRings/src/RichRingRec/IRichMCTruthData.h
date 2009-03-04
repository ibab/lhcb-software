// $Id: IRichMCTruthData.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHRINGREC_IRICHMCTRUTHDATA_H 
#define RICHRINGREC_IRICHMCTRUTHDATA_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRichMCTruthData ( "IRichMCTruthData", 1, 0 );

/** @class IRichMCTruthData IRichMCTruthData.h RichRingRec/IRichMCTruthData.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-26
 */
class IRichMCTruthData : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichMCTruthData; }

  virtual void ResetMCTruthArrays()=0;
  
  virtual void  StoreBGPid(int ns,  VD aBeta, VD aGamma, VD aPid )=0;
  
  virtual void StoreTrueHit(VD rus, VD nus )=0;
  virtual double MCTrueRadiusValue(int i, int irad) =0; 
  virtual double MCTrueNumHitValue(int i, int irad)=0;
  virtual double MCTrueBetaValue(int i, int irad) =0;
  virtual double MCTrueGammaValue(int i, int irad)=0;
  virtual double MCTruePidValue (int i, int irad) =0;
  virtual int numTrueTkInfoValue(int i) =0;  
protected:

private:

};
#endif // RICHRINGREC_IRICHMCTRUTHDATA_H
