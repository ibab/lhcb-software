// $Id: RichSide.h,v 1.2 2004-06-29 14:28:38 jonesc Exp $
#ifndef LHCBKERNEL_RICHSIDE_H
#define LHCBKERNEL_RICHSIDE_H 1

// Include files
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/MsgStream.h"

// namespace for RICH detector specifics
namespace Rich {

  /** @enum Rich::Side
   *  Enumeration for RICH detector splits
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2002-06-19
   */
  enum Side {
    InvalidSide  = -1,
    top          = 0,
    bottom       = 1,
    left         = 0,
    right        = 1
  };

  /// Text conversion for Rich::Side enumeration
  inline std::string text( const Rich::Side & side ) 
  {
    switch( side ) {
    case Rich::top:           return "top or left"; // Note also covers  Rich::left
    case Rich::bottom:        return "bottom or right"; // Note also covers Rich::right
    case Rich::InvalidSide:   return "Invalid side";
    default:                  return "?"; // should never happen
    }
  }

  /// Std Vector typedef
  typedef std::vector<Side> RichSideVector;

}

/// Implement textual ostream << method for Rich::Side enumeration
inline std::ostream& operator << ( std::ostream & s,
                                   const Rich::Side & side ) {
  s << Rich::text( side );
  return s;
}

/// Implement textual MsgStream << method for Rich::Side enumeration
inline MsgStream& operator << ( MsgStream & s,
                                const Rich::Side & side ) {
  s << Rich::text( side );
  return s;
}

#endif // LHCBKERNEL_RICHSIDE_H
