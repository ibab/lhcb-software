// $Id: RichDetectorType.h,v 1.7 2003-11-03 10:19:12 cattanem Exp $
#ifndef RICHKERNEL_RICHDETECTORTYPE_H
#define RICHKERNEL_RICHDETECTORTYPE_H 1

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
    Rich1 = 0,
    Rich2
  };

  /// Text conversion for DetectorType enumeration
  inline std::string text( const Rich::DetectorType& detector ) {
    switch( detector ) {
    case Rich::Rich1:   return "Rich1";
    case Rich::Rich2:   return "Rich2";
    default:            return "?"; // should never happen
    }
  }

  /// Std Vector typedef
  typedef std::vector<Rich::DetectorType> DetectorVector;

}

/// Implement StreamBuffer >> method for Rich::DetectorType enumeration
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   Rich::DetectorType& detector ) {
  int intType;
  s >> intType;
  detector = (Rich::DetectorType)intType;
  return s;
}

/// Implement StreamBuffer << method for Rich::DetectorType enumeration
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const Rich::DetectorType& detector ) {
  s << (int)detector;
  return s;
}

/// Implement textual ostream << method for Rich::DetectorType enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::DetectorType& detector ) {
  s << Rich::text( detector );
  return s;
}

/// Implement textual MsgStream << method for Rich::DetectorType enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::DetectorType& detector ) {
  s << Rich::text( detector );
  return s;
}

#endif // RICHKERNEL_RICHDETECTORTYPE_H
