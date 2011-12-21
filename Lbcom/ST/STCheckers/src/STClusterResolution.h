#ifndef STClusterResolution_H
#define STClusterResolution_H 1

// base class
#include "Kernel/STHistoAlgBase.h"

#include "Linker/LinkerTool.h"

namespace LHCb{
  class MCHit;
  class STCluster;
}

namespace AIDA {
  class IHistogram1D;
}

class DeSTDetector;
class DeSTSector;
class DeSTSensor;
class STDetectionLayer;
class IMCParticleSelector;
class ISTClusterPosition;
class ITrajPoca;

/** @class STClusterResolution STClusterResolution.h
 *
 *  Class for plotting the resolution of STClusters. It makes the following
 *  histograms for 1, 2, 3 and 4 strip clusters:
 *  - Offline resolution (rec. minus true position)
 *  - Offline pull
 *  - Online resolution
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class STClusterResolution : public ST::HistoAlgBase {

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

  virtual void fillHistograms(const LHCb::STCluster* aCluster,
                              const LHCb::MCHit* aHit) const;
  int histoId(const int clusterSize) const;

  double calculateUTrue( const LHCb::MCHit* aHit, 
                         const DeSTSensor* aSector) const;


  // Associator location
  std::string m_asctLocation;

  // selector
  IMCParticleSelector* m_selector;

  // Position tool
  ISTClusterPosition* m_positionTool;

  // job options
  std::string m_selectorName;
  std::string m_positionToolName;
  bool m_mergeSplitClusters; ///< Consider only one STCluster in split cluster
  bool m_skipSplitClusters;  ///< Ignore all STClusters in a split cluster
  std::string m_clusterLocation;

  ITrajPoca*         m_poca; ///< Pointer to the ITrajPoca interface

};

#endif // STClusterResolution_H

