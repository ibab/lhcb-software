// $Id: RichRadiatorType.cpp,v 1.2 2004-06-29 14:28:38 jonesc Exp $
// Include files 

// local
#include "Kernel/RichRadiatorType.h"

//-----------------------------------------------------------------------------
// Implementation file for enumeration : Rich::RadiatorType
//
// 2002-06-19 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Text conversion for RadiatorType enumeration
std::string Rich::text( const Rich::RadiatorType & radiator ) 
{
  switch( radiator ) {
  case Rich::Aerogel:            return "aerogel";
  case Rich::C4F10:              return "C4F10";
  case Rich::CF4:                return "CF4";
  case Rich::InvalidRadiator:    return "Invalid Radiator";
  default:                       return "?"; // should never happen
  }
}
