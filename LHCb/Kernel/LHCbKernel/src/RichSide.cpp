
//-----------------------------------------------------------------------------
/** @file RichSide.cpp
 *
 *  Implementation file for enumeration : Rich::Side
 *
 *  CVS Log :-
 *  $Id: RichSide.cpp,v 1.3 2007-02-01 16:45:22 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-19
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/RichSide.h"

// boost
#include "boost/assign/list_of.hpp"

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

const Rich::Sides & Rich::sides()
{
  static Rich::Sides s = boost::assign::list_of(Rich::top)(Rich::bottom);
  return s;
}
