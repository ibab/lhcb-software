
//-----------------------------------------------------------------------------
/** @file RichRecPixelParentType.h
 *
 * Header file for RichRecPixel enumerations
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2002-07-12
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECPIXELPARENTTYPE_H
#define RICHRECBASE_RICHRECPIXELPARENTTYPE_H 1

// Include files
#include <string>
#include <iostream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

namespace Rich
{
  namespace Rec
  {

    /** @namespace Rich::Rec::PixelParent
     *
     *  Namespace for information on the pixel parent type
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   08/07/2004
     */
    namespace PixelParent
    {

      /** @enum Type
       *
       *  Enumeration for the various different pixel parent types
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   2002-07-12
       */
      enum Type
        {
          Unknown = -1, ///< Parent type is unknown
          Digit,        ///< Pixel derives from a reconstructed RichDigit
          MCHit,        ///< Pixel derives from Monte Carlo MCRichHit information
          RawBuffer,    ///< Pixel was derived directly from the RawBuffer
          NoParent      ///< Pixel has no associated parent (e.g. artificially added background)
        };

    }

    /// Text conversion for Rich::Rec::PixelParent::Type enumeration
    std::string text( const Rich::Rec::PixelParent::Type parent ) ;

  }

  /// Backwards compatibility
  inline std::string text( const Rich::Rec::PixelParent::Type parent ) { return Rich::Rec::text(parent); }

}

/// Implement textual ostream << method for Rich::RecTrack::Type enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::Rec::PixelParent::Type& pixel )
{
  return s << Rich::Rec::text( pixel );
}

#endif // RICHRECBASE_RICHRECPIXELPARENTTYPE_H
