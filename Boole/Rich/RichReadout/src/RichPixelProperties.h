#ifndef RichPixelProperties_h
#define RichPixelProperties_h 1

#include "RichKernel/RichSmartID.h"
#include "RichPixelReadout.h"

class RichPixel;

class RichPixelProperties {

public:

  RichPixelProperties( RichSmartID i )
    : index(i)
  {
    readout = new RichPixelReadout();
  }

  RichPixelProperties( RichPixelProperties & p )
  {
    index = p.index;
    readout = new RichPixelReadout();
  }

  ~RichPixelProperties()
  {
    delete readout;
  }

  const RichPixelReadout* Readout() const
  {
    return readout;
  }

  void SetReadout( RichPixelReadout * r )
  {
    readout = r;
  }

  const unsigned int getNum() const
  {
    return index.index();
  }

  const RichSmartID * Index() const
  {
    return &index;
  }

private:

  RichPixelReadout * readout;
  RichSmartID index;

  friend class RichPixel;

};

#endif
