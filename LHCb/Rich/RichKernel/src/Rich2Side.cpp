// $Id: Rich2Side.cpp,v 1.3 2003-04-15 16:10:33 jonrob Exp $
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
