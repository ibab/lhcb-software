
//-----------------------------------------------------------------------------
/** @file RichTrackID.cpp
 *
 *  Implementation file for class : RichTrackID
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
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::Rec::text( const Rich::Rec::TrackParent::Type parent )
{
  switch( parent )
  {
  case Rich::Rec::TrackParent::Track:          return "Track";
  case Rich::Rec::TrackParent::MCParticle:     return "MCParticle";
  case Rich::Rec::TrackParent::MCRichTrack:    return "MCRichTrack";
  default:                                     return "SHOULD NEVER SEE THIS";
  }
}

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::Rec::text( const Rich::Rec::Track::Type track )
{
  switch( track )
  {
  case Rich::Rec::Track::Trigger:      return "Trigger"; // Place first for speed
  case Rich::Rec::Track::Forward:      return "Forward";
  case Rich::Rec::Track::Match:        return "Match";
  case Rich::Rec::Track::KsTrack:      return "KsTrack";
  case Rich::Rec::Track::VeloTT:       return "VeloTT";
  case Rich::Rec::Track::Seed:         return "Seed";
  case Rich::Rec::Track::Follow:       return "Follow";
  case Rich::Rec::Track::Velo:         return "Velo";
  case Rich::Rec::Track::MuonID:       return "MuonID";
  case Rich::Rec::Track::Unknown:      return "Unknown";
  case Rich::Rec::Track::Unusable:     return "Unusable";
  case Rich::Rec::Track::MCRichTrack:  return "MCRichTrack";
  default:                             return "SHOULD NEVER SEE THIS";
  }
}

Rich::Rec::Track::Type Rich::Rec::Track::type( const std::string & name )
{
  if      ( "Trigger"     == name ) { return Rich::Rec::Track::Trigger;     }
  else if ( "Forward"     == name ) { return Rich::Rec::Track::Forward;     }
  else if ( "Match"       == name ) { return Rich::Rec::Track::Match;       }
  else if ( "KsTrack"     == name ) { return Rich::Rec::Track::KsTrack;     }
  else if ( "VeloTT"      == name ) { return Rich::Rec::Track::VeloTT;      }
  else if ( "Seed"        == name ) { return Rich::Rec::Track::Seed;        }
  else if ( "Follow"      == name ) { return Rich::Rec::Track::Follow;      }
  else if ( "Velo"        == name ) { return Rich::Rec::Track::Velo;        }
  else if ( "MuonID"      == name ) { return Rich::Rec::Track::MuonID;      }
  else if ( "Unknown"     == name ) { return Rich::Rec::Track::Unknown;     }
  else if ( "Unusable"    == name ) { return Rich::Rec::Track::Unusable;    }
  else if ( "MCRichTrack" == name ) { return Rich::Rec::Track::MCRichTrack; }
  else
  { // Should not get here ...
    throw GaudiException( "Unknown track string name '"+name+"'",
                          "*Rich::Rec::Track::type*", StatusCode::FAILURE );
    return Rich::Rec::Track::Unknown;
  }
}

// Returns the enumerated type for a given Track
Rich::Rec::Track::Type Rich::Rec::Track::type( const LHCb::Track * track )
{
  if ( track )
  {
    // track algorithm type
    const LHCb::Track::History hist = (LHCb::Track::History)track->history();
    // check all known track types (order according to abundance)
    // Forward types
    if      ( LHCb::Track::PatForward    == hist || 
              LHCb::Track::PrForward     == hist )  { return Rich::Rec::Track::Forward;  }
    // match track types
    else if ( LHCb::Track::TrackMatching == hist || 
              LHCb::Track::PatMatch      == hist ||
              LHCb::Track::PrMatch       == hist )  { return Rich::Rec::Track::Match;    }
    // seed types
    else if ( LHCb::Track::TsaTrack      == hist ||
              LHCb::Track::PatSeeding    == hist ||
              LHCb::Track::PrSeeding     == hist )  { return Rich::Rec::Track::Seed;     }
    // veloTT types
    else if ( LHCb::Track::PatVeloTT     == hist ||
              LHCb::Track::PrVeloUT      == hist )  { return Rich::Rec::Track::VeloTT;   }
    // Ks Tracks
    else if ( LHCb::Track::PatKShort     == hist ||
              LHCb::Track::PatDownstream == hist ||
              LHCb::Track::PrDownstream  == hist )  { return Rich::Rec::Track::KsTrack;  }
    // Velo only tracks (from any source)
    else if ( LHCb::Track::Velo  == track->type() ||
              LHCb::Track::VeloR == track->type() ) { return Rich::Rec::Track::Velo;     }
    // Muon ID
    else if ( LHCb::Track::MuonID        == hist )  { return Rich::Rec::Track::MuonID;   }
    // MC or ideal tracking
    else if ( LHCb::Track::TrackIdealPR  == hist )  { return Rich::Rec::Track::MCRichTrack; }
    // Imported HLT tracks
    else if ( LHCb::Track::HLTImportedTrack == hist )
    {
      switch( track->type() )
      {
      case LHCb::Track::Downstream:
        return Rich::Rec::Track::KsTrack;
      case LHCb::Track::VeloR:
      case LHCb::Track::Velo:
        return Rich::Rec::Track::Velo;
      case LHCb::Track::Upstream:
        return Rich::Rec::Track::VeloTT;
      case LHCb::Track::Ttrack:
        return Rich::Rec::Track::Seed;
      case LHCb::Track::Long:
      default:
        return Rich::Rec::Track::Forward;
      }
    }
    else
    { // Should not get here ...
      std::ostringstream mess;
      mess << "Unknown Track type : Track::History = " << track->history();
      throw GaudiException( mess.str(), "*Rich::Rec::Track::type*", StatusCode::FAILURE );
    }
  }

  // Should not get here either ...
  throw GaudiException( "Null Track pointer", "*Rich::Rec::Track::type*", StatusCode::FAILURE );
}

Rich::Rec::Track::Type Rich::Rec::Track::type( const LHCb::MCRichTrack * track )
{
  if ( UNLIKELY(!track) ) 
  {
    throw GaudiException( "Null Track pointer", "*Rich::Rec::Track::type*", StatusCode::FAILURE );
  }
  return Rich::Rec::Track::MCRichTrack;
}
