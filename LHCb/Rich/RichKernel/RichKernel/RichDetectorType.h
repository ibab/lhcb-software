// $Id: RichDetectorType.h,v 1.3 2003-04-15 17:50:49 jonrob Exp $
#ifndef RICHKERNEL_RICHDETECTORTYPE_H
#define RICHKERNEL_RICHDETECTORTYPE_H 1

// std include
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

/** @enumeration RichDetectorType RichDetectorType.h RichKernel/RichDetectorType.h
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-19
 */

namespace Rich {

  /// Number of Rich detectors
  static const int NRiches = 2;

  /// Enumeration for Rich detectors 1 and 2
  enum DetectorType {
    Rich1 = 0,
    Rich2
  };

  /// Text conversion for DetectorType enumeration
  std::string text( const Rich::DetectorType& detector );

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

// Text conversion for Detector enumeration
inline std::string Rich::text( const Rich::DetectorType& detector )  {
  switch( detector ) {
  case Rich::Rich1:   return "Rich1";
  case Rich::Rich2:   return "Rich2";
  default:            return "?"; // should never happen
  }
}


#endif // RICHKERNEL_RICHDETECTORTYPE_H
