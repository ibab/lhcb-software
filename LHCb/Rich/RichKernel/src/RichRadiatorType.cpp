// $Id: RichRadiatorType.cpp,v 1.2 2002-11-07 17:23:40 jonrob Exp $
// Include files 

// local
#include "RichKernel/RichRadiatorType.h"

//-----------------------------------------------------------------------------
// Implementation file for enumeration : Rich::RadiatorType
//
// 2002-06-19 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Text conversion for RadiatorType enumeration
std::string Rich::text( const Rich::RadiatorType& radiator ) {
  switch( radiator ) {
  case Rich::Aerogel:      return "aerogel";
  case Rich::C4F10:        return "C4F10";
  case Rich::CF4:          return "CF4";
  default:                 return "?"; // should never happen
  }
}
