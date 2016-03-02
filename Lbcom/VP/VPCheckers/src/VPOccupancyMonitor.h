#ifndef VPOCCUPANCYMONITOR_H 
#define VPOCCUPANCYMONITOR_H 1

// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
// LHCb
// Det/VPDet
#include "VPDet/DeVP.h"

/** @class VPOccupancyMonitor VPOccupancyMonitor.h
 *  
 *
 */

class VPOccupancyMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  VPOccupancyMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor  
  virtual ~VPOccupancyMonitor();
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  
private:
  std::string m_clusterLocation;
  unsigned int m_station;
  DeVP* m_det = nullptr;
  
};

#endif
