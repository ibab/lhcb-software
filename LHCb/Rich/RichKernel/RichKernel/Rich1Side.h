// $Id: Rich1Side.h,v 1.3 2003-04-01 11:22:56 jonrob Exp $
#ifndef RICHKERNEL_RICH1SIDE_H 
#define RICHKERNEL_RICH1SIDE_H 1

// Include files
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/MsgStream.h"

/** @enumeration Rich1Side Rich1Side.h RichKernel/Rich1Side.h
 *  
 *  Enumeration for Rich1 detector splits
 *
 *  @author Chris Jones
 *  @date   2002-06-19
 */

namespace Rich {

  /// Enumeration for Rich1 detector splits
  enum Rich1Side {
    top = 0,
    bottom
  };

  /// Text conversion for Rich1Side enumeration
  std::string text( const Rich::Rich1Side& side ) ;

  /// Std Vector typedef
  typedef std::vector<Rich1Side> Rich1SideVector;

}

/// Implement textual ostream << method for Rich::Rich1Side enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::Rich1Side& side ) {
  s << Rich::text( side );
  return s;
}

/// Implement textual MsgStream << method for Rich::Rich1Side enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::Rich1Side& side ) {
  s << Rich::text( side );
  return s;
}

// Text conversion for Rich1Side enumeration
inline std::string Rich::text( const Rich::Rich1Side& side ) {
  switch( side ) {
  case Rich::top:     return "top";
  case Rich::bottom:  return "bottom";
  default:            return "?"; // should never happen
  }
}

#endif // RICHKERNEL_RICH1SIDE_H
