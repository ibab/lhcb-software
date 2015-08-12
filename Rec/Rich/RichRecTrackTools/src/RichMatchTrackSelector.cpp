//-----------------------------------------------------------------------------
/** @file RichMatchTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::MatchTrackSelector
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichMatchTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MatchTrackSelector )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MatchTrackSelector::MatchTrackSelector( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : BaseTrackSelector ( type, name , parent ),
    m_disabled        ( true )
{
  // Effectively disable these cuts, but keep the possibility to re-enable them later on.
  declareProperty( "MinMatchChi2", m_minMatchChi2 = boost::numeric::bounds<double>::lowest()  );
  declareProperty( "MaxMatchChi2", m_maxMatchChi2 = boost::numeric::bounds<double>::highest() );
}

//=============================================================================
// Destructor
//=============================================================================
MatchTrackSelector::~MatchTrackSelector() {}

void MatchTrackSelector::printSel( std::ostringstream & os ) const
{
  BaseTrackSelector::printSel(os);
  m_disabled = true;
  if ( m_minMatchChi2 > boost::numeric::bounds<double>::lowest() ||
       m_maxMatchChi2 < boost::numeric::bounds<double>::highest() )
  {
    m_disabled = false;
    os << boost::format( " : MatchChi2 = %|-4.2e|->%|-4.2e|" ) % m_minMatchChi2 % m_maxMatchChi2;
  }
}

// Test if the given Track is selected under the current criteria
bool
MatchTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  // Do base check
  const bool baseOK = BaseTrackSelector::trackSelected(track);
  if (!baseOK ) return false;

  // do we have any match track specific cuts to apply ?
  if ( !m_disabled )
  {

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> Apply Match track specific criteria" << endmsg;
    }

    // match chi2 cut
    LHCb::Track::ExtraInfo::const_iterator i = track->extraInfo().find( LHCb::Track::MatchChi2 );
    if ( i == track->extraInfo().end() )
    {
      Warning( "Match track does not have MatchChi2 info").ignore();
      return false;
    }
    if ( i->second < m_minMatchChi2 || i->second > m_maxMatchChi2 )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " -> Track failed match chi2 cut" << endmsg;
      return false;
    }

  }

  if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Track selected" << endmsg;
  return true;
}

bool
MatchTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{
  // get Track pointer.
  // RichRecTrack should always have an associated Seed Track
  const LHCb::Track * tT = dynamic_cast<const LHCb::Track *>(track->parentTrack());
  if ( !tT )
  {
    Warning( "Cannot use RichMatchTrackSelector on non-Track RichRecTracks" ).ignore();
    return false;
  }

  // delegate to Track method
  return this->trackSelected(tT);
}

//=============================================================================
