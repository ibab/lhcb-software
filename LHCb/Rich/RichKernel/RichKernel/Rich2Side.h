// $Id: Rich2Side.h,v 1.9 2003-11-03 10:19:11 cattanem Exp $
#ifndef RICHKERNEL_RICH2SIDE_H
#define RICHKERNEL_RICH2SIDE_H 1

// Include files
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/MsgStream.h"

// namespace for RICH detector specifics
namespace Rich {

  /** @enum Rich::Rich2Side 
   *  Enumeration for RICH2 detector splits
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2002-06-19
   */
  enum Rich2Side {
    left = 0,
    right
  };

  /// Text conversion for Rich2Side enumeration
  inline std::string text( const Rich::Rich2Side& side ) {
    switch( side ) {
    case Rich::left:    return "left";
    case Rich::right:   return "right";
    default:            return "?"; // should never happen
    }
  }

  /// Std Vector typedef
  typedef std::vector<Rich2Side> Rich2SideVector;

}

/// Implement textual ostream << method for Rich::Rich2Side enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::Rich2Side& side ) {
  s << Rich::text( side );
  return s;
}

/// Implement textual MsgStream << method for Rich::Rich2Side enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::Rich2Side& side ) {
  s << Rich::text( side );
  return s;
}

// Text conversion for Rich2Side enumeration



#endif // RICHKERNEL_RICH2SIDE_H
