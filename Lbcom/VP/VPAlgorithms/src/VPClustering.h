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
  /// Destructor
  virtual ~VPClustering();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:

  std::string m_digitLocation;
  std::string m_clusterLocation;

  DeVP* m_vpDet;

  bool m_debug;

  bool isEdge(LHCb::VPDigit* digit) const;

};

#endif  // VPCLUSTERING_H
