#ifndef RAWCLUSTERDECODING_H
#define RAWCLUSTERDECODING_H 1

#include "SiDAQ/SiRawBankDecoder.h"
#include "SiDAQ/SiRawBufferWord.h"
#include "SiDAQ/SiClusterWord.h"
#include "SiDAQ/SiADCBankTraits.h"

/* 
 * Definitions related to decoding of raw data into clusters.
 * Used in VeloClusterMonitor, in the high-multiplicity plot
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
typedef SiClusterWord<VeloBitsPolicy> VeloClusterWord;
typedef SiRawBankDecoder<VeloClusterWord> VeloRawBankDecoder;

static const int BANK_VERSION_v3 = 3;

#endif /// RAWCLUSTERDECODING_H

