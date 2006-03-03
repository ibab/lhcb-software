// $Id: STMCTuner.h,v 1.3 2006-03-03 16:01:22 mneedham Exp $
#ifndef _STMCTuner_H
#define _STMCTuner_H

#include <vector>
#include <string>

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
 *  Class for checking STClusters
 *
 *  @author M.Needham
 *  @date   21/4/2001
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













