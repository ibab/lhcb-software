// $Id: RichTrackSelector.cpp,v 1.3 2004-04-19 22:59:46 jonesc Exp $
// Include files

// local
#include "RichRecBase/RichTrackSelector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTrackSelector
//
// 2003-06-20 : Chris Jones
//-----------------------------------------------------------------------------

// Standard constructor
RichTrackSelector::RichTrackSelector() :
  m_trNames      (      ),
  m_trBits       ( 0    ),
  m_uniqueTrOnly ( true ) {

  // By default add all known track types to selector
  m_trNames.push_back( "unique"  );
  for ( int iTk = 0; iTk < Rich::Track::NTrTypes; ++iTk ) {
    m_trNames.push_back( Rich::text(static_cast<Rich::Track::Type>(iTk)) );
  }

}

bool RichTrackSelector::configureTrackTypes() {

  // Initialise track bit selection
  m_trBits = 0;
  m_uniqueTrOnly = false;
  for ( TrackNames::const_iterator iName = m_trNames.begin();
        iName != m_trNames.end(); ++iName ) {
    if      ( *iName == "unique"   ) { m_uniqueTrOnly = true; }
    else if ( *iName == "velo"     ) { m_trBits += 2;   }
    else if ( *iName == "seed"     ) { m_trBits += 4;   }
    else if ( *iName == "match"    ) { m_trBits += 8;   }
    else if ( *iName == "forward"  ) { m_trBits += 16;  }
    else if ( *iName == "follow"   ) { m_trBits += 32;  }
    else if ( *iName == "veloTT"   ) { m_trBits += 64;  }
    else if ( *iName == "veloBack" ) { m_trBits += 128; }
    else if ( *iName == "KsTrack"  ) { m_trBits += 256; }
    else {
      throw GaudiException( "Unknown track type '"+(*iName)+"'",
                            "RichTrackSelector", StatusCode::FAILURE );
      return false;
    }
  }

  return true;
}

const std::string RichTrackSelector::selectedTracksAsString() const 
{ 
  std::vector<std::string>::const_iterator iT = selectedTrackTypes().begin();
  std::string names = *iT; ++iT;
  for ( ; iT != selectedTrackTypes().end(); ++iT ) { names += " " + *iT; }
  return names;
}
