// $Id: IVeloClusterPosition.h,v 1.2 2009-01-19 10:14:50 dhcroft Exp $
#ifndef KERNEL_IVELOCLUSTERPOSITION_H 
#define KERNEL_IVELOCLUSTERPOSITION_H 1

// Include files

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/SiPositionInfo.h"


/** @class IVeloClusterPosition IVeloClusterPosition.h Kernel/IVeloClusterPosition.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2005-09-30
 */

namespace LHCb
{
  class VeloCluster;
  class VeloLiteCluster;
  class VeloChannelID;
  class StateVector;
}

class IVeloClusterPosition : public extend_interfaces<IAlgTool>{
public:
   DeclareInterfaceID(IVeloClusterPosition, 2, 0);

  // a new tool interface, common for Velo and ST 
  // (defined in Kernel/SiPositonInfo.h)
  typedef LHCb::SiPositionInfo<LHCb::VeloChannelID> toolInfo;
  typedef std::pair<double, double> Direction;
  
  
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

 /** calculate position of the VeloLiteCluster
  * @param aCluster pointer to a VeloLiteCLuster cluster
  * @return toolInfo (templated struct) 
  * <br> strip = channel ID of a floored nearest strip
  * <br> fractionalPosition = interstrip position (in fraction of strip)
  * within range (0-1)
  * <br> error = estimate of the error (in fraction of strip)
  * The returned error estimate depends only on pitch
  */
  virtual toolInfo position(const LHCb::VeloLiteCluster* aCluster) const=0;
 
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
                            const Direction& aDirection) const=0;

 /** calculate position of the VeloLiteCluster
  * @param aCluster pointer to a VeloLiteCLuster cluster
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
  virtual toolInfo position(const LHCb::VeloLiteCluster* aCluster,
                            const Gaudi::XYZPoint& aPoint,
                            const Direction& aDirection) const=0;

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

 /** calculate position of the VeloLiteCluster
  * @param aCluster pointer to a VeloLiteCluster cluster
  * @param aState const reference to VectorState object
  * @return toolInfo (templated struct) 
  * <br> strip = channel ID of a floored nearest strip
  * <br> fractionalPosition = interstrip position (in fraction of strip)
  * <br> error = estimate of the error (in fraction of strip)
  * The returned error estimate depends both on pitch and 
  * projected angle of a track
  */

  virtual toolInfo position(const LHCb::VeloLiteCluster* aCluster,
                            const LHCb::StateVector& aState) const=0;

};
#endif // KERNEL_IVELOCLUSTERPOSITION_H
