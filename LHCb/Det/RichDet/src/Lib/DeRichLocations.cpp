
//----------------------------------------------------------------------------
/** @file DeRichLocations.h
 *
 *  Implementation for for DeRichLocation methods
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2006-03-02
 */
//----------------------------------------------------------------------------

// STL
#include <map>

// Local
#include "RichDet/DeRichLocations.h"

// RichKernel
#include "RichKernel/RichException.h"

const std::string& DeRichLocations::location( const Rich::RadiatorType rad )
{
  // Make a static map containing the mapping between enum and string
  typedef std::map<Rich::RadiatorType, const std::string> RadLocMap;
  static const RadLocMap radLocMap =
  {{Rich::Aerogel,  DeRichLocations::Aerogel  },
   {Rich::Rich1Gas, DeRichLocations::Rich1Gas },
   {Rich::Rich2Gas, DeRichLocations::Rich2Gas }};

  // Find the given radiator in the map
  const auto iRad = radLocMap.find(rad);

  // All OK ?
  if ( radLocMap.end() == iRad )
  {
    throw Rich::Exception( "No Detector Element for radiator type "+Rich::text(rad) );
  }

  // return location string
  return iRad->second;
}

const std::string& DeRichLocations::location( const Rich::DetectorType det )
{
  // Make a static map containing the mapping between enum and string
  typedef std::map<Rich::DetectorType, const std::string> DetLocMap;
  static const DetLocMap detLocMap =
  {{Rich::Rich1, DeRichLocations::Rich1 },
   {Rich::Rich2, DeRichLocations::Rich2 }};

  // Find the given detector in the map
  const auto iDet = detLocMap.find(det);

  // All OK ?
  if ( detLocMap.end() == iDet )
  {
    throw Rich::Exception( "No Detector Element for detector type "+Rich::text(det) );
  }

  // return location string
  return iDet->second;
}
