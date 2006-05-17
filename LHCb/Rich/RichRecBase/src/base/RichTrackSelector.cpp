
//-----------------------------------------------------------------------------
/** @file RichTrackSelector.cpp
 *
 *  Implementation file for class : RichTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichTrackSelector.cpp,v 1.12 2006-05-17 15:59:28 cattanem Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-06-20
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichTrackSelector.h"

// Boost
#include "boost/lexical_cast.hpp"

// namespaces
using namespace LHCb;

// Standard constructor
RichTrackSelector::RichTrackSelector() :
  m_trNames      (         ),
  m_uniqueTrOnly ( true    ),
  m_chargeSel    (  0      ),
  m_minP         ( Rich::Track::NTrTypes,        0 * Gaudi::Units::GeV ),
  m_maxP         ( Rich::Track::NTrTypes,  9999999 * Gaudi::Units::GeV )
{

  // Setup default track selection criteria
  // ======================================

  m_trNames.push_back( "unique"   ); // unique tracks only
  m_trNames.push_back( "positive" ); // positively charged tracks
  m_trNames.push_back( "negative" ); // negatively charged tracks
  // Add all known reconstructed track types to selector
  for ( unsigned iTk = 0; iTk < Rich::Track::NTrTypes; ++iTk ) 
  {
    const Rich::Track::Type type = static_cast<Rich::Track::Type>(iTk);
    // by default do not add MC types ...
    if ( type != Rich::Track::MCRichTrack )
    {
      m_trNames.push_back( Rich::text(type) );
      m_tkTypeSel[type] = true;
    }
    else
    {     
      m_tkTypeSel[type] = false;
    }
  }

}

bool RichTrackSelector::configureTrackTypes( MsgStream & stream )
{

  // Initialise track selection criteria
  // ===================================

  // Configure momentum cuts
  if ( !configureMomentumCuts(stream) ) return false;

  // Set all flags off
  m_uniqueTrOnly = false;
  for ( TrackTypesSel::iterator iTk = m_tkTypeSel.begin();
        iTk != m_tkTypeSel.end(); ++iTk ) { *iTk = false; }
  m_chargeSel = 0;

  // loop over selection criteria
  for ( TrackNames::const_iterator iName = m_trNames.begin();
        iName != m_trNames.end(); ++iName ) 
  {
    stream << MSG::DEBUG << "Processing selection '" << *iName << "'" << endreq;
    if      ( *iName == "unique"      ) { m_uniqueTrOnly = true;                        }
    else if ( *iName == "positive"    ) { ++m_chargeSel;                                }
    else if ( *iName == "negative"    ) { --m_chargeSel;                                }
    else if ( *iName == "forward"     ) { m_tkTypeSel[Rich::Track::Forward] = true;     }
    else if ( *iName == "match"       ) { m_tkTypeSel[Rich::Track::Match]   = true;     }
    else if ( *iName == "KsTrack"     ) { m_tkTypeSel[Rich::Track::KsTrack] = true;     }
    else if ( *iName == "veloTT"      ) { m_tkTypeSel[Rich::Track::VeloTT]  = true;     }
    else if ( *iName == "seed"        ) { m_tkTypeSel[Rich::Track::Seed]    = true;     }
    else if ( *iName == "velo"        ) { m_tkTypeSel[Rich::Track::Velo]    = true;     }
    else if ( *iName == "follow"      ) { m_tkTypeSel[Rich::Track::Follow]  = true;     }
    else if ( *iName == "trigger"     ) { m_tkTypeSel[Rich::Track::Trigger] = true;     }
    else if ( *iName == "MCRichTrack" ) { m_tkTypeSel[Rich::Track::MCRichTrack] = true; }
    else 
    {
      throw GaudiException( "Unknown track selection criteria '"+(*iName)+"'",
                            "*RichTrackSelector*", StatusCode::FAILURE );
      return false;
    }
  }

  return true;
}

bool RichTrackSelector::configureMomentumCuts( MsgStream & stream )
{

  // Loop over cut data and form the numerical cut values
  for ( MomentumCutData::const_iterator iData = m_pCutData.begin();
        iData != m_pCutData.end(); ++iData ) 
  {

    bool OK = false;
    const int f1 = (*iData).find_first_of( "/" );
    if ( f1 > 0 ) 
    {

      // The string before the first slash is expected to be the track type name
      const Rich::Track::Type tracktype = Rich::Track::type( (*iData).substr(0,f1) );

      // The remaining is the cut data in the form "min/max"
      const std::string cuts = (*iData).substr(f1+1);
      const int f2 = cuts.find_first_of( "/" );
      if ( f2 > 0 ) 
      {
        try 
        {
          m_minP[tracktype] = boost::lexical_cast<double>(cuts.substr(0,f2));
          m_maxP[tracktype] = boost::lexical_cast<double>(cuts.substr(f2+1));
          OK = true;
        }
        catch ( const boost::bad_lexical_cast & excp ) 
        { 
          OK = false; 
          stream << MSG::ERROR 
                 << "configureMomentumCuts:: Error '" << excp.what() 
                 << "' in data string : " << *iData << endreq;
        }
      
      stream << MSG::DEBUG 
             << "Set momentum cuts for " << tracktype << " " << m_minP[tracktype] 
             << "->" << m_maxP[tracktype] << endreq;
      }
      else
      {
        stream << MSG::ERROR 
               << "configureMomentumCuts:: Track type " 
               << tracktype << " has no momentum data" << endreq;
      }

    }

    // Something went wrong processing the cut data
    if ( !OK ) 
    {
      throw GaudiException( "Badly formed selection data : '" + *iData + "'",
                            "*RichTrackSelector*", StatusCode::FAILURE );
      return false;
    }

  }

  return true;
}

void RichTrackSelector::printTrackSelection( MsgStream & stream ) const
{
  stream << "Track Selection :-" << endreq;
  for ( std::vector<std::string>::const_iterator iT = selectedTrackTypes().begin();
        iT != selectedTrackTypes().end(); ++iT ) 
  { 
    // should perhaps find a better way that avoids this try/catch
    if ( *iT != "unique" && *iT != "positive" && *iT != "negative" )
    {
      // get track type
      Rich::Track::Type type = Rich::Track::Unknown;
      try { type = Rich::Track::type( *iT ); }
      catch ( const GaudiException & expt ) 
      { 
        std::cout << "RichTrackSelector::printTrackSelection:: Exception '" 
                  << expt.what() << "'" << std::endl; 
        continue; 
      }
      // Construct name string
      std::string name = ( m_uniqueTrOnly ? "Unique "+*iT : *iT );
      name.resize(20,' ');
      // print name and momentum cuts
      stream << "   " << name << " : Sel=" << m_tkTypeSel[type] << " : "
             << minMomentum(type) << " -> " << maxMomentum(type) << " GeV" << endreq; 
    }
  }
}

const std::string RichTrackSelector::selectedTracksAsString() const
{
  std::vector<std::string>::const_iterator iT = selectedTrackTypes().begin();
  std::string names = *iT; 
  ++iT;
  for ( ; iT != selectedTrackTypes().end(); ++iT ) { names += (" " + *iT); }
  return names;
}
