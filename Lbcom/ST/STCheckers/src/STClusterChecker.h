#ifndef STClusterChecker_H
#define STClusterChecker_H 1

// base class
#include "Kernel/STHistoAlgBase.h"

// from Associators
#include "Linker/LinkerTool.h"

// LHCbKernel
#include "Kernel/LHCbConstants.h"
#include "GaudiKernel/SystemOfUnits.h"

class DeSTDetector;

namespace LHCb{
  class MCHit;
  class STCluster;
  class MCParticle;
}

class IMCParticleSelector;


/** @class STClusterChecker STClusterChecker.h
 *
 *  Checking class to plot S/N and charge of STClusters for each readout sector.
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   21/12/2006
 */

class STClusterChecker : public ST::HistoAlgBase {

public:
 
  /// constructer
  STClusterChecker(const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~STClusterChecker();

  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  void fillHistograms(const LHCb::STCluster* aCluster, const LHCb::MCHit* aHit);
  std::string m_clusterLocation; 
  std::string m_asctLocation; 


  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

  double m_betaMin;
  double m_gammaMin;

};



#endif // STClusterChecker_H













