// $Id: RichTrackType.h,v 1.1.1.1 2003-04-01 13:21:56 jonesc Exp $
#ifndef RICHRECKERNEL_RICHTRACKTYPE_H 
#define RICHRECKERNEL_RICHTRACKTYPE_H 1

// Include files
#include <string>
#include <iostream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

// TrEvent
#include "Event/TrStoredTrack.h"

/** @class RichTrackType RichTrackType.h RichRecKernel/RichTrackType.h
 *  
 *  Enumeration for track type
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-12
 */

namespace Rich {

  namespace Track {

    /// Number of Rich particle types
    static const int NTrTypes = 6;

    enum Type {
      Unknown = -2,
      Unusable,
      Forward,
      Match,
      UpStream,
      Seed,
      VeloTT,
      Velo
    };

    Rich::Track::Type type( const TrStoredTrack * track );

  }

  /// Text conversion for Rich::Track::Type enumeration
  std::string text( const Rich::Track::Type& side );
  std::string text( const TrStoredTrack * track );

}

/// Implement StreamBuffer >> method for Rich::Track::Type enumeration
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   Rich::Track::Type& qual ) {
  int intType;
  s >> intType;
  qual = (Rich::Track::Type)intType;
  return s;
}

/// Implement StreamBuffer << method for Rich::Track::Type enumeration
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const Rich::Track::Type& qual ) {
  s << (int)qual;
  return s;
}

/// Implement textual ostream << method for Rich::Track::Type enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::Track::Type& qual ) {
  s << Rich::text( qual );
  return s;
}

/// Implement textual MsgStream << method for Rich::Track::Type enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::Track::Type& qual ) {
  s << Rich::text( qual );
  return s;
}

inline std::string Rich::text( const TrStoredTrack * track ) {
  return Rich::text( Rich::Track::type(track) );
}

#endif // RICHRECKERNEL_RICHTRACKTYPE_H
