
/** @file RichSide.h
 *
 *  Header file for RICH particle ID enumeration : RichSide
 *
 *  CVS Log :-
 *  $Id: RichSide.h,v 1.4 2004-08-17 11:10:41 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */

#ifndef LHCBKERNEL_RICHSIDE_H
#define LHCBKERNEL_RICHSIDE_H 1

/** @file RichSide.h
 *
 *  Header file for enumeration : RichSide
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */

// Include files
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/MsgStream.h"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich {

  /** @enum Rich::Side
   *
   *  RICH detector side enumeration. Used to indentify the "side" of
   *  each of the RICH detectors.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  enum Side
    {
      InvalidSide  = -1, ///< Invalid side
      top          = 0,  ///< Upper panel in RICH1
      bottom       = 1,  ///< Lower panel in RICH1
      left         = 0,  ///< Left panel in RICH2
      right        = 1   ///< Right panel in RICH2
    };

  /** Text conversion for Rich::Side enumeration
   *
   *  @param side Rich Side enumeration
   *  @return Rich Side as an std::string
   */
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
