
#include "RichReadout/RichBase.h"
#include "RichReadout/Pixel.h"
#include "RichReadout/PixelReadout.h"
#include "RichReadout/RichRegistry.h"

RichBase::RichBase( ):  myName("RICH1"), MyState(OPEN){}

RichBase::RichBase(RichBase&b): myName("RICH1"), MyState(b.MyState){}

RichBase::~RichBase() {}

RichBase& RichBase::operator=(const RichBase&b)
{
  MyState=b.MyState;
  return *this;
}

void RichBase::upDate( std::vector<unsigned int> pixelList )
{

  activePixels = new activemap();

  for ( std::vector<unsigned int>::iterator intIndex = pixelList.begin();
        intIndex != pixelList.end();
        ++intIndex ) {

    RichSmartID* pixel = new RichSmartID( (*intIndex) );
    PixelReadout* readout = new PixelReadout();
    PixelProperties* prop = new PixelProperties(pixel, this, readout);
    const unsigned int index = pixel->index();
    activePixels->insert( activemap::value_type(index,prop) );

  }

  RichRegistry::RegisterMe(this);

  CloseState();

}

long RichBase::size() const
{
  return activePixels->size();
}

void RichBase::clear(){
  activePixels->clear();
}

RichBase::activemap::const_iterator RichBase::begin() const
{
  return activePixels->begin();
}

RichBase::activemap::const_iterator RichBase::end() const
{
  return activePixels->end();
}

PixelProperties*  RichBase::DecodeUniqueID (const unsigned int& idx ) const
{

  activemap::const_iterator it = activePixels->find(idx);

  if(it!=activePixels->end()) {
    return (*it).second;
  } else {
    return 0;
  }

}
