// $Id: IVeloClusterPosition.h,v 1.10 2007-07-12 08:07:05 cattanem Exp $
#ifndef KERNEL_IVELOCLUSTERPOSITION_H 
#define KERNEL_IVELOCLUSTERPOSITION_H 1

// Include files

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/SiPositionInfo.h"

static const InterfaceID IID_IVeloClusterPosition ("IVeloClusterPosition", 1, 0);

/** @class IVeloClusterPosition IVeloClusterPosition.h Kernel/IVeloClusterPosition.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2005-09-30
 */

namespace LHCb
{
  class VeloCluster;
  class VeloChannelID;
  class StateVector;
}

class IVeloClusterPosition : virtual public IAlgTool{
public:

  // a new tool interface, common for Velo and ST 
  // (defined in Kernel/SiPositonInfo.h)
  typedef LHCb::SiPositionInfo<LHCb::VeloChannelID> toolInfo;
  typedef std::pair<double, double> Pair;
  
  // Return the interface ID
  static const InterfaceID& interfaceID() {return IID_IVeloClusterPosition;}
  
 /** calculate position of the VeloCluster
  * @param aCluster pointer to a VeloCLuster cluster
  * @return toolInfo (templated struct) 
  * <br> strip = channel ID of a floored nearest strip
  * <br> fractionalPosition = interstrip position (in fraction of strip)
  * within range (0-1)
  * <br> error = estimate of the error (in fraction of strip)
  * The returned error estimate depends only on pitch
  */
  virtual toolInfo position(const LHCb::VeloCluster* aCluster) const=0;
 
 /** calculate position of the VeloCluster
  * @param aCluster pointer to a VeloCLuster cluster
  * @param aPoint const reference to XYZPoint - XYZ position of track  
  * in the sensor in global reference frame. 
  * @param aDirection const reference to pair of doubles - directions of 
  *        track xz slope and yz slope
  * @return toolInfo (templated struct) 
  * <br> strip = channel ID of a floored nearest strip
  * <br> fractionalPosition = interstrip position (in fraction of strip)
  * <br> error = estimate of the error (in fraction of strip)
  * The returned error estimate depends both on pitch and 
  * projected angle of a track
  */
  virtual toolInfo position(const LHCb::VeloCluster* aCluster,
                            const Gaudi::XYZPoint& aPoint,
                            const Pair& aDirection) const=0;

 /** calculate position of the VeloCluster
  * @param aCluster pointer to a VeloCLuster cluster
  * @param aState const reference to VectorState object
  * @return toolInfo (templated struct) 
  * <br> strip = channel ID of a floored nearest strip
  * <br> fractionalPosition = interstrip position (in fraction of strip)
  * <br> error = estimate of the error (in fraction of strip)
  * The returned error estimate depends both on pitch and 
  * projected angle of a track
  */

  virtual toolInfo position(const LHCb::VeloCluster* aCluster,
                            const LHCb::StateVector& aState) const=0;

/** calculate fractional position
  * @param aCluster pointer to a VeloCLuster cluster
  * @return Pair - pair of doubles 
  * The method is very useful for resolution studies
  */
  virtual Pair fracPosLA(const LHCb::VeloCluster* aCluster) const=0;

  /** return projected angle
  * @return double - value of projected angle in degree 
  * Especially designed for resolution study for both MC and real data
  */
  virtual double projectedAngle() const=0;
};
#endif // KERNEL_IVELOCLUSTERPOSITION_H
