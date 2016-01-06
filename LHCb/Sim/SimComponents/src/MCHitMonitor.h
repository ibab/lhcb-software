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

/** class MCHitMonitor, package MCHitMonitor
 *  Top Level Algorithm that manages MCHits digitization code
 *
 *  @author M.Needham
 *  @date 04/12/2008 [revised] 
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

  /// finalize
  virtual StatusCode finalize();

private:

  /// init histograms
  void initHistograms();

  /// fill histograms
  StatusCode fillHistograms(const LHCb::MCHit* aHit) const;
  
  /// station number for a given z
  int getStationID(const double z) const;
  
  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

  std::vector<IHistogram1D*> m_timeOfFlightHistos;
  std::vector<IHistogram2D*> m_XvsYHistos;
  std::vector<IHistogram1D*> m_EnergyLossHistos;
  
  mutable std::vector<double> m_energyVec;

  // job Options
  std::string m_MCHitPath;
  std::vector<double> m_Zstations;
  double m_Xmax;
  double m_Ymax;
  double m_TMax;
  double m_TMin;
  double m_EMax;
  double m_MaxPath;
  double m_ZTolerance;
  double m_minPathLength;
  unsigned int m_nToCollect;
  
};
 
#endif // TrMCHitMonitor_H
