// $Id: VeloBitsPolicy.h,v 1.3 2006-02-23 15:41:50 krinnert Exp $
#ifndef VELOBITSPOLICY_H 
#define VELOBITSPOLICY_H 1

#include "SiDAQ/SiADCBankTraits.h"

/** Velo raw bank bit pattern
 *
 *  This class is a type parameter for the SiClusterWord
 *  template class that allows to share bit-packing code
 *  with the ST software.
 *  The bit pattern is an implementation of the Velo raw
 *  bank specification in EDMS 637676, Version 2.0 as of
 *  December 1, 2005.
 *  
 * @see SiClusterWord
 *
 * @author Kurt Rinnert
 * @date   2006-02-02
 */
class VeloBitsPolicy
{
public:

  typedef SiDAQ::adc_only_bank_tag adc_bank_type;

  enum bits {
    positionBits        =  0,
    channelBits         =  3,
    sizeBits            = 14,
    thresBits           = 15,
    interStripPrecision =  3 
  };

  enum mask {
    positionMask = 0x0007,
    channelMask  = 0x3ff8,
    sizeMask     = 0x4000,
    thresMask    = 0x8000
  };
};

#endif
