// $Id: RichGlobalPIDTkQuality.h,v 1.2 2003-07-08 14:22:16 cattanem Exp $
#ifndef RICHGLOBALPID_RICHGLOBALPIDTKQUALITY_H 
#define RICHGLOBALPID_RICHGLOBALPIDTKQUALITY_H 1

// Include files
#include <string>
#include <iostream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

/** @class RichGlobalPIDTkQuality RichGlobalPIDTkQuality.h RichGlobalPID/RichGlobalPIDTkQuality.h
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

/// Text conversion for Global PID track quality enumeration
  inline  std::string text( const Rich::GlobalPID::TkQuality& track ) {
    switch( track ) {
    case Rich::GlobalPID::Unknown:         return "unknown";
    case Rich::GlobalPID::Unusable:        return "unusable";
    case Rich::GlobalPID::LikelihoodOnly:  return "likelihood only";
    case Rich::GlobalPID::Physics:         return "physics";
    default:                               return "?"; // should never happen
	}
  }

}
  
/// Implement text conversion for Global PID track quality enumeration

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

/// Implement textual MsgStream << method for Rich::GlobalPID::TkQuality enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::GlobalPID::TkQuality& qual ) {
  s << Rich::text( qual );
  return s;
}

#endif // RICHGLOBALPID_RICHGLOBALPIDTKQUALITY_H 
