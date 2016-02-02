
//-----------------------------------------------------------------------------
/** @file RichMCTrueTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichMCTrueTrackSelector
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichMCTrueTrackSelector.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTrueTrackSelector::MCTrueTrackSelector( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : TrackSelectorBase ( type, name, parent )
{
  declareProperty( "MinMCAssocWeight", m_weight = Rich::Rec::MC::DefMinWeightTrToMCP );
}

//=============================================================================
// Destructor
//=============================================================================
MCTrueTrackSelector::~MCTrueTrackSelector() { }

StatusCode MCTrueTrackSelector::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = TrackSelectorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  info() << "Selecting MCParticle associated tracks with weight > " 
         << m_weight << endmsg;

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );

  return sc;
}

// Test if the given Track is selected
bool MCTrueTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  const auto * mcp = m_richRecMCTruth->mcParticle(track,m_weight);
  return ( mcp ? TrackSelectorBase::trackSelected(track) : false );
}

// Test it the given RichRecTrack is selected
bool MCTrueTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{
  const auto * mcp = m_richRecMCTruth->mcParticle(track,m_weight);
  return ( mcp ? TrackSelectorBase::trackSelected(track) : false );
}

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCTrueTrackSelector )

//-----------------------------------------------------------------------------
