// $Id: IVeloPixClusterPosition.h,v 1.1 2010-03-17 14:45:12 cocov Exp $
#ifndef KERNEL_IVELOPIXCLUSTERPOSITION_H 
#define KERNEL_IVELOPIXCLUSTERPOSITION_H 1

// Include files

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/PixelPositionInfo.h"

static const InterfaceID IID_IVeloPixClusterPosition ("IVeloPixClusterPosition", 1, 0);

/** @class IVeloPixClusterPosition IVeloPixClusterPosition.h Kernel/IVeloPixClusterPosition.h
 *  
 *
 *  @author Victor Coco
 *  @date   2010-02-02
 */

namespace LHCb
{
  class VeloPixLiteCluster;
  class VeloPixChannelID;
  class StateVector;
}

class IVeloPixClusterPosition : virtual public IAlgTool{
public:

  // a new tool interface, for VeloPix, can be used in other pixel detector
  // (defined in Kernel/PixelPositonInfo.h)
  typedef LHCb::PixelPositionInfo<LHCb::VeloPixChannelID> toolInfo;
  typedef std::pair<double, double> Direction;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() {return IID_IVeloPixClusterPosition;}

 /** calculate position of the VeloPixLiteCluster
  * @param aCluster pointer to a VeloPixLiteCLuster cluste
  * @return toolInfo (templated struct) 
  * <br> pixel = channel ID of a nearest pixel
  * <br> fractionalPosition = fractional position in the pixel (better precision)
  * within range (0-1,0-1)
  * <br> error = estimate of the error (in fraction of pixel)
  * The returned error estimate depends only on pixel size
  */
  virtual toolInfo position(const LHCb::VeloPixLiteCluster* aCluster) const=0;

 /** calculate position of the VeloPixLiteCluster
  * @param aCluster pointer to a VeloPixLiteCLuster cluster
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
  virtual toolInfo position(const LHCb::VeloPixLiteCluster* aCluster,
                            const Gaudi::XYZPoint& aPoint,
                            const Direction& aDirection) const=0;

 /** calculate position of the VeloPixLiteCluster
  * @param aCluster pointer to a VeloPixLiteCluster cluster
  * @param aState const reference to VectorState object
  * @return toolInfo (templated struct) 
  * <br> pixel = channel ID of a nearest pixel
  * <br> fractionalPosition = fractional position in the pixel (better precision)
  * within range (0-1,0-1)
  * <br> error = estimate of the error (in fraction of pixel)
  * The returned error estimate depends both on pixel size and 
  * projected angle of a track
  */

  virtual toolInfo position(const LHCb::VeloPixLiteCluster* aCluster,
                            const LHCb::StateVector& aState) const=0;

};
#endif // KERNEL_IVELOCLUSTERPOSITION_H
