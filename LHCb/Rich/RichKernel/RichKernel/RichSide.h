// $Id: RichSide.h,v 1.6 2003-04-16 10:48:37 jonrob Exp $
#ifndef RICHKERNEL_RICHSIDE_H
#define RICHKERNEL_RICHSIDE_H 1

// Include files
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/MsgStream.h"

/** @enumeration Side RichSide.h RichKernel/RichSide.h
 *
 *  Enumeration for Rich1 detector splits
 *
 *  @author Chris Jones
 *  @date   2002-06-19
 */

namespace Rich {

  enum Side {
    top,
    bottom,
    left = 0,
    right = 1
  };

  /// Text conversion for Rich::Side enumeration
  inline std::string text( const Rich::Side& side ) {
    switch( side ) {
    case Rich::top:    return "top or left"; // Note also covers  Rich::left
    case Rich::bottom: return "bottom or right"; // Note also covers  Rich::right
    default:           return "?"; // should never happen
    }
  }

  /// Std Vector typedef
  typedef std::vector<Side> RichSideVector;

}

/// Implement textual ostream << method for Rich::Side enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::Side& side ) {
  s << Rich::text( side );
  return s;
}

/// Implement textual MsgStream << method for Rich::Side enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::Side& side ) {
  s << Rich::text( side );
  return s;
}

#endif // RICHKERNEL_RICHSIDE_H
