#ifndef SICBPixel_h
#define SICBPixel_h 1

//#include "RichGlobal/Definitions.h"
#include "RichDet/Rich.h"
#include "RichDet/Pixel.h"
#include "RichDet/CLHEP_Geometry.h"

class SICBPixel : public Pixel {

public:
  SICBPixel (const Rich::RichID rich, const HepPoint3D & position);
 
  virtual ~SICBPixel();

  // Arbitrary, but constant order of Pixels.
  bool operator < (const Pixel &right) const;

  bool operator == (const Pixel &right) const;

  bool operator != (const Pixel &right) const;

  Pixel * clone () const;

  virtual const HepPoint3D & position() const;

protected:

private:

  static const float EPSILON = 1e-5;
 
  SICBPixel();

  SICBPixel(const SICBPixel &right);

  SICBPixel & operator=(const SICBPixel &right);

  // Data Members for Class Attributes
 
  const HepPoint3D position_;
  
private:
};

#endif
