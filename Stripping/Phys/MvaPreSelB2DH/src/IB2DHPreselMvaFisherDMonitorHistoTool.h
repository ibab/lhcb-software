// $Id: $
#ifndef IB2DHPRESELMVAFISHERDMONITORHISTOTOOL_H 
#define IB2DHPRESELMVAFISHERDMONITORHISTOTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <Event/Particle.h>

static const InterfaceID IID_IB2DHPreselMvaFisherDMonitorHistoTool ( "IB2DHPreselMvaFisherDMonitorHistoTool", 1, 0 );

/** @class IB2DHPreselMvaFisherDMonitorHistoTool IB2DHPreselMvaFisherDMonitorHistoTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-07-19
 */
class IB2DHPreselMvaFisherDMonitorHistoTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IB2DHPreselMvaFisherDMonitorHistoTool; }
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
#endif // IB2DHPRESELMVAFISHERDMONITORHISTOTOOL_H
