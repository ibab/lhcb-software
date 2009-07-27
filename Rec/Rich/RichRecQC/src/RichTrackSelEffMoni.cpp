
//-------------------------------------------------------------------------------
/** @file RichTrackSelEffMoni.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::TrackSelEff
 *
 *  CVS Log :-
 *  $Id: RichTrackSelEffMoni.cpp,v 1.6 2009-07-27 20:35:28 jonrob Exp $
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
  declareProperty( "MCParticleAssocWeight", m_mcAssocWeight = 0.75 );
  // reset defaults in base class
  setProperty( "NBins1DHistos", 50 );
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

StatusCode TrackSelEff::prebookHistograms()
{
  // Book (MC free) histos which must be ready after initialisation
  // for online monitoring



  return StatusCode::SUCCESS;
}

// Main execution
StatusCode TrackSelEff::execute()
{
  using namespace Gaudi::Units;

  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Get the raw input tracks
  if ( !exist<LHCb::Tracks>(m_trTracksLocation) )
  {
    return Warning("No Tracks at "+m_trTracksLocation,StatusCode::SUCCESS);
  }
  const LHCb::Tracks * trTracks = get<LHCb::Tracks>( m_trTracksLocation );

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );

  // Is MC available
  const bool mcTrackOK = m_richRecMCTruth->trackToMCPAvailable();

  // Loop over the raw tracks
  unsigned int nGhost(0), nReal(0), nGhostR(0), nRealR(0);
  for ( LHCb::Tracks::const_iterator iT = trTracks->begin();
        iT != trTracks->end(); ++iT )
  {
    // Track OK ?
    if ( !(*iT) ) { Warning("Null Track"); continue; }
    // Is the raw track selected for this monitor
    if ( !m_trSelector->trackSelected(*iT) ) continue;

    // Does this track have a RichRecTrack associated ?
    const LHCb::RichRecTrack * rTrack = richTracks()->object((*iT)->key());

    // Ghost ?
    const LHCb::MCParticle * mcP = m_richRecMCTruth->mcParticle(*iT,m_mcAssocWeight);
    const std::string tkClass = ( !mcTrackOK ? "All/" : mcP ? "Real/" : "Ghost/" );
    if ( mcP ) { ++nReal; } else { ++nGhost; }
    if ( rTrack ) { if ( mcP ) { ++nRealR; } else { ++nGhostR; } }

    // Efficiencies plots
    const double richEff = ( rTrack != NULL ? 100.0 : 0.0 );
    profile1D( (*iT)->p(),  richEff, tkClass+"effVP",  "RICH Track Sel. Eff. V P",  1.00*GeV, 100.0*GeV, nBins1D() );
    profile1D( (*iT)->pt(), richEff, tkClass+"effVPt", "RICH Track Sel. Eff. V Pt", 0.10*GeV, 8.0*GeV,   nBins1D() );
    profile1D( (*iT)->chi2PerDoF(), richEff, tkClass+"effVChi2PDOF",
               "RICH Track Sel. Eff. V Chi^2 / D.O.F.", 0, 10, nBins1D() );
    profile1D( (*iT)->info(LHCb::Track::Likelihood,0), richEff,
               tkClass+"effVLikelihood", "RICH Track Sel. Eff. V Likelihood", -100, 0, nBins1D() );
    profile1D( (*iT)->info(LHCb::Track::GhostProbability,999999), richEff,
               tkClass+"effVGhostProb", "RICH Track Sel. Eff. V Ghost Probability", 0.0, 1.0, nBins1D() );

    // plot selection variables
    const std::string tag = ( rTrack != NULL ? tkClass+"Selected/" : tkClass+"Rejected/" );
    plot1D( (*iT)->p(),  tag+"P",  "Track Momentum",            0*GeV, 100*GeV, nBins1D() );
    plot1D( (*iT)->pt(), tag+"Pt", "Track Transverse Momentum", 0*GeV, 8*GeV,   nBins1D() );
    plot1D( (*iT)->chi2PerDoF(), tag+"Chi2PDOF", "Track Chi^2 / D.O.F.", 0, 10, nBins1D() );
    plot1D( (*iT)->info(LHCb::Track::Likelihood,0),
            tag+"Likelihood", "Track Likelihood", -100, 0, nBins1D() );
    plot1D( (*iT)->info(LHCb::Track::GhostProbability,-1),
            tag+"GhostProb", "Track Ghost Probability", 0.0, 1.0, nBins1D() );

  } // loop over tracks

  plot1D( nReal,  "nRealTracks",  "# Real (MC Matched) Tracks / Event",      -0.5, 200.5, 201 );
  plot1D( nGhost, "nGhostTracks", "# Ghost (Not MC Matched) Tracks / Event", -0.5, 200.5, 201 );
  plot1D( nRealR,  "nRealRichTracks",  "# Real (MC Matched) Rich Tracks / Event",      -0.5, 200.5, 201 );
  plot1D( nGhostR, "nGhostRichTracks", "# Ghost (Not MC Matched) Rich Tracks / Event", -0.5, 200.5, 201 );

  return StatusCode::SUCCESS;
}

