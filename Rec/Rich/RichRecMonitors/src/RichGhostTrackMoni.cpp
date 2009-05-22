
//---------------------------------------------------------------------------
/** @file RichGhostTrackMoni.cpp
 *
 *  Implementation file for algorithm class : RichGhostTrackMoni
 *
 *  $Id: RichGhostTrackMoni.cpp,v 1.4 2009-05-22 15:50:08 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichGhostTrackMoni.h"

using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GhostTrackMoni );

// Standard constructor, initializes variables
GhostTrackMoni::GhostTrackMoni( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : RichRecTupleAlgBase ( name, pSvcLocator ),
    m_trSelector        ( NULL ),
    m_richRecMCTruth    ( NULL ),
    m_tkSignal          ( NULL ),
    m_ckAngle           ( NULL ),
    m_ckAngleRes        ( NULL ),
    m_nSigma            ( Rich::NRadiatorTypes, 2 )
{
  declareProperty( "PhotonNSigma",    m_nSigma );
}

// Destructor
GhostTrackMoni::~GhostTrackMoni() {}

// Initialize
StatusCode GhostTrackMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecTupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // tools
  acquireTool( "TrackSelector", m_trSelector, this );
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichCherenkovResolution", m_ckAngleRes  );
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth   );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal    );

  return sc;
}

// Main execution
StatusCode GhostTrackMoni::execute()
{

  // Do we have track MC truth
  if ( m_richRecMCTruth->trackToMCPAvailable() )
  {

    // loop over tracks
    for ( LHCb::RichRecTracks::const_iterator iT = richTracks()->begin();
          iT != richTracks()->end(); ++iT )
    {
      // is this track a ghost ?
      const LHCb::MCParticle * mcP = m_richRecMCTruth->mcParticle(*iT);
      const bool isGhost = (mcP == NULL);

      // Count reco photons
      CountVects nExpected, nReco;

      // loop over radiators
      for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
      {
        const Rich::RadiatorType rad = static_cast<Rich::RadiatorType>(iRad);

        // Do we have a track segment for this radiator ?
        LHCb::RichRecSegment * seg  = (*iT)->segmentInRad(rad);
        if ( seg )
        {

          // Loop over all particle codes
          for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo )
          {
            const Rich::ParticleIDType hypo = static_cast<Rich::ParticleIDType>(iHypo);
            
            // store expected number of photons
            nExpected.data(rad)[hypo] = m_tkSignal->nObservableSignalPhotons(seg,hypo);
         
            // expected cherenkov theta
            const double expCKtheta = m_ckAngle->avgCherenkovTheta(seg,hypo);
   
            // Loop over photons for this segment and add info on those passing the selection
            unsigned int nPhots(0);
            const LHCb::RichRecSegment::Photons & photons = photonCreator()->reconstructPhotons( seg );
            for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = photons.begin();
                  iPhot != photons.end(); ++iPhot )
            {
              const double ckDiff = fabs( expCKtheta - (*iPhot)->geomPhoton().CherenkovTheta() );
              if ( ckDiff < m_nSigma[rad] * m_ckAngleRes->ckThetaResolution(seg,hypo) )
              {
                ++nPhots;
              }
            }

            // store number of reco photons
            nReco.data(rad)[hypo] = nPhots;

          }
        }
      }

      // fill a tuple with track info
      Tuple tuple = nTuple( "ghosts", "GhostTracksTuple" );

      tuple->column( "IsGhost", (int)isGhost );

      tuple->column( "RecoVertPtot", (*iT)->vertexMomentum() );

      tuple->array( "NExpectAero",  nExpected.data(Rich::Aerogel).begin(),  nExpected.data(Rich::Aerogel).end()  );
      tuple->array( "NExpectR1Gas", nExpected.data(Rich::Rich1Gas).begin(), nExpected.data(Rich::Rich1Gas).end() );
      tuple->array( "NExpectR2Gas", nExpected.data(Rich::Rich2Gas).begin(), nExpected.data(Rich::Rich2Gas).end() );

      tuple->array( "NRecAero",  nReco.data(Rich::Aerogel).begin(),  nReco.data(Rich::Aerogel).end()  );
      tuple->array( "NRecR1Gas", nReco.data(Rich::Rich1Gas).begin(), nReco.data(Rich::Rich1Gas).end() );
      tuple->array( "NRecR2Gas", nReco.data(Rich::Rich2Gas).begin(), nReco.data(Rich::Rich2Gas).end() );

      tuple->write();

    }

  } // have MC truth
  else
  {
    // Print one message only
    Warning( "No Track MC truth available -> Cannot find ghost tracks.", 1 );
  }

  return StatusCode::SUCCESS;
}

// Finalize
StatusCode GhostTrackMoni::finalize()
{
  // Execute base class method
  return RichRecTupleAlgBase::finalize();
}
