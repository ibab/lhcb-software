// $Id: RichDetectorType.h,v 1.2 2004-06-29 14:28:36 jonesc Exp $
#ifndef LHCBKERNEL_RICHDETECTORTYPE_H
#define LHCBKERNEL_RICHDETECTORTYPE_H 1

// std include
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

// namespace for RICH detector specifics
namespace Rich {

  /// Number of RICH detectors
  static const int NRiches = 2;

  /** @enum Rich::DetectorType 
   *  Enumeration for the main RICH detectors 
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2002-06-19
   */
  enum DetectorType {
    InvalidDetector = -1,
    Rich1           = 0,
    Rich2           = 1
  };

  /// Text conversion for DetectorType enumeration
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
