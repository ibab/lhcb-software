// $Id: Rich2Side.h,v 1.6 2003-04-16 10:48:37 jonrob Exp $
#ifndef RICHKERNEL_RICH2SIDE_H 
#define RICHKERNEL_RICH2SIDE_H 1

// Include files
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/MsgStream.h"

/** @class Rich2Side Rich2Side.h
 *  
 *  Enumeration for Rich2 detector splits
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-19
 */

namespace Rich {

  /// Enumeration for Rich2 detector splits
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
