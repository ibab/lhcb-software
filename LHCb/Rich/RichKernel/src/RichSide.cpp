// $Id: RichSide.cpp,v 1.3 2003-04-15 16:10:33 jonrob Exp $
// Include files 

// local
#include "RichKernel/RichSide.h"

//-----------------------------------------------------------------------------
// Implementation file for enumeration : Rich::Side
//
// 2002-06-19 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Text conversion for Rich::Side enumeration
std::string Rich::text( const Rich::Side& side ) {
  switch( side ) {
  case Rich::top:    return "top or left"; // Note also covers  Rich::left
  case Rich::bottom: return "bottom or right"; // Note also covers  Rich::right
  default:           return "?"; // should never happen
  }
}
