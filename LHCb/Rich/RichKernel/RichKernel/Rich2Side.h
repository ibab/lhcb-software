// $Id: Rich2Side.h,v 1.3 2003-04-01 11:22:56 jonrob Exp $
#ifndef RICH2SIDE_H 
#define RICH2SIDE_H 1

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
  std::string text( const Rich::Rich2Side& side ) ;

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
inline std::string Rich::text( const Rich::Rich2Side& side ) {
  switch( side ) {
  case Rich::left:    return "left";
  case Rich::right:   return "right";
  default:            return "?"; // should never happen
  }
}


#endif // RICH2SIDE_H
