#ifndef VL_CLUSTER_CREATOR_H
#define VL_CLUSTER_CREATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event/MCEvent
#include "Event/MCVLDigit.h"
// Event/DigiEvent
#include "Event/VLCluster.h"
// Det/VLDet
#include "VLDet/DeVL.h"
// Local
#include "VLInternalCluster.h"

/** @class VLClusterCreator VLClusterCreator.h
 *
 * Create VLClusters from MCVLDigits
 */

class IStripNoiseTool;

class VLClusterCreator : public GaudiAlgorithm {

public:
  /// Constructor
  VLClusterCreator(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VLClusterCreator() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  void makeClusters();
  StatusCode storeClusters();

  bool makeCluster(LHCb::MCVLDigit* digit, 
                   VLInternalCluster& cluster, double& stn);
  bool addCentralChannel(VLInternalCluster& cluster, double& stn, 
                         LHCb::MCVLDigit* digit);
  bool addAdjacentChannel(VLInternalCluster& cluster, double& stn,
                          LHCb::MCVLDigit* digit, int offset);
  void addChannel(VLInternalCluster& cluster, double& stn,
                  LHCb::MCVLDigit* digit, int offset);

  /// Reject a cluster allowing the digits in it to be used in other clusters
  void unmarkCluster(VLInternalCluster& cluster);

  void getRangeOfSensor(const unsigned int sensorId,
                        std::pair<LHCb::MCVLDigits::iterator, 
                                  LHCb::MCVLDigits::iterator>& range);

  /// Get the S/N for this channel 
  double signalToNoise(LHCb::MCVLDigit* digit);
  
  LHCb::MCVLDigits* m_digits; 
  LHCb::VLClusters* m_clusters; 

  /// Pointer to detector element
  DeVL* m_det;
  /// Tag channels of current sensor as used
  std::vector<bool> m_used; 
  /// S/N cut to apply to all detectors
  double m_signalToNoiseCut;
  /// S/N cut for clusters to apply to all detectors
  double m_clusterSignalToNoiseCut;
  ///  Absolute ADC cut for clusters to apply to all detectors
  double m_clusterAbsoluteADCCut;
  /// Maximum number of clusters to make per event
  unsigned int m_maxClusters; 
  /// Threshold for adding strips to a cluster
  double m_inclusionThreshold; 
  /// Spillover threshold 
  double m_highThreshold; 

  /// Tool for estimating strip noise
  IStripNoiseTool* m_StripNoiseTool;
  /// Number of electrons per ADC count
  double m_electronsPerADC;

  // Output level
  bool m_debug;
  bool m_verbose;
  
};
#endif 
