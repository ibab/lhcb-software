
#include "RichReadout/RichCoder.h"
#include "RichReadout/RichTimeSample.h"
#include <vector>
#include <iostream>

RichCoder::RichCoder() { };

RichHpdData RichCoder::convert (RichTimeSample) const
{
  return RichHpdData();
}
