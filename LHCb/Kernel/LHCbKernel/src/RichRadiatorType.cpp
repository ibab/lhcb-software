
//-----------------------------------------------------------------------------
/** @file RichRadiatorType.cpp
 *
 *  Implementation file for enumeration : Rich::RadiatorType
 *
 *  CVS Log :-
 *  $Id: RichRadiatorType.cpp,v 1.5 2005-10-17 08:56:55 jonrob Exp $
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
  case Rich::Aerogel:            return "aerogel";
  case Rich::C4F10:              return "C4F10";
  case Rich::CF4:                return "CF4";
  case Rich::InvalidRadiator:    return "Invalid Radiator";
  default:                       return "SHOULD NEVER SEE THIS";
  }
}
