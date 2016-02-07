
//---------------------------------------------------------------------------
/** @file RichGhostTrackMoni.cpp
 *
 *  Implementation file for algorithm class : RichGhostTrackMoni
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
DECLARE_ALGORITHM_FACTORY( GhostTrackMoni )

// Standard constructor, initializes variables
GhostTrackMoni::GhostTrackMoni( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : TupleAlgBase ( name, pSvcLocator ),
    m_nSigma     ( Rich::NRadiatorTypes, 2 )
{
  declareProperty( "PhotonNSigma", m_nSigma );
}

// Destructor
GhostTrackMoni::~GhostTrackMoni() {}

// Initialize
StatusCode GhostTrackMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = TupleAlgBase::initialize();
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
    for ( auto * tk : *richTracks() )
    {
      // is this track a ghost ?
      const auto * mcP = m_richRecMCTruth->mcParticle(tk);
      const bool isGhost = ( mcP == nullptr );

      // Count reco photons
      CountVects nExpected, nReco;

      // loop over radiators
      for ( const auto rad : Rich::radiators() )
      {

        // Do we have a track segment for this radiator ?
        auto * seg  = tk->segmentInRad(rad);
        if ( seg )
        {

          // Loop over all particle codes
          for ( const auto hypo : Rich::particles() )
          {
            
            // store expected number of photons
            nExpected.data(rad)[hypo] = m_tkSignal->nObservableSignalPhotons(seg,hypo);
         
            // expected cherenkov theta
            const double expCKtheta = m_ckAngle->avgCherenkovTheta(seg,hypo);
   
            // Loop over photons for this segment and add info on those passing the selection
            unsigned int nPhots(0);
            const auto & photons = photonCreator()->reconstructPhotons( seg );
            for ( const auto * photon : photons )
            {
              const double ckDiff = fabs( expCKtheta - photon->geomPhoton().CherenkovTheta() );
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

      StatusCode sc = StatusCode::SUCCESS;

      sc = sc && tuple->column( "IsGhost", (int)isGhost );

      sc = sc && tuple->column( "RecoVertPtot", tk->vertexMomentum() );

      sc = sc && tuple->array( "NExpectAero",  nExpected.data(Rich::Aerogel).begin(),  nExpected.data(Rich::Aerogel).end()  );
      sc = sc && tuple->array( "NExpectR1Gas", nExpected.data(Rich::Rich1Gas).begin(), nExpected.data(Rich::Rich1Gas).end() );
      sc = sc && tuple->array( "NExpectR2Gas", nExpected.data(Rich::Rich2Gas).begin(), nExpected.data(Rich::Rich2Gas).end() );

      sc = sc && tuple->array( "NRecAero",  nReco.data(Rich::Aerogel).begin(),  nReco.data(Rich::Aerogel).end()  );
      sc = sc && tuple->array( "NRecR1Gas", nReco.data(Rich::Rich1Gas).begin(), nReco.data(Rich::Rich1Gas).end() );
      sc = sc && tuple->array( "NRecR2Gas", nReco.data(Rich::Rich2Gas).begin(), nReco.data(Rich::Rich2Gas).end() );

      sc = sc && tuple->write();

      if ( sc.isFailure() ) return sc;

    }

  } // have MC truth
  else
  {
    // Print one message only
    Warning( "No Track MC truth available -> Cannot find ghost tracks.", 1 );
  }

  return StatusCode::SUCCESS;
}
