#ifndef VELOLITEBITSPOLICY_H 
#define VELOLITEBITSPOLICY_H 1

#include "SiDAQ/SiADCBankTraits.h"

/** VeloLite raw bank bit pattern
 *
 *  This class is a type parameter for the SiClusterWord
 *  template class that allows to share bit-packing code
 *  with the ST software.
 *  
 * @see SiClusterWord
 *
 */
class VeloLiteBitsPolicy {

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
