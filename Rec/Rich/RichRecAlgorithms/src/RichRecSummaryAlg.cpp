
//--------------------------------------------------------------------------
/** @file RichRecSummaryAlg.cpp
 *
 *  Implementation file for algorithm class : RichRecSummaryAlg
 *
 *  CVS Log :-
 *  $Id: RichRecSummaryAlg.cpp,v 1.5 2006-12-19 09:46:30 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecSummaryAlg.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespaces
using namespace LHCb;

//--------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RichRecSummaryAlg );

// Standard constructor, initializes variables
RichRecSummaryAlg::RichRecSummaryAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name, pSvcLocator ),
    m_ckAngle      ( NULL ),
    m_ckAngleRes   ( NULL ),
    m_tkSignal     ( NULL ),
    m_trSelector   ( NULL ),
    m_summaryLoc   ( RichSummaryTrackLocation::Default ),
    m_nSigma       ( Rich::NRadiatorTypes, 1 )
{
  if      ( context() == "Offline" )
  {
    m_summaryLoc = RichSummaryTrackLocation::Offline;
  }
  else if ( context() == "HLT" )
  {
    m_summaryLoc = RichSummaryTrackLocation::HLT;
  }
  // job opts
  declareProperty( "SummaryLocation", m_summaryLoc );
  declareProperty( "PhotonNSigma",    m_nSigma );
}

// Destructor
RichRecSummaryAlg::~RichRecSummaryAlg() {}

// Initialize
StatusCode RichRecSummaryAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() )
  { return Error( "Failed to initialize base class", sc ); }

  // get tools
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichCherenkovResolution", m_ckAngleRes  );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal    );
  acquireTool( "TrackSelector",           m_trSelector, this );

  info() << "Will select photons within (aero/R1Gas/R2Gas) " << m_nSigma
         << " sigma of any mass hypothesis" << endreq;

  return sc;
}

StatusCode RichRecSummaryAlg::execute()
{

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );
  // make sure RichrecPixels are ready
  if ( pixelCreator()->newPixels().isFailure() )
    return Error( "Problem creating RichRecPixels" );
  // make sure photons are available
  if ( photonCreator()->reconstructPhotons().isFailure() )
    return Error( "Problem creating RichRecPhotons" );

  // Create a new container for RICH reconstruction summary Tracks
  RichSummaryTracks * sumTracks = new RichSummaryTracks();
  put( sumTracks, m_summaryLoc );

  // All OK, load the RichRecTracks
  for ( RichRecTracks::iterator track = richTracks()->begin();
        track != richTracks()->end(); ++track )
  {

    // apply track selection
    if ( !m_trSelector->trackSelected(*track) ) continue;

    // get the reco track
    const Track * trtrack = dynamic_cast<const Track *>((*track)->parentTrack());
    if ( !trtrack ) 
    {
      Warning( "Input track type is not Track -> RichRecTrack skipped" );
      continue;
    }

    // Make a new summary track
    RichSummaryTrack * sumTrack = new RichSummaryTrack();
    // give to Gaudi
    sumTracks->insert( sumTrack, (*track)->key() );

    // store track reference
    sumTrack->setTrack( trtrack );

    // vector of summary segments
    RichSummaryRadSegment::Vector sumSegs;

    // loop over radiator segments
    const RichRecTrack::Segments & segments = (*track)->richRecSegments();
    for ( RichRecTrack::Segments::const_iterator iSeg = segments.begin();
          iSeg != segments.end(); ++iSeg )
    {
      // Add a summary segment for this reconstructed segment
      sumSegs.push_back( RichSummaryRadSegment() );
      // get reference to this segment
      RichSummaryRadSegment & sumSeg = sumSegs.back();

      // which radiator
      const Rich::RadiatorType rad = (*iSeg)->trackSegment().radiator();

      // set the radiator type
      sumSeg.setRadiator( rad );

      // Loop over all particle codes.
      for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) 
      {
        const Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);

        // Set expected CK theta angles
        sumSeg.setExpectedCkTheta( hypo, m_ckAngle->avgCherenkovTheta(*iSeg,hypo) );
        // set expected CK resolutions (errors)
        sumSeg.setExpectedCkThetaError( hypo, m_ckAngleRes->ckThetaResolution(*iSeg,hypo) );
        // Expected number of observable signal photons
        sumSeg.setExpectedNumPhotons( hypo, m_tkSignal->nObservableSignalPhotons(*iSeg,hypo) );
        
      }

      // vector of summary photons
      RichSummaryPhoton::Vector sumPhots;

      // Loop over photons for this segment and add info on those passing the selection
      const RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( *iSeg );
      for ( RichRecSegment::Photons::const_iterator iPhot = photons.begin();
            iPhot != photons.end(); ++iPhot )
      {
        // get Cherenkov angles
        const double ckTheta ( (*iPhot)->geomPhoton().CherenkovTheta() );
        const double ckPhi   ( (*iPhot)->geomPhoton().CherenkovPhi()   );

        // does this photon come with n-sigma of any of the expected CK angles for this segment ?
        bool OK ( false );
        for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
        {
          const Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);
          const double ckDiff = fabs( sumSeg.expectedCkTheta(hypo) - ckTheta );
          if ( ckDiff < m_nSigma[rad] * sumSeg.expectedCkThetaError(hypo) )
          {
            OK = true; break;
          }
        }

        // select all photons for testing
        //OK = true;

        // is this photon selected
        if ( OK )
        {
          // Add a new summary photon
          sumPhots.push_back( RichSummaryPhoton() );
          RichSummaryPhoton & sumPhot = sumPhots.back();
          // set angles
          sumPhot.setCherenkovTheta(ckTheta);
          sumPhot.setCherenkovPhi(ckPhi);
          // channel ID
          sumPhot.setSmartID( (*iPhot)->richRecPixel()->smartID() );
        }

      } // loop over reco photons

      // add vector of photons to segment
      sumSeg.setPhotons( sumPhots );

    } // loop over reco segments

    // add vector of segments to track
    sumTrack->setRadSegments( sumSegs );

  } // loop over reco tracks

  return StatusCode::SUCCESS;
}

StatusCode RichRecSummaryAlg::finalize()
{
  // Execute base class method and return
  return RichRecAlgBase::finalize();
}
