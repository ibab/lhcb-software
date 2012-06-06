#ifndef VELOLITE_CLUSTER_CREATOR_H
#define VELOLITE_CLUSTER_CREATOR_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event/MCEvent
#include "Event/MCVeloLiteDigit.h"
// Event/DigiEvent
#include "Event/VeloStripCluster.h"
// Det/VeloLiteDet
#include "VeloLiteDet/DeVeloLite.h"
// Local
#include "VeloLiteInternalCluster.h"

/** @class VeloLiteClusterCreator VeloLiteClusterCreator.h
 *
 * Create VeloStripClusters from MCVeloLiteDigits
 */

class IStripNoiseTool;

class VeloLiteClusterCreator : public GaudiAlgorithm {

public:
  /// Constructor
  VeloLiteClusterCreator(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VeloLiteClusterCreator() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  void makeClusters();
  StatusCode storeClusters();

  bool makeCluster(LHCb::MCVeloLiteDigit* digit, 
                   VeloLiteInternalCluster& cluster, double& stn);
  bool addCentralChannel(VeloLiteInternalCluster& cluster, double& stn, 
                         LHCb::MCVeloLiteDigit* digit);
  bool addAdjacentChannel(VeloLiteInternalCluster& cluster, double& stn,
                          LHCb::MCVeloLiteDigit* digit, int offset);
  void addChannel(VeloLiteInternalCluster& cluster, double& stn,
                  LHCb::MCVeloLiteDigit* digit, int offset);

  /// Reject a cluster allowing the digits in it to be used in other clusters
  void unmarkCluster(VeloLiteInternalCluster& cluster);

  void getRangeOfSensor(const unsigned int sensorId,
                        std::pair<LHCb::MCVeloLiteDigits::iterator, 
                                  LHCb::MCVeloLiteDigits::iterator>& range);

  /// Get the S/N for this channel 
  double signalToNoise(LHCb::MCVeloLiteDigit* digit);
  
  LHCb::MCVeloLiteDigits* m_digits; 
  LHCb::VeloStripClusters* m_clusters; 

  /// Pointer to detector element
  DeVeloLite* m_det;
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
