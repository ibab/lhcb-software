
//-----------------------------------------------------------------------------
/** @file RichRadiatorType.cpp
 *
 *  Implementation file for enumeration : Rich::RadiatorType
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/RichRadiatorType.h"

#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L)
// boost
#include "boost/assign/list_of.hpp"
#endif

// Text conversion for RadiatorType enumeration
std::string Rich::text( const Rich::RadiatorType radiator )
{
  switch( radiator )
  {
  case Rich::Aerogel:            return "Aerogel";
  case Rich::Rich1Gas:           return "Rich1Gas";
  case Rich::Rich2Gas:           return "Rich2Gas";
  case Rich::GasQuartzWin:       return "GasQuartzWin";
  case Rich::HPDQuartzWin:       return "HPDQuartzWin";
  case Rich::Nitrogen:           return "Nitrogen";
  case Rich::AerogelFilter:      return "AerogelFilter";
  case Rich::InvalidRadiator:    return "Invalid Radiator";
  default:                       return "SHOULD NEVER SEE THIS";
  }
}

const Rich::Radiators & Rich::radiators()
{
  static Rich::Radiators rads =
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
    {Rich::Aerogel, Rich::Rich1Gas, Rich::Rich2Gas};
#else
    boost::assign::list_of(Rich::Aerogel)(Rich::Rich1Gas)(Rich::Rich2Gas);
#endif
  return rads;
}

