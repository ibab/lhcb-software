// $Id: RichGlobalPIDTkQuality.h,v 1.1.1.1 2003-04-01 13:21:56 jonesc Exp $
#ifndef RICHRECKERNEL_RICHGLOBALPIDTKQUALITY_H 
#define RICHRECKERNEL_RICHGLOBALPIDTKQUALITY_H 1

// Include files
#include <string>
#include <iostream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

/** @class RichGlobalPIDTkQuality RichGlobalPIDTkQuality.h RichRecKernel/RichGlobalPIDTkQuality.h
 *  
 *  Enumeration for Global PID track quality
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-12
 */

namespace Rich {

  namespace GlobalPID {
    enum TkQuality {
      Unknown = -1,
      Unusable,
      LikelihoodOnly,
      Physics
    };
  }

  /// Text conversion for Rich2Side enumeration
  std::string text( const Rich::GlobalPID::TkQuality& side ) ;

}

/// Implement StreamBuffer >> method for Rich::GlobalPID::TkQuality enumeration
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   Rich::GlobalPID::TkQuality& qual ) {
  int intType;
  s >> intType;
  qual = (Rich::GlobalPID::TkQuality)intType;
  return s;
}

/// Implement StreamBuffer << method for Rich::GlobalPID::TkQuality enumeration
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const Rich::GlobalPID::TkQuality& qual ) {
  s << (int)qual;
  return s;
}

/// Implement textual ostream << method for Rich::GlobalPID::TkQuality enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::GlobalPID::TkQuality& qual ) {
  s << Rich::text( qual );
  return s;
}

/// Implement textual MsgStream << method for Rich::Rich2Side enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::GlobalPID::TkQuality& qual ) {
  s << Rich::text( qual );
  return s;
}

#endif // RICHRECKERNEL_RICHGLOBALPIDTKQUALITY_H 
