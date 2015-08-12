
//-------------------------------------------------------------------------------
/** @file RichTrackSelEffMoni.cpp
 *
 *  Implementation file for RICH reconstruction monitoring algorithm :
 *  Rich::Rec::MC::TrackSelEff
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   21/05/2009
 */
//-------------------------------------------------------------------------------

// local
#include "RichTrackSelEffMoni.h"

// namespaces
using namespace Rich::Rec::MC;

//-------------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackSelEff )

// Standard constructor, initializes variables
  TrackSelEff::TrackSelEff( const std::string& name,
                            ISvcLocator* pSvcLocator )
    : Rich::Rec::HistoAlgBase ( name, pSvcLocator ),
      m_richRecMCTruth        ( NULL ),
      m_trSelector            ( NULL )
{
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
  // global quantities
  richHisto1D( HID("nTracks"),     "# Tracks / Event", -0.5, 200.5, 201 );
  richHisto1D( HID("nRichTracks"), "# Rich Tracks / Event", -0.5, 200.5, 201 );
  // track variable plots
  prebookHistograms( "All/" );
  // return
  return StatusCode::SUCCESS;
}

void TrackSelEff::prebookHistograms( const std::string & tkClass )
{
  using namespace Gaudi::Units;

  richProfile1D( tkClass+"effVP",  "RICH Track Sel. Eff. V P",  1.00*GeV, 100.0*GeV, nBins1D() );
  richProfile1D( tkClass+"effVPt", "RICH Track Sel. Eff. V Pt", 0.10*GeV, 8.0*GeV,   nBins1D() );
  richProfile1D( tkClass+"effVChi2PDOF",
                 "RICH Track Sel. Eff. V Chi^2 / D.O.F.", 0, 3, nBins1D() );
  richProfile1D( tkClass+"effVLikelihood", "RICH Track Sel. Eff. V Likelihood", -100, 0, nBins1D() );
  richProfile1D( tkClass+"effVGhostProb", "RICH Track Sel. Eff. V Ghost Probability", 0.0, 1.0, nBins1D() );
  richProfile1D( tkClass+"effVCloneDist", "RICH Track Sel. Eff. V Clone Distance", 0.0, 6e3, nBins1D() );

  trackPlots( tkClass+"Selected/" );
  trackPlots( tkClass+"Rejected/" );
}

void TrackSelEff::trackPlots( const std::string & tag )
{
  using namespace Gaudi::Units;

  richHisto1D( tag+"P",  "Track Momentum",            0*GeV, 100*GeV, nBins1D() );
  richHisto1D( tag+"Pt", "Track Transverse Momentum", 0*GeV, 8*GeV,   nBins1D() );
  richHisto1D( tag+"Chi2PDOF", "Track Chi^2 / D.O.F.", 0, 3, nBins1D() );
  richHisto1D( tag+"Likelihood", "Track Likelihood", -100, 0, nBins1D() );
  richHisto1D( tag+"GhostProb", "Track Ghost Probability", 0.0, 1.0, nBins1D() );
  richHisto1D( tag+"CloneDist", "Track Clone Distance", 0.0, 6e3, nBins1D() );
}

// Main execution
StatusCode TrackSelEff::execute()
{
  // Event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );

  // Pre-loop over RICH tracks to get the raw Track containers
  std::set<std::string> trackLocs;
  for ( LHCb::RichRecTracks::const_iterator iT = richTracks()->begin();
        iT != richTracks()->end(); ++iT )
  {
    // get location of associated track container
    const std::string contLoc = objectLocation( (*iT)->parentTrack()->parent() );
    if ( !contLoc.empty() ) trackLocs.insert(contLoc);
  }

  // Is MC available
  const bool mcTrackOK = m_richRecMCTruth->trackToMCPAvailable();

  // Loop over raw track locations
  for ( std::set<std::string>::const_iterator iLoc = trackLocs.begin();
        iLoc != trackLocs.end(); ++iLoc )
  {
    // Load these tracks
    const LHCb::Tracks * trTracks = get<LHCb::Tracks>( *iLoc );

    // Loop over the raw tracks
    unsigned int nGhost(0), nReal(0), nGhostR(0), nRealR(0), nTotal(0), nTotalR(0);
    for ( LHCb::Tracks::const_iterator iT = trTracks->begin();
          iT != trTracks->end(); ++iT )
    {
      // Track OK ?
      if ( !(*iT) ) { Warning("Null Track"); continue; }
      // Is the raw track selected for this monitor
      if ( !m_trSelector->trackSelected(*iT) ) continue;
      // count all raw selected tracks
      ++nTotal;

      // Does this track have a RichRecTrack associated ?
      const LHCb::RichRecTrack * rTrack = getRichTrack(*iT);
      if ( rTrack ) ++nTotalR;

      // Fill real data plots
      fillTrackPlots( *iT, rTrack, "All/" );

      // MC only plots
      const LHCb::MCParticle * mcP = ( mcTrackOK ?
                                       m_richRecMCTruth->mcParticle(*iT,m_mcAssocWeight) : NULL );
      if ( mcTrackOK )
      {
        if ( mcP ) { ++nReal; } else { ++nGhost; }
        if ( rTrack ) { if ( mcP ) { ++nRealR; } else { ++nGhostR; } }
        fillTrackPlots( *iT, rTrack, mcP ? "Real/" : "Ghost/" );
      }

    } // loop over tracks

    richHisto1D( HID("nTracks")     ) -> fill( nTotal  );
    richHisto1D( HID("nRichTracks") ) -> fill( nTotalR );
    if ( mcTrackOK )
    {
      plot1D( nReal,   "nRealTracks",      "# Real (MC Matched) Tracks / Event",           -0.5, 200.5, 201 );
      plot1D( nGhost,  "nGhostTracks",     "# Ghost (Not MC Matched) Tracks / Event",      -0.5, 200.5, 201 );
      plot1D( nRealR,  "nRealRichTracks",  "# Real (MC Matched) Rich Tracks / Event",      -0.5, 200.5, 201 );
      plot1D( nGhostR, "nGhostRichTracks", "# Ghost (Not MC Matched) Rich Tracks / Event", -0.5, 200.5, 201 );
    }

  } // loop over track locations

  return StatusCode::SUCCESS;
}

void TrackSelEff::fillTrackPlots( const LHCb::Track * track,
                                  const LHCb::RichRecTrack * rTrack,
                                  const std::string & tkClass )
{
  // Make sure (MC) histos are booked
  if ( tkClass != "All/" ) prebookHistograms(tkClass);

  // cache clone dist
  const double cloneDist = track->info(LHCb::Track::CloneDist,5.5e3);

  // Efficiencies plots
  const double richEff = ( rTrack != NULL ? 100.0 : 0.0 );
  richProfile1D( tkClass+"effVP"          ) -> fill ( track->p(),  richEff );
  richProfile1D( tkClass+"effVPt"         ) -> fill ( track->pt(), richEff );
  richProfile1D( tkClass+"effVChi2PDOF"   ) -> fill ( track->chi2PerDoF(), richEff );
  richProfile1D( tkClass+"effVLikelihood" ) -> fill ( track->likelihood(), richEff );
  richProfile1D( tkClass+"effVGhostProb"  ) -> fill ( track->ghostProbability(), richEff );
  richProfile1D( tkClass+"effVCloneDist"  ) -> fill ( cloneDist, richEff );

  // plot selection variables
  const std::string tag = ( rTrack != NULL ? tkClass+"Selected/" : tkClass+"Rejected/" );
  richHisto1D( tag+"P"          ) -> fill ( track->p()  );
  richHisto1D( tag+"Pt"         ) -> fill ( track->pt() );
  richHisto1D( tag+"Chi2PDOF"   ) -> fill ( track->chi2PerDoF() );
  richHisto1D( tag+"Likelihood" ) -> fill ( track->likelihood() );
  richHisto1D( tag+"GhostProb"  ) -> fill ( track->ghostProbability() );
  richHisto1D( tag+"CloneDist"  ) -> fill ( cloneDist );
}

const LHCb::RichRecTrack * TrackSelEff::getRichTrack( const LHCb::Track * track ) const
{
  const LHCb::RichRecTrack * rT = NULL;

  // Check Track pointer is OK
  if ( track )
  {

    // First try using same track key convention
    rT = richTracks()->object(track->key());
    if ( !rT || dynamic_cast<const LHCb::Track*>(rT->parentTrack()) != track )
    {

      // Key convention failed, so try direct search
      rT = NULL;
      for ( LHCb::RichRecTracks::const_iterator iRT = richTracks()->begin();
            iRT != richTracks()->end(); ++iRT )
      {
        if ( dynamic_cast<const LHCb::Track*>((*iRT)->parentTrack()) == track )
        {
          rT = *iRT;
          break;
        }
      }

    } // key search failed

  } // track OK

  // return the RichRecTrack pointer
  return rT;
}
