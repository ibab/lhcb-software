#ifndef VELOLITE_CLUSTER_MONITOR_H 
#define VELOLITE_CLUSTER_MONITOR_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// Event/DigiEvent
#include "Event/VeloStripCluster.h"

/** @class VeloLiteClusterMonitor VeloLiteClusterMonitor.h
 *  
 */

class VeloLiteClusterMonitor : public GaudiTupleAlg {

public: 
  /// Standard constructor
  VeloLiteClusterMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VeloLiteClusterMonitor();

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalization

private:

  StatusCode monitor();

  bool m_printInfo;
  LHCb::VeloStripClusters* m_clusters;
  double m_nClusters;
  double m_nClusters2;
  double m_nOneStrip;
  double m_nTwoStrip;
  double m_nThreeStrip;
  double m_nFourStrip;
  int m_nEvents;

};
#endif
