
#ifndef RichPixel_h
#define RichPixel_h 1

#include <vector>

#include "RichKernel/RichSmartID.h"
#include "RichBase.h"
#include "RichPixelProperties.h"
#include "RichPixelReadout.h"
#include "RichRegistry.h"

class RichPixel {

public:

  RichPixel(){};

  //Basic constructor, takes a base and an index
  RichPixel(const RichPixelProperties* p) : property(p){}

  RichPixel(const RichSmartID& sid)
  {
    const RichBase* MyBase = NULL;
    if ( sid.rich() == 0 ) MyBase = RichRegistry::GetBase();
    property = MyBase->DecodeUniqueID( sid.index());
  }

  RichPixel( const RichBase * b,
             const RichSmartID & sid )
  {
    property = b->DecodeUniqueID(sid.index());
    //unsigned int num = property->getNum();
  }

  const RichPixelReadout* Readout() const
  {
    return property->Readout();
  }

  unsigned int GetUniqueNum()
  {
    return property->getNum();
  }

private: // methods

  const RichSmartID* TheIndex() const { return property->Index(); }

  const RichPixelProperties* Property() const { return property; }

private: // data

  const RichPixelProperties* property;

  friend class RichBase;

};


#endif










