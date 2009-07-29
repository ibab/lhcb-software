
//-------------------------------------------------------------------------------
/** @file RichTrackSelEffMoni.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::TrackSelEff
 *
 *  CVS Log :-
 *  $Id: RichTrackSelEffMoni.cpp,v 1.7 2009-07-29 12:35:07 jonrob Exp $
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
  prebookHistograms( "All/" );
  return StatusCode::SUCCESS;
}

void TrackSelEff::prebookHistograms( const std::string & tkClass )
{
  using namespace Gaudi::Units;

  richProfile1D( tkClass+"effVP",  "RICH Track Sel. Eff. V P",  1.00*GeV, 100.0*GeV, nBins1D() );
  richProfile1D( tkClass+"effVPt", "RICH Track Sel. Eff. V Pt", 0.10*GeV, 8.0*GeV,   nBins1D() );
  richProfile1D( tkClass+"effVChi2PDOF",
                 "RICH Track Sel. Eff. V Chi^2 / D.O.F.", 0, 10, nBins1D() );
  richProfile1D( tkClass+"effVLikelihood", "RICH Track Sel. Eff. V Likelihood", -100, 0, nBins1D() );
  richProfile1D( tkClass+"effVGhostProb", "RICH Track Sel. Eff. V Ghost Probability", 0.0, 1.0, nBins1D() );

  trackPlots( tkClass+"Selected/" );
  trackPlots( tkClass+"Rejected/" );
}

void TrackSelEff::trackPlots( const std::string & tag )
{
  using namespace Gaudi::Units;

  richHisto1D( tag+"P",  "Track Momentum",            0*GeV, 100*GeV, nBins1D() );
  richHisto1D( tag+"Pt", "Track Transverse Momentum", 0*GeV, 8*GeV,   nBins1D() );
  richHisto1D( tag+"Chi2PDOF", "Track Chi^2 / D.O.F.", 0, 10, nBins1D() );
  richHisto1D( tag+"Likelihood", "Track Likelihood", -100, 0, nBins1D() );
  richHisto1D( tag+"GhostProb", "Track Ghost Probability", 0.0, 1.0, nBins1D() );
}

// Main execution
StatusCode TrackSelEff::execute()
{
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
    const LHCb::MCParticle * mcP = ( mcTrackOK ?
                                     m_richRecMCTruth->mcParticle(*iT,m_mcAssocWeight) : NULL );
    const std::string tkClass = ( !mcTrackOK ? "All/" : mcP ? "Real/" : "Ghost/" );
    if ( mcTrackOK )
    {
      if ( mcP ) { ++nReal; } else { ++nGhost; }
      if ( rTrack ) { if ( mcP ) { ++nRealR; } else { ++nGhostR; } }
    }

    fillTrackPlots( *iT, rTrack, "All/" );
    if ( mcTrackOK )
      fillTrackPlots( *iT, rTrack, mcP ? "Real/" : "Ghost/" );

  } // loop over tracks

  if ( mcTrackOK )
  {
    plot1D( nReal,  "nRealTracks",  "# Real (MC Matched) Tracks / Event",      -0.5, 200.5, 201 );
    plot1D( nGhost, "nGhostTracks", "# Ghost (Not MC Matched) Tracks / Event", -0.5, 200.5, 201 );
    plot1D( nRealR,  "nRealRichTracks",  "# Real (MC Matched) Rich Tracks / Event",      -0.5, 200.5, 201 );
    plot1D( nGhostR, "nGhostRichTracks", "# Ghost (Not MC Matched) Rich Tracks / Event", -0.5, 200.5, 201 );
  }

  return StatusCode::SUCCESS;
}

void TrackSelEff::fillTrackPlots( const LHCb::Track * track,
                                  const LHCb::RichRecTrack * rTrack,
                                  const std::string & tkClass )
{
  // Make sure (MC) histos are booked
  if ( tkClass != "All/" ) prebookHistograms(tkClass);

  // Efficiencies plots
  const double richEff = ( rTrack != NULL ? 100.0 : 0.0 );
  richProfile1D( tkClass+"effVP"  )         -> fill ( track->p(),  richEff );
  richProfile1D( tkClass+"effVPt" )         -> fill ( track->pt(), richEff );
  richProfile1D( tkClass+"effVChi2PDOF" )   -> fill ( track->chi2PerDoF(), richEff );
  richProfile1D( tkClass+"effVLikelihood" ) -> fill ( track->info(LHCb::Track::Likelihood,0), richEff );
  richProfile1D( tkClass+"effVGhostProb" )  -> fill ( track->info(LHCb::Track::GhostProbability,999999), richEff );

  // plot selection variables
  const std::string tag = ( rTrack != NULL ? tkClass+"Selected/" : tkClass+"Rejected/" );
  richHisto1D( tag+"P"  ) -> fill ( track->p()  );
  richHisto1D( tag+"Pt" ) -> fill ( track->pt() );
  richHisto1D( tag+"Chi2PDOF" ) -> fill ( track->chi2PerDoF() );
  richHisto1D( tag+"Likelihood" ) -> fill ( track->info(LHCb::Track::Likelihood,0) );
  richHisto1D( tag+"GhostProb"  ) -> fill ( track->info(LHCb::Track::GhostProbability,-1) );
}
