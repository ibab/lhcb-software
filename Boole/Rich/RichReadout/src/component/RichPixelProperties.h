#ifndef RichPixelProperties_h
#define RichPixelProperties_h 1

//#include "Kernel/RichSmartID.h"
#include "RichPixelReadout.h"

class RichPixel;

class RichPixelProperties {

public:

  RichPixelProperties( /*const RichSmartID i*/ )
    : readout ( new RichPixelReadout() ) 
  { }

  ~RichPixelProperties()
  {
    cleanUp();
  }

  inline const RichPixelReadout* Readout() const
  {
    return readout;
  }

  inline void cleanUp()
  {
    if (readout) { delete readout; readout = 0; }
  }

private:

  RichPixelReadout * readout;

  friend class RichPixel;

};

#endif
