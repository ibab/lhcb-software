#ifndef RichFrontEndDigitiser_h
#define RichFrontEndDigitiser_h 1

#include "RichTimeSample.h"

// The class could be removed ?


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

      /// Utility class represeneting the frontend digitiser
      class RichFrontEndDigitiser {

      public:

        RichFrontEndDigitiser() { }

        ~RichFrontEndDigitiser() { }

        inline bool process ( const RichTimeSample & sample,
                              const double new_threshold ) const
        {
          for ( unsigned int bin = 0; bin < sample.size(); ++bin ) {
            if ( sample[bin] >= new_threshold ) { return true; }
          }
          return false;
        }

      };

    }
  }
}

#endif
