
#ifndef RichPixel_h
#define RichPixel_h 1

#include <vector>

#include "Kernel/RichSmartID.h"
#include "RichBase.h"
#include "RichPixelProperties.h"
#include "RichPixelReadout.h"
#include "RichRegistry.h"

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      /// Rich Pixel
      class RichPixel
      {

      public:

        RichPixel(){};

        //Basic constructor, takes a base and an index
        RichPixel(const RichPixelProperties* p) : property(p){}

        RichPixel( const LHCb::RichSmartID sid )
          : property ( 0 )
        {
          const RichBase* MyBase = NULL;
          if ( sid.rich() == 0 ) MyBase = RichRegistry::GetBase();
          property = MyBase->DecodeUniqueID( sid );
        }

        RichPixel( const RichBase * b,
                   const LHCb::RichSmartID sid )
          : property ( b->DecodeUniqueID( sid ) )
        { }

        const RichPixelReadout* Readout() const
        {
          return property->Readout();
        }

      private: // methods

        const RichPixelProperties* Property() const { return property; }

      private: // data

        const RichPixelProperties* property;

        friend class RichBase;

      };

    }
  }
}

#endif










