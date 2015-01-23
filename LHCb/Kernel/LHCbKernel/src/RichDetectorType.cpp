
//-----------------------------------------------------------------------------
/** @file RichDetectorType.cpp
 *
 *  Implementation file for enumeration : Rich::DetectorType
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/RichDetectorType.h"

#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L)
// boost
#include "boost/assign/list_of.hpp"
#endif

// Text conversion for DetectorType enumeration
std::string Rich::text( const Rich::DetectorType detector )
{
  switch( detector )
  {
  case Rich::Rich1:              return "Rich1";
  case Rich::Rich2:              return "Rich2";
    //case Rich::Rich :              return "Single Rich";
  case Rich::InvalidDetector:    return "Invalid Detector";
  default:                       return "SHOULD NEVER SEE THIS";
  }
}

const Rich::Detectors & Rich::detectors()
{
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  static Rich::Detectors dets = {Rich::Rich1, Rich::Rich2};
#else
  static Rich::Detectors dets =
    boost::assign::list_of(Rich::Rich1)(Rich::Rich2);
#endif
  return dets;
}
