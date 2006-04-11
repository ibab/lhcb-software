
//-----------------------------------------------------------------------------
/** @file RichSide.cpp
 *
 *  Implementation file for enumeration : Rich::Side
 *
 *  CVS Log :-
 *  $Id: RichSide.cpp,v 1.2 2006-04-11 14:13:25 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-19
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/RichSide.h"

// Text conversion for Rich::Side enumeration
std::string Rich::text( const Rich::Side side )
{
  switch( side ) 
  {
  case Rich::top:           return "top or left"; // Note also covers  Rich::left
  case Rich::bottom:        return "bottom or right"; // Note also covers Rich::right
  case Rich::InvalidSide:   return "Invalid side";
  default:                  return "SHOULD NEVER SEE THIS";
  }
}
