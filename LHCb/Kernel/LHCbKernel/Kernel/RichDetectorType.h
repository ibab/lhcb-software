// $Id: RichDetectorType.h,v 1.3 2004-08-17 11:07:23 jonrob Exp $
// ===========================================================================
// $Log: not supported by cvs2svn $

#ifndef LHCBKERNEL_RICHDETECTORTYPE_H
#define LHCBKERNEL_RICHDETECTORTYPE_H 1

/** @file RichDetectorType.h
 *
 *  Header file for enumeration : RichDetectorType
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */

// std include
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
namespace Rich {

  /// Number of RICH detectors
  static const int NRiches = 2;

  /** @enum Rich::DetectorType
   *
   *  RICH Detector types
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  enum DetectorType
    {
      InvalidDetector = -1, ///< Unspecified Detector
      Rich1           = 0,  ///< RICH1 detector
      Rich2           = 1   ///< RICH2 detector
    };

  /** Text conversion for DetectorType enumeration
   *
   *  @param detector RICH detector enumeration
   *  @return Detector type as a string
   */
  inline std::string text( const Rich::DetectorType & detector )
  {
    switch( detector ) {
    case Rich::Rich1:              return "Rich1";
    case Rich::Rich2:              return "Rich2";
    case Rich::InvalidDetector:    return "Invalid Detector";
    default:                       return "?"; // should never happen
    }
  }

  /// Std Vector typedef
  typedef std::vector<Rich::DetectorType> DetectorVector;

}

/// Implement StreamBuffer >> method for Rich::DetectorType enumeration
inline StreamBuffer& operator >> ( StreamBuffer & s,
                                   Rich::DetectorType & detector ) {
  int intType;
  s >> intType;
  detector = static_cast<Rich::DetectorType>(intType);
  return s;
}

/// Implement StreamBuffer << method for Rich::DetectorType enumeration
inline StreamBuffer& operator << ( StreamBuffer & s,
                                   const Rich::DetectorType & detector ) {
  s << static_cast<int>(detector);
  return s;
}

/// Implement textual ostream << method for Rich::DetectorType enumeration
inline std::ostream& operator << ( std::ostream & s,
                                   const Rich::DetectorType & detector ) {
  s << Rich::text( detector );
  return s;
}

/// Implement textual MsgStream << method for Rich::DetectorType enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::DetectorType detector ) {
  s << Rich::text( detector );
  return s;
}

#endif // LHCBKERNEL_RICHDETECTORTYPE_H
