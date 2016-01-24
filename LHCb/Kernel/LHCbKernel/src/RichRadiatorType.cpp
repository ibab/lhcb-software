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
  case Rich::CO2:                return "Rich CO2";
  case Rich::PMTQuartzWin:       return "PMTQuartzWin";
  case Rich::InvalidRadiator:    return "Invalid Radiator";
  default:                       return "SHOULD NEVER SEE THIS";
  }
}

const Rich::Radiators & Rich::radiators() noexcept
{
  static Rich::Radiators rads = {Rich::Aerogel, Rich::Rich1Gas, Rich::Rich2Gas};
  return rads;
}

