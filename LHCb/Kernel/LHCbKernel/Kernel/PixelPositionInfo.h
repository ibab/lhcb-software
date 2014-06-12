#ifndef KERNEL_PIXELPOSITIONINFO_H
#define KERNEL_PIXELPOSITIONINFO_H 1

#include "Kernel/VPChannelID.h"

namespace LHCb {

/** @struct PixelPositionInfo PixelPositionInfo.h Kernel/PixelPositionInfo.h
 *
 *  Adds 2D position information to a channel identifier
 *
 *  @author Victor Coco
 *  @date   2010-02-01
 */

template <class ChannelType>
struct PixelPositionInfo {
  double x;
  double y;
  double dx;
  double dy;
};

typedef PixelPositionInfo<VPChannelID> VPPositionInfo; 

}

#endif  // KERNEL_PIXELPOSITIONINFO_H
