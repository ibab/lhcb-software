
//-----------------------------------------------------------------------------
/** @file RichMCTrueTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichMCTrueTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichMCTrueTrackSelector.cpp,v 1.1 2006-08-28 11:13:28 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichMCTrueTrackSelector.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RichMCTrueTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichMCTrueTrackSelector::RichMCTrueTrackSelector( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
  : RichTrackSelectorBase ( type, name, parent ) 
{
  declareProperty( "MinMCAssocWeight", m_weight = 0.9 );
}

//=============================================================================
// Destructor
//=============================================================================
Rich::RichMCTrueTrackSelector::~RichMCTrueTrackSelector() { }

StatusCode Rich::RichMCTrueTrackSelector::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichTrackSelectorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  info() << "Selecting MCParticle associated tracks with weight > " << m_weight << endreq;

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );

  return sc;
}

// Test if the given Track is selected
bool Rich::RichMCTrueTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  const LHCb::MCParticle * mcp = m_richRecMCTruth->mcParticle(track,m_weight);
  return ( NULL != mcp ? RichTrackSelectorBase::trackSelected(track) : false );
}

// Test it the given RichRecTrack is selected
bool Rich::RichMCTrueTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{
  const LHCb::MCParticle * mcp = m_richRecMCTruth->mcParticle(track,m_weight);
  return ( NULL != mcp ? RichTrackSelectorBase::trackSelected(track) : false );
}
