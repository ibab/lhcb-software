// $Id: STMCTuner.h,v 1.4 2006-12-21 17:54:48 jvantilb Exp $
#ifndef STMCTuner_H
#define STMCTuner_H 1

// base class
#include "GaudiAlg/GaudiHistoAlg.h"

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

/** @class STMCTuner STMCTuner.h
 *
 *  Checking class to plot S/N and charge of STClusters for each readout sector.
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   21/12/2006
 */

class STMCTuner : public GaudiHistoAlg {

public:
 
  /// constructer
  STMCTuner(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~STMCTuner();

  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  virtual StatusCode fillHistograms(const LHCb::STCluster* aCluster,
                                    const LHCb::MCHit* aHit);

  double betaGamma(const LHCb::MCParticle* aParticle) const;
  
  DeSTDetector* m_tracker;
  std::string m_detType;
  std::string m_clusterLocation; 
  std::string m_asctLocation; 

  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;

  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

};

#endif // _STMCTuner_H













