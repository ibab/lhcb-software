// $Id: RichTrackSelector.cpp,v 1.1 2003-10-13 16:10:54 jonrob Exp $
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

  m_trNames.push_back( "unique" );
  m_trNames.push_back( "seed" );
  m_trNames.push_back( "match" );
  m_trNames.push_back( "forward" );
  m_trNames.push_back( "upstream" );
  m_trNames.push_back( "veloTT" );

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
    else if ( *iName == "upstream" ) { m_trBits += 32;  }
    else if ( *iName == "veloTT"   ) { m_trBits += 64;  }
    else if ( *iName == "veloBack" ) { m_trBits += 128; }
    else {
      std::cout << "ERROR : Unknown track type " << *iName << std::endl;
      return false;
    }
  }

  return true;
}
