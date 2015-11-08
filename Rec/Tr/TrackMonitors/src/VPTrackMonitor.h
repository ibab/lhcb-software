#ifndef VPTRACKMONI_H
#define VPTRACKMONI_H 1

// Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// LHCb
#include "VPDet/DeVPSensor.h"
#include "VPDet/DeVP.h"
#include "Event/Track.h"
#include "Event/State.h"
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

  virtual ~VPTrackMonitor();  ///< Destructor

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

 private:
  // TES locations
  std::string m_tracklocation;
  DeVP* m_veloDet_VP;

  Gaudi::XYZPoint getResidual(Gaudi::XYZPoint, const DeVPSensor*,
                              const LHCb::FitNode&);
};
#endif  // VPTRACKMONI_H
