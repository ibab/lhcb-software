
//-----------------------------------------------------------------------------
/** @file RichRecPixelParentType.h
 *
 * Header file for RichRecPixel enumerations
 *
 * CVS Log :-
 * $Id: RichRecPixelParentType.h,v 1.7 2004-11-20 12:30:24 jonrob Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2004/10/30 19:23:21  jonrob
 * Add enmeration type for RawBuffer parentage
 *
 * Revision 1.5  2004/07/26 18:00:58  jonrob
 * Various improvements to the doxygen comments
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

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich {

  /** @namespace Rich::PixelParent
   *
   *  Namespace for information on the pixel parent type
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace PixelParent {

    /** @enum Rich::PixelParent::Type
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

  /// Text conversion for Rich::RecTrack::Type enumeration
  std::string text( const Rich::PixelParent::Type& parent ) ;

}

/// Implement StreamBuffer >> method for Rich::PixelParent::Type enumeration
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   Rich::PixelParent::Type& pixel ) {
  int intType;
  s >> intType;
  pixel = static_cast<Rich::PixelParent::Type>(intType);
  return s;
}

/// Implement StreamBuffer << method for Rich::PixelParent::Type enumeration
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const Rich::PixelParent::Type& pixel ) {
  s << static_cast<int>(pixel);
  return s;
}

/// Implement textual ostream << method for Rich::RecTrack::Type enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::PixelParent::Type& pixel ) {
  s << Rich::text( pixel );
  return s;
}

/// Implement textual MsgStream << method for Rich::Rich2Side enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::PixelParent::Type& pixel ) {
  s << Rich::text( pixel );
  return s;
}

#endif // RICHRECBASE_RICHRECPIXELPARENTTYPE_H
