#ifndef RichPixelProperties_h
#define RichPixelProperties_h 1

#include "Kernel/RichSmartID.h"
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

  const RichSmartID::KeyType getNum() const
  {
    return index.key();
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
