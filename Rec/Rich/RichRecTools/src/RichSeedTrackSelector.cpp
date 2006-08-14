
//-----------------------------------------------------------------------------
/** @file RichSeedTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichSeedTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichSeedTrackSelector.cpp,v 1.3 2006-08-14 10:05:54 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichSeedTrackSelector.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RichSeedTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichSeedTrackSelector::RichSeedTrackSelector( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : RichBaseTrackSelector ( type, name , parent )
{
  // interface
  declareInterface<Rich::IRichBaseTrackSelector>(this);
  // job options
  declareProperty( "MinSeedLikelihood", m_minSeedLL = -30 );
  declareProperty( "MaxSeedLikelihood", m_maxSeedLL = boost::numeric::bounds<double>::highest() );
}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichSeedTrackSelector::~RichSeedTrackSelector() {}

MsgStream & Rich::RichSeedTrackSelector::printSel( MsgStream & os ) const
{
  RichBaseTrackSelector::printSel(os);
  os << boost::format( " LL = %|-4.2e|->%|-4.2e|" ) % m_minSeedLL % m_maxSeedLL;
  return os;
}

// Test if the given Track is selected under the current criteria
bool
Rich::RichSeedTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  // Do base check
  const bool baseOK = RichBaseTrackSelector::trackSelected(track);
  if (!baseOK ) return false;

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " -> Apply Seed track specific criteria" << endreq;
  }

  // seed likelihood cuts
  LHCb::Track::ExtraInfo::const_iterator i = track->extraInfo().find( LHCb::Track::Likelihood );
  if ( i == track->extraInfo().end() )
  {
    Warning( "Seed track does not have Likelihood info");
    return false;
  }
  if ( i->second < m_minSeedLL || i->second > m_maxSeedLL )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << " -> Track failed seed-likelihood cut" << endreq;
    return false;
  }
  
  if ( msgLevel(MSG::DEBUG) ) debug() << " -> Track selected" << endreq;
  return true;
}

bool
Rich::RichSeedTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{
  // get Track pointer.
  // RichRecTrack should always have an associated Seed Track
  const LHCb::Track * tT = dynamic_cast<const LHCb::Track *>(track->parentTrack());
  if ( !tT ) 
  {
    Warning( "Cannot use RichSeedTrackSelector on non-Track RichRecTracks" );
    return false;
  }

  // delegate to Track method
  return this->trackSelected(tT);
}

//=============================================================================


