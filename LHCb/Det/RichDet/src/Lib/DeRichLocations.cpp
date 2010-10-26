
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

// boost
#include "boost/assign/list_of.hpp"

const std::string& DeRichLocations::location( const Rich::RadiatorType rad )
{
  // Make a static map containing the mapping between enum and string
  typedef std::map<Rich::RadiatorType, const std::string> RadLocMap;
  static const RadLocMap radLocMap = boost::assign::map_list_of
    ( Rich::Aerogel,  DeRichLocations::Aerogel  )
    ( Rich::Rich1Gas, DeRichLocations::Rich1Gas )
    ( Rich::Rich2Gas, DeRichLocations::Rich2Gas );

  // Find the given radiator in the map
  RadLocMap::const_iterator iRad = radLocMap.find(rad);

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
  static const DetLocMap detLocMap = boost::assign::map_list_of
    ( Rich::Rich1, DeRichLocations::Rich1 )
    ( Rich::Rich2, DeRichLocations::Rich2 );

  // Find the given detector in the map
  DetLocMap::const_iterator iDet = detLocMap.find(det);

  // All OK ?
  if ( detLocMap.end() == iDet )
  {
    throw Rich::Exception( "No Detector Element for detector type "+Rich::text(det) );
  }

  // return location string
  return iDet->second;
}
