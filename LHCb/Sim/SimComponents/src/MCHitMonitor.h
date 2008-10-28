// $Id $
#ifndef _MCHitMonitor_H
#define _MCHitMonitor_H

#include "GaudiAlg/GaudiHistoAlg.h"
#include <string>
#include <vector>

namespace LHCb {
  class MCHit;
  class MCParticle;
}

namespace AIDA {
  class IHistogram1D;
  class IHistogram2D;
}

class IMCParticleSelector;

/** class TrMCHitMonitor, package TrMCHitMonitor
 *  Top Level Algorithm that manages MCHits digitization code
 *
 *  @author M.Needham
 *  @date   21/10/2000
*/

class MCHitMonitor: public GaudiHistoAlg {

public:

  /// Constructer
  MCHitMonitor(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructer
  ~MCHitMonitor();

  /// intialize
  virtual StatusCode initialize();

  /// execute
  virtual StatusCode execute();

private:

  /// init histograms
  StatusCode initHistograms();

  /// fill histograms
  StatusCode fillHistograms(LHCb::MCHit* aHit);
  
  /// station number for a given z
  int getStationID(const double z) const;
  
  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

  std::vector<IHistogram1D*> m_timeOfFlightHistos;
  std::vector<IHistogram2D*> m_XvsYHistos;
  std::vector<IHistogram1D*> m_EnergyLossHistos;
  std::vector<IHistogram1D*> m_pathHistos;
  std::vector<IHistogram1D*> m_betaGammaHistos;

  // job Options
  std::string m_MCHitPath;
  std::vector<double> m_Zstations;
  double m_Xmax;
  double m_Ymax;
  double m_ZTolerance;
  
};
 
#endif // TrMCHitMonitor_H
