// $Id: VPClusterMonitor.h, v1.0 2012-07-06 dhynds
#ifndef VPCLUSTERMONITOR_H 
#define VPCLUSTERMONITOR_H 1

// Include files

// Event
#include "Event/VPDigit.h"
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"
#include "Event/MCHit.h"

// GAUDI
#include "GaudiAlg/IHistoTool.h"
#include "GaudiAlg/GaudiHistoAlg.h"

// VPelDet
#include "VPDet/DeVP.h"

// Local
#include "VPRadiationDamageTool.h"


/** @class VPClusterMonitor VPClusterMonitor.h
 *  
 *
 *  @author Daniel Hynds
 *  @date   2012-07-06
 */
class VPClusterMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  VPClusterMonitor( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~VPClusterMonitor( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  
protected:
  
private:
  LHCb::VPClusters* m_clusters;
  LHCb::MCHits* m_hits;
  std::string m_clusterCont;
  std::string m_hitCont;
  
  StatusCode loopClusters();
  
  VPRadiationDamageTool* m_radDamageTool;
  DeVP* m_vpDet;
  
  double m_dataTaken;
  bool m_irradiated;
  
};

#endif // VPCLUSTERMONITOR_H

