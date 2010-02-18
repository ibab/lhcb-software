#ifndef RichPixelProperties_h
#define RichPixelProperties_h 1

#include "RichPixelReadout.h"

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      class RichPixel;

      /// General pixel properties
      class RichPixelProperties
      {

      public:

        RichPixelProperties( /*const RichSmartID i*/ )
          : readout ( new RichPixelReadout() )
        { }

        ~RichPixelProperties()
        {
          cleanUp();
        }

        inline const RichPixelReadout* Readout() const
        {
          return readout;
        }

        inline void cleanUp()
        {
          if (readout) { delete readout; readout = 0; }
        }

      private:

        RichPixelReadout * readout;

        friend class RichPixel;

      };

    }
  }
}

#endif
