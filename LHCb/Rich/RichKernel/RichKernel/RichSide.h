// $Id: RichSide.h,v 1.1 2002-06-21 15:33:29 jonesc Exp $
#ifndef RICHKERNEL_RICHSIDE_H
#define RICHKERNEL_RICHSIDE_H 1

// Include files
#include <string>
#include <vector>

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
  std::string text( const Rich::Side& side ) ;

  /// Std Vector typedef
  typedef std::vector<Side> RichSideVector;

}

/// Implement textual ostream << method for Rich::Side enumeration
inline ostream& operator << ( ostream& s,
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
