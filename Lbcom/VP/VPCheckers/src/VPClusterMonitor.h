#ifndef VPCLUSTERMONITOR_H 
#define VPCLUSTERMONITOR_H 1

// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
// LHCb
// Event/DigiEvent
#include "Event/VPCluster.h"
// Det/VPDet
#include "VPDet/DeVP.h"


/** @class VPClusterMonitor VPClusterMonitor.h
 *  
 *
 *  @author Daniel Hynds
 *  @date   2012-07-06
 */

class VPClusterMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  VPClusterMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor  
  virtual ~VPClusterMonitor();
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  
private:
  std::string m_clusterLocation;
  std::string m_linkedHitsLocation;
  
  DeVP* m_det;
  
};

#endif

