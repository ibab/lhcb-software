
//-----------------------------------------------------------------------------
/** @file RichDetectorType.cpp
 *
 *  Implementation file for enumeration : Rich::DetectorType
 *
 *  CVS Log :-
 *  $Id: RichDetectorType.cpp,v 1.2 2007-02-01 16:45:22 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/RichDetectorType.h"

// boost
#include "boost/assign/list_of.hpp"

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

const Rich::Detectors & Rich::detectors()
{
  static Rich::Detectors dets =
    boost::assign::list_of(Rich::Rich1)(Rich::Rich2);
  return dets;
}
