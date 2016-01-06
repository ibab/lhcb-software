#ifndef KERNEL_IVPCLUSTERPOSITION_H
#define KERNEL_IVPCLUSTERPOSITION_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Kernel/LHCbKernel
#include "Kernel/PixelPositionInfo.h"


/** @class IVPClusterPosition IVPClusterPosition.h
 *TrackInterfaces/IVPClusterPosition.h
 *
 *
 *  @author Victor Coco
 *  @date   2010-02-02
 */

namespace LHCb {
class VPCluster;
}

class IVPClusterPosition : public extend_interfaces<IAlgTool> {
 public:
    DeclareInterfaceID(IVPClusterPosition, 2, 0);

  /** Calculate position of a given VPCluster
   * @param cluster pointer to a VPCluster
   * @return struct containing coordinates and errors
   * The returned error estimate depends only on the pixel size.
   */
  virtual LHCb::VPPositionInfo position(const LHCb::VPCluster* cluster)
      const = 0;

  /** Calculate position of a given VPCluster
   * @param cluster pointer to a VPCluster
   * @param point const reference to XYZ coordinates of track intercept
   * @param tx track xz slope
   * @param ty track yz slope
   * @return struct containing coordinates and errors
   * The returned error estimate depends both on the pixel size and
   * the projected angle of a track.
   */
  virtual LHCb::VPPositionInfo position(const LHCb::VPCluster* cluster,
                                        const Gaudi::XYZPoint& point,
                                        const double& tx,
                                        const double& ty) const = 0;
};
#endif  // KERNEL_IVPCLUSTERPOSITION_H
