
#include <iostream>

#include "RichDet/Pixel.h"

Pixel::Pixel (const Rich::RichID rich)
  : m_rich(rich)
{
#ifdef MEM_DEBUG
  ++s_number;
#endif
}


Pixel::~Pixel()
{
#ifdef MEM_DEBUG
  --s_number;
#endif
}


// Additional Declarations
#ifdef MEM_DEBUG
Pixel::s_number = 0;

static void Pixel::memDebug() const
{
  cout << "MEM_DEBUG: Pixel = " << s_number << endl;
}
#endif


