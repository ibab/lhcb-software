// $Id: RichTrackType.cpp,v 1.1.1.1 2003-04-01 13:21:55 jonesc Exp $
// Include files 

// local
#include "RichRecBase/RichTrackType.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTrackType
//
// 2002-07-12 : Chris Jones
//-----------------------------------------------------------------------------

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::text( const Rich::Track::Type& track ) {
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

Rich::Track::Type Rich::Track::type( const TrStoredTrack * track ) {
  
  if ( track ) {
    if      ( track->forward() )  { return Rich::Track::Forward; }
    else if ( track->match() )    { return Rich::Track::Match; }
    else if ( track->upstream() ) { return Rich::Track::UpStream; }
    else if ( track->seed() )     { return Rich::Track::Seed; }
    else if ( track->veloTT() )   { return Rich::Track::VeloTT; }
    else if ( track->velo() )     { return Rich::Track::Velo; }
  }
  
  return Rich::Track::Unknown;
}
