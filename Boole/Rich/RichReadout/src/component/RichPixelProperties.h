#ifndef RichPixelProperties_h
#define RichPixelProperties_h 1

#include "RichPixelReadout.h"

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

      class RichPixel;

     
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
