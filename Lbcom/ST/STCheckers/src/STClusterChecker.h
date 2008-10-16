// $Id: STClusterChecker.h,v 1.7 2008-10-16 13:10:34 mneedham Exp $
#ifndef STClusterChecker_H
#define STClusterChecker_H 1

// base class
#include "Kernel/STHistoAlgBase.h"

// from Associators
#include "Linker/LinkerTool.h"

class DeSTDetector;

namespace LHCb{
  class MCHit;
  class STCluster;
  class MCParticle;
};

class IMCParticleSelector;
class ISTSignalToNoiseTool;

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

  double betaGamma(const LHCb::MCParticle* aParticle) const;
  

  std::string m_clusterLocation; 
  std::string m_asctLocation; 

  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;

  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

};

#endif // STClusterChecker_H













