// $Id: Rich2Side.h,v 1.1 2002-06-21 15:32:56 jonesc Exp $
#ifndef RICH2SIDE_H 
#define RICH2SIDE_H 1

// Include files
#include <string>
#include <vector>

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
inline ostream& operator << ( ostream& s,
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

#endif // RICH2SIDE_H
