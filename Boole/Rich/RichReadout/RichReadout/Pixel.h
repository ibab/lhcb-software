
#ifndef Pixel_h
#define Pixel_h
#include <vector>

#include "RichKernel/RichSmartID.h"
#include "RichReadout/RichBase.h"
#include "RichReadout/PixelProperties.h"
#include "RichReadout/PixelReadout.h"

class RichBase;
class PixelReadout;

class Pixel
{
public:

  Pixel(){};

  //Basic constructor, takes a base and an index
  Pixel(const PixelProperties* p) : property(p){}

  Pixel(const RichSmartID&);

  Pixel(const RichBase * base, const RichSmartID&);

  //Iterator like

  Pixel operator++()
  {
    const RichBase* b = property->Base();
    *this = b->next(*this);
    return *this;
  }

  Pixel operator++(int)
  {
    Pixel tmp=*this;
    const RichBase* b =property->Base();
    *this = b->next(*this);
    return tmp;
  }

  bool operator<(const Pixel&c) const
  {
    return property<c.property ? true : false;
  }

  bool operator==(const Pixel&c) const
  {
    if(property==c.property) return true;
    if(property && c.property)
      {
        return *property==*c.property ? true : false;
      }
    else
      {
        return false;
      }
  }

  bool operator!=(const Pixel&c) const
  {
    if(property!=c.property) return true;
    if(property && c.property)
      {
        return *property!=*c.property ? true : false;
      }
    else
      {
        return false;
      }
  }

  const PixelReadout* Readout() const{
    return property->Readout();
  }

  unsigned int GetUniqueNum(){

    return property->getNum();

  }

  void         SetUniqueNum(unsigned int un) {
    property->setNum(un);
  }


  string Base() const  {
    return string(base,4);
  }


private:

  char base[4];

  const PixelProperties* property;

  const RichSmartID* TheIndex() const{return property->Index();}

  const PixelProperties* Property() const{return property;}

  friend class RichBase;

};


#endif










