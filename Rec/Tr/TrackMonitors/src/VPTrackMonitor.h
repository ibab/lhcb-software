#pragma once

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/Vector3DTypes.h"

// LHCb
// Det/VPDet
#include "VPDet/DeVPSensor.h"
#include "VPDet/DeVP.h"

// Rec
// Tr/TrackFitEvent
#include "Event/FitNode.h"

/** @class VPTrackMonitor VPTrackMonitor.h
 *
 *
 *  @author Christoph Hombach
 *  @date   2015-01-08
 */
class VPTrackMonitor : public GaudiTupleAlg {
 public:
  /// Standard constructor
  VPTrackMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VPTrackMonitor();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  // TES location of tracks
  std::string m_trackLocation;

  /// Detector element
  DeVP* m_det;

  Gaudi::XYZVector getResidual(const Gaudi::XYZPoint& cluster, 
                               const DeVPSensor* sensor,
                               const LHCb::FitNode& fitNode) const;
};
