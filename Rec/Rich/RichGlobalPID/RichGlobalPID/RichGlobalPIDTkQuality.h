
/** @file RichGlobalPIDTkQuality.h
 *
 *  Header file for track quality enumeration for the Global PID algorithm
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
#include "GaudiKernel/MsgStream.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

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

      /// Text conversion for Global PID track quality enumeration
      inline std::string text( const Rich::Rec::GlobalPID::TkQuality& track )
      {
        switch( track )
        {
        case Rich::Rec::GlobalPID::Unknown:         return "unknown";
        case Rich::Rec::GlobalPID::Unusable:        return "unusable";
        case Rich::Rec::GlobalPID::LikelihoodOnly:  return "likelihood only";
        case Rich::Rec::GlobalPID::Physics:         return "physics";
        default:                                    return "SHOULD NEVER SEE THIS";
        }
      }

    }
  }
}

/// Implement textual ostream << method for Rich::GlobalPID::TkQuality enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::Rec::GlobalPID::TkQuality& qual )
{
  return s << Rich::Rec::GlobalPID::text( qual );
}

#endif // RICHGLOBALPID_RICHGLOBALPIDTKQUALITY_H
