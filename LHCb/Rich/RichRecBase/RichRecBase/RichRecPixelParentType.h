// $Id: RichRecPixelParentType.h,v 1.1.1.1 2003-04-01 13:21:56 jonesc Exp $
#ifndef RICHRECKERNEL_RICHRECPIXELPARENTTYPE_H 
#define RICHRECKERNEL_RICHRECPIXELPARENTTYPE_H 1

// Include files
#include <string>
#include <iostream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

/** @class RichRecPixelParentType RichRecPixelParentType.h RichRecKernel/RichRecPixelParentType.h
 *  
 *  Enumeration for Rich reconstruction working pixel object
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-12
 */

namespace Rich {

  /// Enumeration for RecPixel parent Type
  namespace RecPixel {
    enum ParentType {
      Unknown = -1,
      Digit,
      MCHit
    };
  }

  /// Text conversion for Rich::RecTrack::ParentType enumeration
  std::string text( const Rich::RecPixel::ParentType& parent ) ;

}

/// Implement StreamBuffer >> method for Rich::RecPixel::ParentType enumeration
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   Rich::RecPixel::ParentType& pixel ) {
  int intType;
  s >> intType;
  pixel = (Rich::RecPixel::ParentType)intType;
  return s;
}

/// Implement StreamBuffer << method for Rich::RecPixel::ParentType enumeration
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const Rich::RecPixel::ParentType& pixel ) {
  s << (int)pixel;
  return s;
}

/// Implement textual ostream << method for Rich::RecTrack::ParentType enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::RecPixel::ParentType& pixel ) {
  s << Rich::text( pixel );
  return s;
}

/// Implement textual MsgStream << method for Rich::Rich2Side enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::RecPixel::ParentType& pixel ) {
  s << Rich::text( pixel );
  return s;
}

#endif // RICHRECKERNEL_RICHRECPIXELPARENTTYPE_H
