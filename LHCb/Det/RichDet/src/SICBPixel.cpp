#include <iostream>
#include <cmath>
#include "RichDet/SICBPixel.h"

SICBPixel::SICBPixel (const Rich::RichID rich, const HepPoint3D & position)
  : Pixel(rich), position_(position)
{
}

SICBPixel::~SICBPixel()
{
}

/*
SICBPixel::SICBPixel(const SICBPixel &right)
{
}


SICBPixel::SICBPixel & operator=(const SICBPixel &right)
{
}
*/

bool SICBPixel::operator < (const Pixel &right) const
{
  const SICBPixel & r = dynamic_cast<const SICBPixel&>(right);
  if ( rich() != r.rich() ) return rich() < r.rich();
  if ( abs(position_.x() - r.position_.x()) > EPSILON )
    return position_.x() < r.position_.x();
  if ( abs(position_.y() - r.position_.y()) > EPSILON )
    return position_.y() < r.position_.y();
  return position_.z() < r.position_.z();

}

bool SICBPixel::operator == (const Pixel &right) const
{
  const SICBPixel & r = dynamic_cast<const SICBPixel&>(right);
  return rich() == r.rich() &&
    position_.distance2(r.position_) < EPSILON * EPSILON;

}

bool SICBPixel::operator != (const Pixel &right) const
{
  const SICBPixel & r = dynamic_cast<const SICBPixel&>(right);
  return rich() != r.rich() ||
    position_.distance2(r.position_) > EPSILON * EPSILON;
}

SICBPixel* SICBPixel::clone () const
{
  return new SICBPixel(rich(),position_);
}

const HepPoint3D & SICBPixel::position () const
{
  return position_;
}


