// $Id: Pixel.h,v 1.2 2001-10-26 13:02:12 rihill Exp $
#ifndef RICHDET_PIXEL_H
#define RICHDET_PIXEL_H 1

//#include "RichGlobal/Definitions.h"
#include "RichDet/Rich.h"

// This represents a ID for a pixel, that naturally depends
// on the used Photondetectors. So a specific detector has
// its own PixelID type.
//
// It has to sortable, so it has to answer to operator <.
// The order can be arbirary, but naturally has to be
// constant. This allows to use Pixel as a Key in a set
// (e.g. set<Pixel,RecPixel>).

class Pixel
{

public:
  Pixel (const Rich::RichID rich);

  virtual ~Pixel();

  Rich::RichID rich () const;

  virtual Pixel * clone () const = 0;

  // Arbirary, but constant order of Pixels.
  virtual bool operator < (const Pixel &right) const = 0;

  virtual bool operator == (const Pixel &right) const = 0;

  virtual bool operator != (const Pixel &right) const = 0;

  // Additional Public Declarations
#ifdef MEM_DEBUG
  static void memDebug() const;
#endif

private:
  Pixel();

  Pixel(const Pixel &right);

  Pixel & operator=(const Pixel &right);

  // Data Members for Class Attributes

  Rich::RichID m_rich;

  // Additional Private Declarations
#ifdef MEM_DEBUG
  static int s_number;
#endif

private:
  // Additional Implementation Declarations

};

inline Rich::RichID Pixel::rich () const {
  return m_rich;
}

#endif // RICHDET_PIXEL_H
