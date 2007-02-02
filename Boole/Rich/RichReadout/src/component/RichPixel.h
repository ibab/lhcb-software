
#ifndef RichPixel_h
#define RichPixel_h 1

#include <vector>

#include "Kernel/RichSmartID.h"
#include "RichBase.h"
#include "RichPixelProperties.h"
#include "RichPixelReadout.h"
#include "RichRegistry.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace MC
   *
   *  General namespace for RICH MC related software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   05/12/2006
   */
  //-----------------------------------------------------------------------------
  namespace MC
  {

    //-----------------------------------------------------------------------------
    /** @namespace Digi
     *
     *  General namespace for RICH Digitisation simuation related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   17/01/2007
     */
    //-----------------------------------------------------------------------------
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










