#ifndef RichFrontEndDigitiser_h
#define RichFrontEndDigitiser_h 1

#include "RichTimeSample.h"

// This class could be removed ?

namespace Rich
{
  namespace MC
  {
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
