// $Id: RichRecTrackParentType.h,v 1.2 2003-06-30 15:11:57 jonrob Exp $
#ifndef RICHRECKERNEL_RICHRECTRACKPARENTTYPE_H
#define RICHRECKERNEL_RICHRECTRACKPARENTTYPE_H 1

// Include files
#include <string>
#include <iostream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

/// namespace for RICH detector specifics
namespace Rich {

  /// namespace for RichRecPixel objects
  namespace RecTrack {

    /** @class Enumeration for Rich reconstruction working track object
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2002-07-12
     */
    enum ParentType {
      Unknown = -1,
      TrStoredTrack,
      MCParticle
    };
  }

  /// Text conversion for Rich::RecTrack::ParentType enumeration
  std::string text( const Rich::RecTrack::ParentType& parent ) ;

}

/// Implement StreamBuffer >> method for Rich::RecTrack::ParentType enumeration
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   Rich::RecTrack::ParentType& track ) {
  int intType;
  s >> intType;
  track = (Rich::RecTrack::ParentType)intType;
  return s;
}

/// Implement StreamBuffer << method for Rich::RecTrack::ParentType enumeration
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const Rich::RecTrack::ParentType& track ) {
  s << (int)track;
  return s;
}

/// Implement textual ostream << method for Rich::RecTrack::ParentType enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::RecTrack::ParentType& track ) {
  s << Rich::text( track );
  return s;
}

/// Implement textual MsgStream << method for Rich::Rich2Side enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::RecTrack::ParentType& track ) {
  s << Rich::text( track );
  return s;
}

#endif // RICHRECKERNEL_RICHRECTRACKPARENTTYPE_H
