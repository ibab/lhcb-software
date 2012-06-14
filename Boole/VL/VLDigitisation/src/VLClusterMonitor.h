#ifndef VL_CLUSTER_MONITOR_H 
#define VL_CLUSTER_MONITOR_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// Event/DigiEvent
#include "Event/VLCluster.h"

/** @class VLClusterMonitor VLClusterMonitor.h
 *  
 */

class VLClusterMonitor : public GaudiTupleAlg {

public: 
  /// Standard constructor
  VLClusterMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VLClusterMonitor() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  void monitor();

  bool m_detailed;
  bool m_printInfo;
  LHCb::VLClusters* m_clusters;
  double m_nClusters;
  double m_nClusters2;
  double m_nOneStrip;
  double m_nTwoStrip;
  double m_nThreeStrip;
  double m_nFourStrip;
  int m_nEvents;

};
#endif
