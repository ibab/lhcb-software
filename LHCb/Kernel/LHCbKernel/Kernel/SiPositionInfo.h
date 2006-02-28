// $Id: SiPositionInfo.h,v 1.1 2006-02-28 14:01:15 szumlat Exp $
#ifndef KERNEL_SIPOSITIONINFO_H 
#define KERNEL_SIPOSITIONINFO_H 1

// Include files

/** @class SiPositionInfo SiPositionInfo.h Kernel/SiPositionInfo.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-02-27
 */

namespace LHCb
{
  template <class ChannelType>
  struct SiPositionInfo
  {
    ChannelType strip;
    double fractionalPosition;
    double fractionalError;
  };
};

#endif // KERNEL_SIPOSITIONINFO_H
