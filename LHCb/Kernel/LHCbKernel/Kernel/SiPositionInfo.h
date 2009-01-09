// $Id: SiPositionInfo.h,v 1.3 2009-01-09 16:12:48 jvantilb Exp $
#ifndef KERNEL_SIPOSITIONINFO_H
#define KERNEL_SIPOSITIONINFO_H 1

// Include files

namespace LHCb
{

  /** @struct SiPositionInfo SiPositionInfo.h Kernel/SiPositionInfo.h
   *
   *  Adds position information to a channel identifier
   *
   *  @author Tomasz Szumlak
   *  @date   2006-02-27
   */

  template <class ChannelType>
  struct SiPositionInfo
  {
    ChannelType strip;
    double fractionalPosition;
    double fractionalError;
    unsigned int clusterSize;
  };
};

#endif // KERNEL_SIPOSITIONINFO_H
