// $Id: STClusterResolution.h,v 1.8 2008-12-11 14:41:13 cattanem Exp $
#ifndef STClusterResolution_H
#define STClusterResolution_H 1

// base class
#include "Kernel/STHistoAlgBase.h"

#include "Linker/LinkerTool.h"

namespace LHCb{
  class MCHit;
  class STCluster;
};

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


  std::string m_clusterLocation;
  std::string m_asctLocation;

  // selector
  std::string m_selectorName;
  IMCParticleSelector* m_selector;

  ISTClusterPosition* m_positionTool;
  std::string m_positionToolName;

  ITrajPoca*         m_poca; ///< Pointer to the ITrajPoca interface

};

#endif // STClusterResolution_H

