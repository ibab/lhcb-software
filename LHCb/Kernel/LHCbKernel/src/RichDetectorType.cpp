
//-----------------------------------------------------------------------------
/** @file RichDetectorType.cpp
 *
 *  Implementation file for enumeration : Rich::DetectorType
 *
 *  CVS Log :-
 *  $Id: RichDetectorType.cpp,v 1.1 2005-10-17 08:56:55 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/RichDetectorType.h"

// Text conversion for DetectorType enumeration
std::string Rich::text( const Rich::DetectorType detector )
{
  switch( detector ) 
  {
  case Rich::Rich1:              return "Rich1";
  case Rich::Rich2:              return "Rich2";
  case Rich::InvalidDetector:    return "Invalid Detector";
  default:                       return "SHOULD NEVER SEE THIS";
  }
}
