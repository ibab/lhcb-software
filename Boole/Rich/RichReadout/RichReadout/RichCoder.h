#ifndef RichCoder_h
#define RichCoder_h

#include "RichReadout/RichHpdData.h"
#include "RichReadout/RichTimeSample.h"
#include "RichReadout/PixelReadout.h"


// will convert digits to online format when defined

class RichCoder
{
	
public:

  RichCoder();

  ~RichCoder(){};

  RichHpdData convert (RichTimeSample) const;
  
};

#endif
  
