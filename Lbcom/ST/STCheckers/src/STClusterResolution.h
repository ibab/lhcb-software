// $Id: STClusterResolution.h,v 1.2 2006-02-20 16:44:32 mneedham Exp $
#ifndef _STClusterResolution_H
#define _STClusterResolution_H

#include <vector>
#include <string>

// base class
#include "GaudiAlg/GaudiHistoAlg.h"

#include "Linker/LinkerTool.h"

namespace LHCb{
 class MCHit;
 class STCluster;
};

class DeSTDetector;
class DeSTSector;
class AIDA::IHistogram1D;
class STDetectionLayer;
class IMCParticleSelector;
class ISTClusterPosition;

/** @class STClusterResolution STClusterResolution.h STCheckers/STClusterResolution.h
 *
 *  Class for checking STClusters
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class STClusterResolution : public GaudiHistoAlg {

public:
 
  /// constructer
  STClusterResolution(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~STClusterResolution();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  virtual StatusCode initHistograms();
  virtual StatusCode fillHistograms(const LHCb::STCluster* aCluster,
                                    const LHCb::MCHit* aHit) const;
  int histoId(const int clusterSize) const;

  double calculateUTrue(const LHCb::MCHit* aHit, const DeSTSector* aSector) const;

  std::string m_detType;
  std::string m_clusterLocation;
  std::string m_asctLocation;
  DeSTDetector* m_tracker;

  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

  ISTClusterPosition* m_positionTool;
  std::string m_positionToolName;

  std::vector<AIDA::IHistogram1D*> m_resHistoVector;
  std::vector<AIDA::IHistogram1D*> m_pullHistoVector;


};

#endif // _STClusterResolution_H













