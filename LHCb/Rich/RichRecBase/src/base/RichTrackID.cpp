// $Id: RichTrackID.cpp,v 1.3 2004-03-16 13:40:00 jonesc Exp $

// local
#include "RichRecBase/RichTrackID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTrackID
//
// 2003-09-23 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::text( const Rich::TrackParent::Type parent ) {
  switch( parent ) {
  case Rich::TrackParent::TrStoredTrack:  return "TrStoredTrack";
  case Rich::TrackParent::MCParticle:     return "MCParticle";
  default:                                return "?"; // should never happen
  }
}

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::text( const Rich::Track::Type track ) {
  switch( track ) {
  case Rich::Track::Forward:      return "forward";
  case Rich::Track::Match:        return "match";
  case Rich::Track::Follow:       return "follow";
  case Rich::Track::VeloTT:       return "veloTT";
  case Rich::Track::Seed:         return "seed";
  case Rich::Track::KsTrack:      return "KsTrack";
  case Rich::Track::Velo:         return "velo";
  case Rich::Track::Unknown:      return "unknown";
  case Rich::Track::Unusable:     return "unusable";
  default:                        return "?"; // should never happen
  }
}

// Returns the enumerated type for a given TrStoredTrackelse
Rich::Track::Type Rich::Track::type( const TrStoredTrack * track ) {

  if ( track ) {

    // check all known track types (order ifs according to abundance)
    if      ( track->forward()  )     { return Rich::Track::Forward;  }
    else if ( track->match()    )     { return Rich::Track::Match;    }
    else if ( track->follow()   )     { return Rich::Track::Follow;   }
    else if ( track->veloTT()   )     { return Rich::Track::VeloTT;   }
    else if ( track->seed()     )     { return Rich::Track::Seed;     }
    else if ( track->ksTrack()  )     { return Rich::Track::KsTrack;  }
    else if ( track->velo()     )     { return Rich::Track::Velo;     }
    else if ( track->veloBack() )     { return Rich::Track::Unusable; }
    else { // Should not get here...
      std::ostringstream mess;
      mess << "Unknown TrStoredTrack type : history = " << track->history();
      throw GaudiException( mess.str(), "RichTrackID", StatusCode::FAILURE );
    }

  }

  return Rich::Track::Unknown;
}
