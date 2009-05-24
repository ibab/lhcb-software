
//-------------------------------------------------------------------------------
/** @file RichTrackSelEffMoni.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::TrackSelEff
 *
 *  CVS Log :-
 *  $Id: RichTrackSelEffMoni.cpp,v 1.2 2009-05-24 19:54:00 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   21/05/2009
 */
//-------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichTrackSelEffMoni.h"

// namespaces
using namespace Rich::Rec::MC;

//-------------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackSelEff );

// Standard constructor, initializes variables
TrackSelEff::TrackSelEff( const std::string& name,
                          ISvcLocator* pSvcLocator )
  : Rich::Rec::HistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth        ( NULL ),
    m_trSelector            ( NULL ),
    m_trTracksLocation      ( LHCb::TrackLocation::Default )
{
  // Context specific track locations
  if ( context() == "HLT" || context() == "Hlt" )
  {
    m_trTracksLocation = LHCb::TrackLocation::HltForward;
  }
  // JOs
  declareProperty( "NumberBins", m_nBins = 50 );
}

// Destructor
TrackSelEff::~TrackSelEff() { }

// Initialisation
StatusCode TrackSelEff::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "TrackSelector",    m_trSelector, this );
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );

  return sc;
}

// Main execution
StatusCode TrackSelEff::execute()
{
  using namespace Gaudi::Units;

  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Rich Histo ID
  const RichHistoID hid;

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );

  // Is MC available
  const bool mcTrackOK = m_richRecMCTruth->trackToMCPAvailable();

  // Get the raw input tracks
  const LHCb::Tracks * trTracks = get<LHCb::Tracks>( m_trTracksLocation );

  // Loop over the raw tracks
  for ( LHCb::Tracks::const_iterator iT = trTracks->begin();
        iT != trTracks->end(); ++iT )
  {
    // Track OK ?
    if ( !(*iT) ) { Warning("Null Track"); continue; }
    // Is the raw track selected for this monitor
    if ( !m_trSelector->trackSelected(*iT) ) continue;

    // Does this track have a RichRecTrack associated ?
    //const LHCb::RichRecTrack * rTrack = trackCreator()->newTrack(*iT);
    /** @todo figure out problem with track stats when using the above */
    const LHCb::RichRecTrack * rTrack = richTracks()->object((*iT)->key());
    const double richEff = ( rTrack ? 100.0 : 0.0 );

    // Ghost ?
    const std::string tkClass = ( !mcTrackOK ? "All/" :
                                  (NULL == m_richRecMCTruth->mcParticle(*iT)) ? "Ghost/" : "Real/" );

    // Efficiencies versus momentum
    profile1D( (*iT)->p(),  richEff, tkClass+"effVP",  "RICH Track Sel. Eff. V P",  1.00*GeV, 100.0*GeV, m_nBins );
    profile1D( (*iT)->pt(), richEff, tkClass+"effVPt", "RICH Track Sel. Eff. V Pt", 0.10*GeV, 10.0*GeV, m_nBins );

  }

  return StatusCode::SUCCESS;
}
