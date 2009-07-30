
//---------------------------------------------------------------------------
/** @file RichTracklessRingMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::TracklessRingMoni
 *
 *  $Id: RichTracklessRingMoni.cpp,v 1.8 2009-07-30 11:02:36 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichTracklessRingMoni.h"

// namespace
using namespace Rich::Rec::MC;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TracklessRingMoni );

// Standard constructor, initializes variables
TracklessRingMoni::TracklessRingMoni( const std::string& name,
                                      ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_richRecMCTruth    ( NULL ),  
    m_ckAngle           ( NULL )
{
  using namespace boost::assign;
  declareProperty( "RingLocation", m_ringLoc = LHCb::RichRecRingLocation::ENNRings+"All" );
  declareProperty( "ChThetaRecHistoLimitMin",
                   m_ckThetaMin = list_of(0.1)(0.025)(0.02) );
  declareProperty( "ChThetaRecHistoLimitMax",
                   m_ckThetaMax = list_of(0.3)(0.06)(0.035) );
  declareProperty( "RingRadiiHistoLimitMin",
                   m_radiiMin = list_of(200.0)(20.0)(60.0) );
  declareProperty( "RingRadiiHistoLimitMax",
                   m_radiiMax = list_of(400.0)(100.0)(175.0) );
  declareProperty( "MaxFitVariance",
                   m_maxFitVariance = list_of(100)(100)(100) );
  declareProperty( "CKThetaRes", m_ckThetaRes = list_of(0.2)(0.0075)(0.0075) );
  declareProperty( "MCAssocFrac", m_mcAssocFrac = 0.75 );
}

// Destructor
TracklessRingMoni::~TracklessRingMoni() {}

//  Initialize
StatusCode TracklessRingMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  debug() << "Monitoring Trackless rings at '" << m_ringLoc << "'" << endmsg;

  return sc;
}

StatusCode TracklessRingMoni::prebookHistograms()
{
  // Book (MC free) histos which must be ready after initialisation
  // for online monitoring
 
  // Loop over radiators
  for ( Rich::Radiators::const_iterator rad = Rich::radiators().begin();
        rad != Rich::radiators().end(); ++rad )
  {
    richHisto1D( *rad, "ringRadii", "Trackless Ring Radii",
                 m_ckThetaMin[*rad], m_ckThetaMax[*rad], nBins1D() );
    richHisto1D( *rad, "totalPhotons", "Photon Yield : Trackless Rings",
                 -0.5, 50.5, 51 );
    richHisto1D( *rad, "ringRadiiRefitted", "Refitted Trackless Ring Radii (mm on HPD plane)",
                 m_radiiMin[*rad], m_radiiMax[*rad], nBins1D() );
    richHisto1D( *rad, "nRings", "Number of Rings",
                 0.5, 50.5, 50 );
  }

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode TracklessRingMoni::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Retrieve rings
  if ( !exist<LHCb::RichRecRings>(m_ringLoc) ) return StatusCode::SUCCESS;
  const LHCb::RichRecRings * rings = get<LHCb::RichRecRings>( m_ringLoc );
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << rings->size() << " rings at " << m_ringLoc << endmsg;

  // Rich Histo ID
  const RichHistoID hid;

  // loop
  Rich::Map<Rich::RadiatorType,unsigned int> ringsPerRad;
  for ( LHCb::RichRecRings::const_iterator iR = rings->begin();
        iR != rings->end(); ++iR )
  {
    LHCb::RichRecRing * ring = *iR;

    // Radiator info
    const Rich::RadiatorType rad = ring->radiator();

    // Ring knows if it is associated or not
    LHCb::RichRecSegment * nearestSeg = ring->richRecSegment();

    // Is MC available
    const bool mcTrackOK = richRecMCTool()->trackToMCPAvailable();
    // Get the Cherenkov angle for the nearest segment and ring radius
    Rich::ParticleIDType mcType = Pion;
    bool hasTrackMC = false;
    const LHCb::MCParticle * segmentMCP(NULL);
    if ( mcTrackOK )
    {
      // True particle
      segmentMCP = richRecMCTool()->mcParticle(nearestSeg);
      mcType     = richRecMCTool()->mcParticleType(nearestSeg);
      hasTrackMC  = ( segmentMCP != NULL );
      if ( Rich::Unknown  == mcType ) mcType = Pion;
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly.
    }

    // count rings per radiator
    ++ringsPerRad[rad];

    const double rRadius = ring->radius(); // Ring radius (rads)

    // radius plot
    richHisto1D(rad,"ringRadii")->fill(rRadius);

    // Expected CK theta for associated track
    const double avChTheta = ( hasTrackMC && nearestSeg ?
                               m_ckAngle->avgCherenkovTheta(nearestSeg,mcType) : 999.9 );

    if ( hasTrackMC && nearestSeg )
    {
      richProfile1D( rad, "MCChThetaVRingRadius", "Ring Radius versus expected CK theta",
                     m_ckThetaMin[rad], m_ckThetaMax[rad],
                     nBins1D() ) -> fill( rRadius, avChTheta );
      richHisto1D( rad, "ringRadVCKexpect", "Ring radius - Expected track CK theta",
                   -m_ckThetaRes[rad], m_ckThetaRes[rad], nBins1D() ) -> fill( rRadius-avChTheta );
    }

    // photon yield
    richHisto1D( rad, "totalPhotons" )->fill( (*iR)->richRecPixels().size() );

    // refit the ring ...
    FastRingFitter fitter(**iR);
    // run the fit
    fitter.fit();
    // plot the fitted radius
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Fit "<< fitter.result().Status
                << " " << fitter.result().Radius
                << " " << fitter.result().Variance
                << " " << fitter.numberOfPoints()
                << endmsg;
    const bool refitOK = ( fitter.result().Status == 0 &&
                           fitter.result().Variance < m_maxFitVariance[rad] );
    if ( refitOK )
    {
      richHisto1D( rad, "ringRadiiRefitted" )->fill( fitter.result().Radius );
    }

    if ( richRecMCTool()->pixelMCHistoryAvailable() )
    {
      const IMCTruthTool::MCPartAssocInfo mcinfo = richRecMCTool()->mcParticle(ring,m_mcAssocFrac);
      if ( mcinfo.mcParticle != NULL )
      {
        // refitted radius
        if ( refitOK )
        {
          richHisto1D( rad, "ringRadiiRefittedMCtrue", 
                       "Refitted Trackless Ring Radii (mm on HPD plane) : MCTrue Rings",
                       m_radiiMin[rad], m_radiiMax[rad], nBins1D() ) 
            -> fill( fitter.result().Radius );
        }

        // association fraction
        richHisto1D( rad, "mcAssocFrac", "MC Association purity", 
                     m_mcAssocFrac, 1, nBins1D() ) -> fill ( mcinfo.associationFrac );

        // Compare to MC segment expected values
        if ( hasTrackMC && nearestSeg && segmentMCP == mcinfo.mcParticle )
        {
          richProfile1D( rad, "MCChThetaVRingRadiusMCtrue",
                         "Ring Radius versus expected CK theta : MCTrue Rings",
                         m_ckThetaMin[rad], m_ckThetaMax[rad], nBins1D() )
            -> fill ( rRadius, avChTheta );
          richHisto1D( rad, "ringRadVCKexpectMCtrue",
                       "Ring radius - Expected track CK theta : MCTrue Rings",
                       -m_ckThetaRes[rad], m_ckThetaRes[rad], nBins1D() )
            -> fill ( rRadius-avChTheta );
        }

      }
    }

  } // outer ring loop

  for ( Rich::Radiators::const_iterator iRad = Rich::radiators().begin();
        iRad != Rich::radiators().end(); ++iRad )
  {
    if ( ringsPerRad[*iRad]>0 )
      richHisto1D( *iRad, "nRings" )->fill( ringsPerRad[*iRad] );
  }

  return StatusCode::SUCCESS;
}
