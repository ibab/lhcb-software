// $Id: Rich2Side.cpp,v 1.1 2002-06-21 15:33:51 jonesc Exp $
// Include files 

// local
#include "RichKernel/Rich2Side.h"

//-----------------------------------------------------------------------------
// Implementation file for enumeration : Rich::Rich2Side
//
// 2002-06-19 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Text conversion for Rich2Side enumeration
std::string Rich::text( const Rich::Rich2Side& side ) {
  switch( side ) {
  case Rich::left:    return "left";
  case Rich::right:   return "right";
  default:            return "?"; // should never happen
  }
}
