// $Id: RichRecTrackParentType.cpp,v 1.1.1.1 2003-04-01 13:21:55 jonesc Exp $
// Include files 

// local
#include "RichRecBase/RichRecTrackParentType.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecTrackParentType
//
// 2002-07-12 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::text( const Rich::RecTrack::ParentType& parent ) {
  switch( parent ) {
  case Rich::RecTrack::TrStoredTrack:  return "reconstructed track";
  case Rich::RecTrack::MCParticle:     return "MC particle";
  default:                             return "?"; // should never happen
  }
}

