// $Id: Rich1Side.cpp,v 1.3 2003-04-15 16:10:33 jonrob Exp $
// Include files 

// local
#include "RichKernel/Rich1Side.h"

//-----------------------------------------------------------------------------
// Implementation file for enumeration : Rich::Rich1Side
//
// 2002-06-19 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Text conversion for Rich1Side enumeration
std::string Rich::text( const Rich::Rich1Side& side ) {
  switch( side ) {
  case Rich::top:     return "top";
  case Rich::bottom:  return "bottom";
  default:            return "?"; // should never happen
  }
}
