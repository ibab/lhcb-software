
//-----------------------------------------------------------------------------
/** @file RichTrackID.cpp
 *
 *  Implementation file for class : RichTrackID
 *
 *  CVS Log :-
 *  $Id: RichTrackID.cpp,v 1.11 2005-10-13 15:38:41 jonrob Exp $
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
  switch( parent ) {
  case Rich::TrackParent::TrgTrack:       return "TrgTrack";  // Place first for speed
  case Rich::TrackParent::Track:          return "Track";
  case Rich::TrackParent::TrStoredTrack:  return "TrStoredTrack";
  case Rich::TrackParent::MCParticle:     return "MCParticle";
  default:                                return "SHOULD NEVER SEE THIS";
  }
}

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::text( const Rich::Track::Type track )
{
  switch( track ) {
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
  default:                        return "SHOULD NEVER SEE THIS";
  }
}

Rich::Track::Type Rich::Track::type( const std::string & name )
{
  if      ( "trigger"  == name )   { return Rich::Track::Trigger;  }
  else if ( "forward"  == name )   { return Rich::Track::Forward;  }
  else if ( "match"    == name )   { return Rich::Track::Match;    } 
  else if ( "KsTrack"  == name )   { return Rich::Track::KsTrack;  }
  else if ( "veloTT"   == name )   { return Rich::Track::VeloTT;   }
  else if ( "seed"     == name )   { return Rich::Track::Seed;     }
  else if ( "follow"   == name )   { return Rich::Track::Follow;   }
  else if ( "velo"     == name )   { return Rich::Track::Velo;     }
  else if ( "unknown"  == name )   { return Rich::Track::Unknown;  }
  else if ( "unusable" == name )   { return Rich::Track::Unusable; }
  else 
  { // Should not get here ...
    throw GaudiException( "Unknown track string name '"+name+"'",
                          "*Rich::Track::type*", StatusCode::FAILURE );
    return Rich::Track::Unknown; 
  }
}

// Returns the enumerated type for a given TrStoredTrack
Rich::Track::Type Rich::Track::type( const ::Track * track )
{
  if ( track ) 
  {
    // track algorithm type
    const TrackKeys::History hist = (TrackKeys::History)track->history();
    // check all known track types (order according to abundance)
    // Convertered track types
    if      ( TrackKeys::CnvForward  == hist )  { return Rich::Track::Forward;  }
    else if ( TrackKeys::CnvMatch    == hist )  { return Rich::Track::Match;    }
    else if ( TrackKeys::CnvSeed     == hist )  { return Rich::Track::Seed;     }
    else if ( TrackKeys::CnvKsTrack  == hist )  { return Rich::Track::KsTrack;  }
    else if ( TrackKeys::CnvVeloTT   == hist )  { return Rich::Track::VeloTT;   }
    else if ( TrackKeys::CnvVelo     == hist )  { return Rich::Track::Velo;     }
    else if ( TrackKeys::CnvVeloBack == hist )  { return Rich::Track::Unusable; }
    else 
    { // Should not get here ...
      std::ostringstream mess;
      mess << "Unknown Track type : TrackKeys::History = " << track->history();
      throw GaudiException( mess.str(), "*Rich::Track::type*", StatusCode::FAILURE );
    }
  }

  // Should not get here either ...
  throw GaudiException( "Null Track pointer", "*Rich::Track::type*", StatusCode::FAILURE );
}

// Returns the enumerated type for a given TrStoredTrack
Rich::Track::Type Rich::Track::type( const TrStoredTrack * track )
{
  if ( track ) 
  {
    // check all known track types (order according to abundance)
    if      ( track->forward()  )     { return Rich::Track::Forward;  }
    else if ( track->match()    )     { return Rich::Track::Match;    }
    else if ( track->veloTT()   )     { return Rich::Track::VeloTT;   }
    else if ( track->seed()     )     { return Rich::Track::Seed;     }
    else if ( track->ksTrack()  )     { return Rich::Track::KsTrack;  }
    else if ( track->velo()     )     { return Rich::Track::Velo;     }
    else if ( track->follow()   )     { return Rich::Track::Follow;   }
    else if ( track->veloBack() )     { return Rich::Track::Unusable; }
    else 
    { // Should not get here ...
      std::ostringstream mess;
      mess << "Unknown TrStoredTrack type : history code = " << track->history();
      throw GaudiException( mess.str(), "*Rich::Track::type*", StatusCode::FAILURE );
    }
  }

  // Should not get here either ...
  throw GaudiException( "Null TrStoredTrack pointer",
                        "*Rich::Track::type*", StatusCode::FAILURE );
}

// Returns the enumerated type for a given TrStoredTrack
Rich::Track::Type Rich::Track::type( const TrgTrack * track )
{
  if ( track ) 
  {
    // Only one type at the moment
    return Rich::Track::Trigger;
  }

  // Should not get here 
  throw GaudiException( "Null TrgTrack pointer","*Rich::Track::type*", StatusCode::FAILURE );
}
