// $Id: RichTrackID.cpp,v 1.1 2003-10-13 16:10:54 jonrob Exp $

// local
#include "RichRecBase/RichTrackID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTrackID
//
// 2003-09-23 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Constructor from a ContainedObject
RichTrackID::RichTrackID( const ContainedObject * obj )
  : m_tkType     ( Rich::Track::Unknown ),
    m_parentType ( Rich::TrackParent::Unknown ),
    m_history    ( 0 ),
    m_unique     ( true )  {

  if ( const TrStoredTrack * track =
       dynamic_cast<const TrStoredTrack *>(obj) ) {
    initTrStoredTrack(track);
  } else if ( const MCParticle * mcP =
              dynamic_cast<const MCParticle *>(obj) ) {
    initMCParticle( mcP );
  } else {
    std::cout << "RichTrackID ERROR : Unknown parent type" << std::endl;
  }
  
}

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
  case Rich::Track::Unknown:      return "unknown";
  case Rich::Track::Unusable:     return "unusable";
  case Rich::Track::Velo:         return "velo";
  case Rich::Track::VeloTT:       return "veloTT";
  case Rich::Track::Seed:         return "seed";
  case Rich::Track::UpStream:     return "upstream";
  case Rich::Track::Match:        return "match";
  case Rich::Track::Forward:      return "forward";
  default:                        return "?"; // should never happen
  }
}

// Returns the enumerated type for a given TrStoredTrack
Rich::Track::Type Rich::Track::type( const TrStoredTrack * track ) {

  if ( track ) {
    if      ( track->forward() )  { return Rich::Track::Forward;  }
    else if ( track->match() )    { return Rich::Track::Match;    }
    else if ( track->upstream() ) { return Rich::Track::UpStream; }
    else if ( track->seed() )     { return Rich::Track::Seed;     }
    else if ( track->veloTT() )   { return Rich::Track::VeloTT;   }
    else if ( track->velo() )     { return Rich::Track::Velo;     }
  }

  return Rich::Track::Unknown;
}
