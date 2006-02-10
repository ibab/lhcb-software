#ifndef _STBitsPolicy_H
#define _STBitsPolicy_H 1

class STBitsPolicy{

public:

  enum bits {positionBits = 0, channelBits = 2, 
              sizeBits = 14, thresBits = 15, interStripPrecision = channelBits};
  
  enum mask {positionMask = 0x0003, channelMask = 0x3ffc, 
              sizeMask = 0x4000, thresMask = 0x8000 };

};

#endif
