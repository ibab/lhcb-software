
//-----------------------------------------------------------------------------
/** @file RichRadiatorType.cpp
 *
 *  Implementation file for enumeration : Rich::RadiatorType
 *
 *  CVS Log :-
 *  $Id: RichRadiatorType.cpp,v 1.6 2006-02-16 15:48:37 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/RichRadiatorType.h"

// boost
//#include "boost/assign/list_of.hpp"

// Text conversion for RadiatorType enumeration
std::string Rich::text( const Rich::RadiatorType radiator )
{
  switch( radiator )
  {
  case Rich::Aerogel:            return "aerogel";
  case Rich::C4F10:              return "C4F10";
  case Rich::CF4:                return "CF4";
  case Rich::InvalidRadiator:    return "Invalid Radiator";
  default:                       return "SHOULD NEVER SEE THIS";
  }
}

/*
const Rich::Radiators & Rich::radiators()
{
  static const Rich::Radiators rads =
    boost::assign::list_of(Rich::Aerogel)(Rich::Rich1Gas)(Rich::Rich2Gas);
  return rads;
}
*/
