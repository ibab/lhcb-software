
#include "RichReadout/Pixel.h"
#include "RichKernel/RichSmartID.h"
#include "RichReadout/RichRegistry.h"

Pixel::Pixel(const RichSmartID& sid) {

  const RichBase* MyBase;
  if ( sid.rich() == 0 ) MyBase = RichRegistry::GetBase();
  property=MyBase->DecodeUniqueID( sid.index());

}

Pixel::Pixel(const RichBase * b, const RichSmartID& sid) {

  property=b->DecodeUniqueID(sid.index());
  //unsigned int num = property->getNum();

}
