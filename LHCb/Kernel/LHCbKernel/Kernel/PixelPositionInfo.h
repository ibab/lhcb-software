#ifndef KERNEL_PIXELPOSITIONINFO_H
#define KERNEL_PIXELPOSITIONINFO_H 1

// Include files

namespace LHCb
{

  /** @struct PixelPositionInfo PixelPositionInfo.h Kernel/PixelPositionInfo.h
   *
   *  Adds 2D position information to a channel identifier
   *
   *  @author Victor Coco
   *  @date   2010-02-01
   */

  template <class ChannelType>
  struct PixelPositionInfo
  {
    ChannelType pixel;
    std::pair<double,double> fractionalPosition;
    std::pair<double,double> fractionalError;
    unsigned int clustToT;
  };
};

#endif // KERNEL_PIXELPOSITIONINFO_H
