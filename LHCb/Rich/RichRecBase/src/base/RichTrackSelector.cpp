// $Id: RichTrackSelector.cpp,v 1.5 2004-07-02 14:17:03 jonrob Exp $
// Include files

// local
#include "RichRecBase/RichTrackSelector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTrackSelector
//
// 2003-06-20 : Chris Jones    Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Standard constructor
RichTrackSelector::RichTrackSelector() :
  m_trNames      (         ),
  m_uniqueTrOnly ( true    ),
  m_chargeSel    (  0      ),
  m_minP         ( Rich::Track::NTrTypes, -9999999 * GeV ),
  m_maxP         ( Rich::Track::NTrTypes,  9999999 * GeV )
{

  // Setup default track selection criteria
  // ======================================

  m_trNames.push_back( "unique"   ); // unique tracks only
  m_trNames.push_back( "positive" ); // positively charged tracks
  m_trNames.push_back( "negative" ); // negatively charged tracks
  // Add all known track types to selector
  for ( unsigned iTk = 0; iTk < Rich::Track::NTrTypes; ++iTk ) {
    const Rich::Track::Type type = static_cast<Rich::Track::Type>(iTk);
    m_trNames.push_back( Rich::text(type) );
    m_tkTypeSel[type] = true;
  }

}

bool RichTrackSelector::configureTrackTypes()
{

  // Initialise track selection criteria
  // ===================================

  // Check momentum cut vectors are the correct sizes
  if ( maxMomenta().size() != Rich::Track::NTrTypes ||
       minMomenta().size() != Rich::Track::NTrTypes ) {
    throw GaudiException( "Momentum cut vectors have wrong size !",
                          "*RichTrackSelector*", StatusCode::FAILURE );
  }

  // Set all flags off
  m_uniqueTrOnly = false;
  for ( TrackTypesSel::iterator iTk = m_tkTypeSel.begin();
        iTk != m_tkTypeSel.end(); ++iTk ) { *iTk = false; }
  m_chargeSel = 0;

  // loop over selection criteria
  for ( TrackNames::const_iterator iName = m_trNames.begin();
        iName != m_trNames.end(); ++iName ) {
    if      ( *iName == "unique"   ) { m_uniqueTrOnly = true;                    }
    else if ( *iName == "positive" ) { ++m_chargeSel;                            }
    else if ( *iName == "negative" ) { --m_chargeSel;                            }
    else if ( *iName == "forward"  ) { m_tkTypeSel[Rich::Track::Forward] = true; }
    else if ( *iName == "match"    ) { m_tkTypeSel[Rich::Track::Match]   = true; }
    else if ( *iName == "KsTrack"  ) { m_tkTypeSel[Rich::Track::KsTrack] = true; }
    else if ( *iName == "veloTT"   ) { m_tkTypeSel[Rich::Track::VeloTT]  = true; }
    else if ( *iName == "seed"     ) { m_tkTypeSel[Rich::Track::Seed]    = true; }
    else if ( *iName == "velo"     ) { m_tkTypeSel[Rich::Track::Velo]    = true; }
    else if ( *iName == "follow"   ) { m_tkTypeSel[Rich::Track::Follow]  = true; }
    else {
      throw GaudiException( "Unknown track selection criteria '"+(*iName)+"'",
                            "*RichTrackSelector*", StatusCode::FAILURE );
      return false;
    }
  }

  return true;
}
