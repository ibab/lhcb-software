#ifndef KERNEL_IVPCLUSTERPOSITION_H 
#define KERNEL_IVPCLUSTERPOSITION_H 1

// Include files

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/PixelPositionInfo.h"

static const InterfaceID IID_IVPClusterPosition ("IVPClusterPosition", 1, 0);

/** @class IVPClusterPosition IVPClusterPosition.h Kernel/IVPClusterPosition.h
 *  
 *
 *  @author Victor Coco
 *  @date   2010-02-02
 */

namespace LHCb {
  class VPCluster;
  class VPLiteCluster;
  class VPChannelID;
  class StateVector;
}

class IVPClusterPosition : virtual public IAlgTool{
public:

  // a new tool interface, for VP, can be used in other pixel detector
  // (defined in Kernel/PixelPositonInfo.h)
  typedef LHCb::PixelPositionInfo<LHCb::VPChannelID> toolInfo;
  typedef std::pair<double, double> Direction;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() {return IID_IVPClusterPosition;}

 /** calculate position of the VPLiteCluster
  * @param aCluster pointer to a VPLiteCLuster
  * @return toolInfo (templated struct) 
  * <br> pixel = channel ID of a nearest pixel
  * <br> fractionalPosition = fractional position in the pixel (better precision)
  * within range (0-1,0-1)
  * <br> error = estimate of the error (in fraction of pixel)
  * The returned error estimate depends only on pixel size
  */
  virtual toolInfo position(const LHCb::VPLiteCluster* aCluster) const=0;

 /** Calculate position of a given VPCluster
  * @param cluster pointer to a VPCluster
  * @return toolInfo (templated struct) 
  * <br> pixel = channel ID of the cluster
  * <br> fractionalPosition = inter-pixel fraction (within range 0-1)
  * <br> error = estimate of the error (in fraction of pixel pitch)
  * The returned error estimate depends only on pixel size
  */
  virtual toolInfo position(const LHCb::VPCluster* cluster) const = 0;

 /** calculate position of the VPLiteCluster
  * @param aCluster pointer to a VPLiteCLuster cluster
  * @param aPoint const reference to XYZPoint - XYZ position of track  
  * in the sensor in global reference frame. 
  * @param aDirection const reference to pair of doubles - directions of 
  *        track xz slope and yz slope
  * @return toolInfo (templated struct) 
  * <br> pixel = channel ID of a nearest pixel
  * <br> fractionalPosition = fractional position in the pixel (better precision)
  * within range (0-1,0-1)
  * <br> error = estimate of the error (in fraction of pixel)
  * The returned error estimate depends both on pixel size and 
  * projected angle of a track
  */
  virtual toolInfo position(const LHCb::VPLiteCluster* aCluster,
                            const Gaudi::XYZPoint& aPoint,
                            const Direction& aDirection) const=0;

 /** Calculate position of a given VPCluster
  * @param cluster pointer to a VPCluster
  * @param point const reference to XYZ coordinates of track intercept 
  * @param direction const reference to pair of doubles (track xz and yz slope)
  * @return toolInfo (templated struct) 
  * <br> pixel = channel ID of the cluster
  * <br> fractionalPosition = inter-pixel fraction (within range 0-1)
  * <br> error = estimate of the error (in fraction of pixel pitch)
  * The returned error estimate depends both on pixel size and 
  * projected angle of a track
  */
  virtual toolInfo position(const LHCb::VPCluster* cluster,
                            const Gaudi::XYZPoint& point,
                            const Direction& direction) const = 0;

 /** calculate position of the VPLiteCluster
  * @param aCluster pointer to a VPLiteCluster cluster
  * @param aState const reference to VectorState object
  * @return toolInfo (templated struct) 
  * <br> pixel = channel ID of a nearest pixel
  * <br> fractionalPosition = fractional position in the pixel (better precision)
  * within range (0-1,0-1)
  * <br> error = estimate of the error (in fraction of pixel)
  * The returned error estimate depends both on pixel size and 
  * projected angle of a track
  */

  virtual toolInfo position(const LHCb::VPLiteCluster* aCluster,
                            const LHCb::StateVector& aState) const=0;

};
#endif // KERNEL_IVPCLUSTERPOSITION_H 
