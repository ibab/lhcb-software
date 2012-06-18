#ifndef VL_CLUSTER_CREATOR_H
#define VL_CLUSTER_CREATOR_H 1

// STL
#include <vector>
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event/MCEvent
#include "Event/MCVLDigit.h"
// Event/DigiEvent
#include "Event/VLCluster.h"
// Event/LinkerEvent
#include "Linker/LinkerWithKey.h"

/** @class VLClusterCreator VLClusterCreator.h
 *
 * Create VLClusters from MCVLDigits
 */

class DeVL;
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
  /// Try to create a cluster from a given digit.
  bool makeCluster(LHCb::MCVLDigit* digit);
  /// Try to add a digit to an existing cluster candidate.
  bool addChannel(LHCb::MCVLDigit* digit, int offset);
  /// Get iterators to the first and last digit on a given sensor.
  void getRange(const unsigned int sensor,
                std::pair<LHCb::MCVLDigits::iterator,
                          LHCb::MCVLDigits::iterator>& range);
  /// Calculate the S/N ratio for a given digit.
  double signalToNoise(LHCb::MCVLDigit* digit);
  /// Link a cluster to the underlying MC hits and particles
  void linkCluster(LHCb::VLCluster* cluster,     
                   LinkerWithKey<LHCb::MCHit, LHCb::VLCluster>& hl,
                   LinkerWithKey<LHCb::MCParticle, LHCb::VLCluster>& pl);

  LHCb::MCVLDigits* m_digits; 
  LHCb::VLClusters* m_clusters;
  /// Pointer to detector element
  DeVL* m_det;
  /// Digits forming a cluster candidate 
  std::vector<LHCb::MCVLDigit*> m_candidate;
  /// Channels of current sensor already used for clusters.
  std::vector<bool> m_used;

  /// S/N threshold for a seed digit
  double m_seedSignalToNoiseCut;
  /// S/N threshold for adding digits to an existing cluster
  double m_lowSignalToNoiseCut;
  /// Maximum number of clusters to make per event
  unsigned int m_maxClusters; 
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
