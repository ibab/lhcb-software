// $Id: RichDetectorType.h,v 1.2 2003-04-15 16:10:33 jonrob Exp $
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

#endif // RICHKERNEL_RICHDETECTORTYPE_H
