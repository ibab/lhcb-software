// $Id: RichDetector.h,v 1.1 2002-06-21 15:33:13 jonesc Exp $
#ifndef RICHKERNEL_RICHDETECTOR_H
#define RICHKERNEL_RICHDETECTOR_H 1

// std include
#include <string>
#include <vector>

// from gaudi
#include "GaudiKernel/MsgStream.h"

/** @enumeration RichDetector RichDetector.h RichKernel/RichDetector.h
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-19
 */

namespace Rich {

  /// Enumeration for Rich detectors 1 and 2
  enum Detector {
    Rich1 = 0,
    Rich2
  };

  /// Text conversion for Detector enumeration
  std::string text( const Rich::Detector& detector ) ;

  /// Std Vector typedef
  typedef std::vector<Rich::Detector> DetectorVector;

}

/// Implement textual ostream << method for Rich::Detector enumeration
inline ostream& operator << ( ostream& s,
                              const Rich::Detector& det ) {
  s << Rich::text( det );
  return s;
}

/// Implement textual MsgStream << method for Rich::Detector enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::Detector& det ) {
  s << Rich::text( det );
  return s;
}

#endif // RICHKERNEL_RICHDETECTOR_H
