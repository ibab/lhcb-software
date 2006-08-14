
//-----------------------------------------------------------------------------
/** @file RichMatchTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichMatchTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichMatchTrackSelector.cpp,v 1.1 2006-08-14 10:05:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichMatchTrackSelector.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RichMatchTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichMatchTrackSelector::RichMatchTrackSelector( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : RichBaseTrackSelector ( type, name , parent )
{
  // interface
  declareInterface<Rich::IRichBaseTrackSelector>(this);
  // job options
  declareProperty( "MinMatchChi2", m_minMatchChi2 = 0 );
  declareProperty( "MaxMatchChi2", m_maxMatchChi2 = boost::numeric::bounds<double>::highest() );
}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichMatchTrackSelector::~RichMatchTrackSelector() {}

MsgStream & Rich::RichMatchTrackSelector::printSel( MsgStream & os ) const
{
  RichBaseTrackSelector::printSel(os);
  os << boost::format( " MatchChi2 = %|-4.2e|->%|-4.2e|" ) % m_minMatchChi2 % m_maxMatchChi2;
  return os;
}

// Test if the given Track is selected under the current criteria
bool
Rich::RichMatchTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  // Do base check
  const bool baseOK = RichBaseTrackSelector::trackSelected(track);
  if (!baseOK ) return false;

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " -> Apply Match track specific criteria" << endreq;
  }

  // match chi2 cut
  LHCb::Track::ExtraInfo::const_iterator i = track->extraInfo().find( LHCb::Track::MatchChi2 );
  if ( i == track->extraInfo().end() )
  {
    Warning( "Match track does not have MatchChi2 info");
    return false;
  }
  if ( i->second < m_minMatchChi2 || i->second > m_maxMatchChi2 )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track failed match chi2 cut" << endreq;
    return false;
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << " -> Track selected" << endreq;
  return true;
}

bool
Rich::RichMatchTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{
  // get Track pointer.
  // RichRecTrack should always have an associated Seed Track
  const LHCb::Track * tT = dynamic_cast<const LHCb::Track *>(track->parentTrack());
  if ( !tT )
  {
    Warning( "Cannot use RichMatchTrackSelector on non-Track RichRecTracks" );
    return false;
  }

  // delegate to Track method
  return this->trackSelected(tT);
}

//=============================================================================


