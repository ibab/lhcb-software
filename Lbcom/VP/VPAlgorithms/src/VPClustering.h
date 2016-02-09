#ifndef VPCLUSTERING_H
#define VPCLUSTERING_H 1

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// LHCb
// Event/DigiEvent
#include "Event/VPDigit.h"

/** @class VPClustering VPClustering.h
 * 
 *  @author Daniel Hynds
 *  @date   2013-08-13
 */

class DeVP;

class VPClustering : public GaudiAlgorithm {
 public:
  /// Standard constructor
  VPClustering(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize() override;  ///< Algorithm initialization
  StatusCode execute() override;     ///< Algorithm execution

 private:

  /// Location of input container (VPDigits) 
  std::string m_digitLocation;
  /// Location of output container (VPClusters)
  std::string m_clusterLocation;

  /// Detector element
  DeVP* m_vpDet = nullptr;

  bool m_debug;

};

#endif  // VPCLUSTERING_H
