// $Id: IB2DHMvaMonitorHistoTool.h,v 1.1 2010/06/10 12:42:22 seaso Exp $
#ifndef IB2DSHMVAMONITORHISTOTOOL_H 
#define IB2DHMVAMONITORHISTOTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <Event/Particle.h>


static const InterfaceID IID_IB2DHMvaMonitorHistoTool ( "IB2DHMvaMonitorHistoTool", 1, 0 );

/** @class IB2DHMvaMonitorHistoTool IB2DHMvaMonitorHistoTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2009-07-17
 */
class IB2DHMvaMonitorHistoTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IB2DHMvaMonitorHistoTool; }

  virtual void fillHistoForEventSelectionSteps(const LHCb::Particles* partsBs, int anumCand, int indexOfbestCand,
                                                                     std::vector<double> aProb,std::vector<double>aIps,
                                                                     std::vector<int> aCndNoSel, 
                                                                     std::vector<int> aCandPrelimSel,
                                                                     std::vector<int> aCandFisherSel,
                                                                     std::vector<int> aCandAuxSel,
                                                                     std::vector<int> aCandMassSel,
                                               std::vector<int> aCandIndex )=0;
  

protected:

private:

};
#endif // IBS2DSKFISHERDMONITORHISTOTOOL_H
