
/** @file RichGlobalPIDTkQuality.h
 *
 *  Header file for track quality enumeration for the Global PID algorithm
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDTkQuality.h,v 1.3 2004-07-27 10:56:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */

#ifndef RICHGLOBALPID_RICHGLOBALPIDTKQUALITY_H
#define RICHGLOBALPID_RICHGLOBALPIDTKQUALITY_H 1

// Include files
#include <string>
#include <iostream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

/** @namespace Rich
 *
 *  General namespace for RICH specific definitions
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-12
 */
namespace Rich {

  /** @namespace Rich::GlobalPID
   *
   *  Enumeration for Global PID track quality
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2002-07-12
   */
  namespace GlobalPID {

    /** @enum TkQuality
     *
     *  Track quality for the Global PID
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   08/07/2004
     */
    enum TkQuality
      {
        Unknown = -1,    ///< Track quality
        Unusable,        ///< Track is not usable
        LikelihoodOnly,  ///< Track can be used during the likelihood minimisation, but not for physics
        Physics          ///< Track is of physics quality
      };


  }

  /// Text conversion for Global PID track quality enumeration
  inline  std::string text( const Rich::GlobalPID::TkQuality& track )
  {
    switch( track ) {
    case Rich::GlobalPID::Unknown:         return "unknown";
    case Rich::GlobalPID::Unusable:        return "unusable";
    case Rich::GlobalPID::LikelihoodOnly:  return "likelihood only";
    case Rich::GlobalPID::Physics:         return "physics";
    default:                               return "?"; // should never happen
    }
  }

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

/// Implement textual MsgStream << method for Rich::GlobalPID::TkQuality enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::GlobalPID::TkQuality& qual ) {
  s << Rich::text( qual );
  return s;
}

#endif // RICHGLOBALPID_RICHGLOBALPIDTKQUALITY_H
