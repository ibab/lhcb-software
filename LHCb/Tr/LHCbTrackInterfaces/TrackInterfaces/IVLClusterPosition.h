#ifndef KERNEL_IVLCLUSTERPOSITION_H 
#define KERNEL_IVLCLUSTERPOSITION_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/SiPositionInfo.h"

static const InterfaceID IID_IVLClusterPosition ("IVLClusterPosition", 1, 0);

/** @class IVLClusterPosition IVLClusterPosition.h Kernel/IVLClusterPosition.h
 * 
 * Interface for VLClusterPosition tool (based on IVeloClusterPosition) 
 *
 */

namespace LHCb {
  class VLCluster;
  class VLLiteCluster;
  class VLChannelID;
  class StateVector;
}

class IVLClusterPosition : virtual public IAlgTool {

public:

  typedef LHCb::SiPositionInfo<LHCb::VLChannelID> toolInfo;
  
  /// Return the interface ID
  static const InterfaceID& interfaceID() {return IID_IVLClusterPosition;}
  
  /// Calculate position of a given cluster.
  /// Error estimate depends only on strip pitch.
  virtual toolInfo position(const LHCb::VLCluster* cluster) const = 0;
  virtual toolInfo position(const LHCb::VLLiteCluster* cluster) const = 0;
 /// Calculate position of a given cluster.
 /// Error estimate depends on pitch and projected track angle.
  virtual toolInfo position(const LHCb::VLCluster* cluster,
                            const Gaudi::XYZPoint& point,
                            const double tx, const double ty) const = 0;
  virtual toolInfo position(const LHCb::VLLiteCluster* cluster,
                            const Gaudi::XYZPoint& point,
                            const double tx, const double ty) const = 0;
  /// Calculate position of a given cluster.
  virtual toolInfo position(const LHCb::VLCluster* cluster,
                            const LHCb::StateVector& state) const = 0;
  virtual toolInfo position(const LHCb::VLLiteCluster* cluster,
                            const LHCb::StateVector& state) const = 0;

};
#endif 
