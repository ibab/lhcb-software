
//-----------------------------------------------------------------------------
/** @file RichSeedTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::SeedTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichSeedTrackSelector.cpp,v 1.3 2009-05-21 17:29:16 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichSeedTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SeedTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SeedTrackSelector::SeedTrackSelector( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : BaseTrackSelector ( type, name , parent )
{
  // interface
  declareInterface<IBaseTrackSelector>(this);
  // job options
  //declareProperty( "MinSeedLikelihood", m_minSeedLL = -15 );
  declareProperty( "MinSeedLikelihood", m_minSeedLL = boost::numeric::bounds<double>::lowest()  );
  declareProperty( "MaxSeedLikelihood", m_maxSeedLL = boost::numeric::bounds<double>::highest() );
}

//=============================================================================
// Destructor
//=============================================================================
SeedTrackSelector::~SeedTrackSelector() {}

MsgStream & SeedTrackSelector::printSel( MsgStream & os ) const
{
  BaseTrackSelector::printSel(os);
  if ( m_minSeedLL > boost::numeric::bounds<double>::lowest() ||
       m_maxSeedLL < boost::numeric::bounds<double>::highest() )
  {
    os << boost::format( " : LL = %|-4.2e|->%|-4.2e|" ) % m_minSeedLL % m_maxSeedLL;
  }
  return os;
}

// Test if the given Track is selected under the current criteria
bool
SeedTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  // Do base check
  const bool baseOK = BaseTrackSelector::trackSelected(track);
  if (!baseOK ) return false;

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " -> Apply Seed track specific criteria" << endmsg;
  }

  // seed likelihood cuts
  LHCb::Track::ExtraInfo::const_iterator i = track->extraInfo().find( LHCb::Track::TsaLikelihood );
  if ( i == track->extraInfo().end() )
  {
    Warning( "Seed track does not have Likelihood info" ).ignore();
    return false;
  }
  if ( i->second < m_minSeedLL || i->second > m_maxSeedLL )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> Track failed seed-likelihood cut" << endmsg;
    return false;
  }

  if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> Track selected" << endmsg;
  return true;
}

bool
SeedTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{
  // get Track pointer.
  // RichRecTrack should always have an associated Seed Track
  const LHCb::Track * tT = dynamic_cast<const LHCb::Track *>(track->parentTrack());
  if ( !tT )
  {
    Warning( "Cannot use RichSeedTrackSelector on non-Track RichRecTracks" ).ignore();
    return false;
  }

  // delegate to Track method
  return this->trackSelected(tT);
}

//=============================================================================


