
//-----------------------------------------------------------------------------
/** @file RichTrackID.cpp
 *
 *  Implementation file for class : RichTrackID
 *
 *  CVS Log :-
 *  $Id: RichTrackID.cpp,v 1.17 2006-06-21 13:37:52 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-23
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichTrackID.h"

// STD
#include <sstream>

// from Gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::text( const Rich::TrackParent::Type parent )
{
  switch( parent ) 
  {
  case Rich::TrackParent::Track:          return "Track";
  case Rich::TrackParent::MCParticle:     return "MCParticle";
  case Rich::TrackParent::MCRichTrack:    return "MCRichTrack";
  default:                                return "SHOULD NEVER SEE THIS";
  }
}

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::text( const Rich::Track::Type track )
{
  switch( track ) 
  {
  case Rich::Track::Trigger:      return "trigger"; // Place first for speed
  case Rich::Track::Forward:      return "forward";
  case Rich::Track::Match:        return "match";
  case Rich::Track::KsTrack:      return "KsTrack";
  case Rich::Track::VeloTT:       return "veloTT";
  case Rich::Track::Seed:         return "seed";
  case Rich::Track::Follow:       return "follow";
  case Rich::Track::Velo:         return "velo";
  case Rich::Track::Unknown:      return "unknown";
  case Rich::Track::Unusable:     return "unusable";
  case Rich::Track::MCRichTrack:  return "MCRichTrack";
  default:                        return "SHOULD NEVER SEE THIS";
  }
}

Rich::Track::Type Rich::Track::type( const std::string & name )
{
  if      ( "trigger"  == name )    { return Rich::Track::Trigger;  }
  else if ( "forward"  == name )    { return Rich::Track::Forward;  }
  else if ( "match"    == name )    { return Rich::Track::Match;    } 
  else if ( "KsTrack"  == name )    { return Rich::Track::KsTrack;  }
  else if ( "veloTT"   == name )    { return Rich::Track::VeloTT;   }
  else if ( "seed"     == name )    { return Rich::Track::Seed;     }
  else if ( "follow"   == name )    { return Rich::Track::Follow;   }
  else if ( "velo"     == name )    { return Rich::Track::Velo;     }
  else if ( "unknown"  == name )    { return Rich::Track::Unknown;  }
  else if ( "unusable" == name )    { return Rich::Track::Unusable; }
  else if ( "MCRichTrack" == name ) { return Rich::Track::MCRichTrack;}
  else 
  { // Should not get here ...
    throw GaudiException( "Unknown track string name '"+name+"'",
                          "*Rich::Track::type*", StatusCode::FAILURE );
    return Rich::Track::Unknown; 
  }
}

// Returns the enumerated type for a given Track
Rich::Track::Type Rich::Track::type( const LHCb::Track * track )
{
  if ( track ) 
  {
    // track algorithm type
    const LHCb::Track::History hist = (LHCb::Track::History)track->history();
    // check all known track types (order according to abundance)
    if      ( LHCb::Track::PatForward    == hist )  { return Rich::Track::Forward;  }
    else if ( LHCb::Track::TrackMatching == hist )  { return Rich::Track::Match;    }
    else if ( LHCb::Track::TsaTrack      == hist )  { return Rich::Track::Seed;     }
    else if ( LHCb::Track::TrackSeeding  == hist )  { return Rich::Track::Seed;     }
    //else if ( LHCb::Track::CnvKsTrack  == hist )  { return Rich::Track::KsTrack;  }
    else if ( LHCb::Track::TrackVeloTT   == hist )  { return Rich::Track::VeloTT;   }
    else if ( LHCb::Track::PatVeloTT     == hist )  { return Rich::Track::VeloTT;   }
    else if ( LHCb::Track::PatVelo       == hist )  { return Rich::Track::Velo;     }
    //else if ( LHCb::Track::CnvVeloBack == hist )  { return Rich::Track::Unusable; }
    else 
    { // Should not get here ...
      std::ostringstream mess;
      mess << "Unknown Track type : Track::History = " << track->history();
      throw GaudiException( mess.str(), "*Rich::Track::type*", StatusCode::FAILURE );
    }
  }

  // Should not get here either ...
  throw GaudiException( "Null Track pointer", "*Rich::Track::type*", StatusCode::FAILURE );
}

Rich::Track::Type Rich::Track::type( const LHCb::MCRichTrack * track )
{
  if ( track ) 
  {
    return Rich::Track::MCRichTrack;
  }
  // Should not get here either ...
  throw GaudiException( "Null Track pointer", "*Rich::Track::type*", StatusCode::FAILURE );
}
