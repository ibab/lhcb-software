#ifndef PixelProperties_h
#define PixelProperties_h 1

#include "RichKernel/RichSmartID.h"

class RichBase;
class Pixel;
class PixelReadout;

class PixelProperties {

public:

  PixelProperties( RichSmartID* i, const RichBase* b, const PixelReadout* r )
    : base(b), readout(r)
  {
    if (i) { index=i; } else { index = 0;}
    setNum( index->index() );
  }

  PixelProperties(PixelProperties&p) : base(p.base), readout(p.readout)
  {
    index=p.index;
  }

  PixelProperties& operator=(const PixelProperties&);

  const RichBase* Base() const { return base; }

  const PixelReadout* Readout() const
  {
    if(readout){ return readout;} else { return 0; }
  }

  void SetReadout(PixelReadout*r) {readout=r;}

  void  setNum (unsigned int num) const { UniqueNumber = num; }

  const unsigned int getNum() const { return UniqueNumber; }

  const RichSmartID* Index() const {return index;}


  bool operator==(const PixelProperties&p) const
  {
    if( readout==p.readout) {return true;}
    return false;
  }

  bool operator!=(const PixelProperties&p) const
  {
    if( readout==p.readout) { return true ;}
    return false;
  }

private:

  const RichBase* base;
  const PixelReadout* readout;
  const RichSmartID* index;
  mutable unsigned int UniqueNumber;

  friend class Pixel;

};

#endif
