// $Id: RichRadiatorType.h,v 1.2 2002-07-03 06:08:46 cattanem Exp $
#ifndef RICHKERNEL_RICHRADIATORTYPE_H
#define RICHKERNEL_RICHRADIATORTYPE_H 1

// Include files
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/MsgStream.h"

/** @enumeration RichRadiatorType RichRadiatorType.h RichKernel/RichRadiatorType.h
 *
 *  Enumeration for Rich radiator types
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-19
 */

namespace Rich {

  /// Enumeration for radiator types
  enum RadiatorType {
    Aerogel = 0,
    C4F10,
    CF4
  };

  /// Text conversion for RadiatorType enumeration
  std::string text( const Rich::RadiatorType& radiator ) ;

  /// Std Vector typedef
  typedef std::vector<RadiatorType> RadiatorTypeVector;

}

/// Implement textual ostream << method for Rich::RadiatorType enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::RadiatorType& radiator ) {
  s << Rich::text( radiator );
  return s;
}

/// Implement textual MsgStream << method for Rich::RadiatorType enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::RadiatorType& radiator ) {
  s << Rich::text( radiator );
  return s;
}

#endif // RICHKERNEL_RICHRADIATORTYPE_H
