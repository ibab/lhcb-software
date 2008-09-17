
//---------------------------------------------------------------------------
/** @file RichMarkovRingFinderMoni.cpp
 *
 *  Implementation file for algorithm class : RichMarkovRingFinderMoni
 *
 *  $Id: RichMarkovRingFinderMoni.cpp,v 1.42 2008-09-17 12:34:20 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

// local
#include "RichMarkovRingFinderMoni.h"

// namespace
using namespace LHCb;
using namespace Rich::Rec::MarkovRingFinder::MC;

//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Moni );

// Standard constructor, initializes variables
Moni::Moni( const std::string& name,
            ISvcLocator* pSvcLocator )
  : RichRecHistoAlgBase ( name, pSvcLocator )
{
  declareProperty( "RingLocation", m_ringLoc = LHCb::RichRecRingLocation::MarkovRings+"All" );
  declareProperty( "ChThetaRecHistoLimitMin",
                   m_ckThetaMin = boost::assign::list_of(0.1)(0.03)(0.01) );
  declareProperty( "ChThetaRecHistoLimitMax",
                   m_ckThetaMax = boost::assign::list_of(0.3)(0.08)(0.05) );
  declareProperty( "NumberBins", m_nBins = 100 );
  declareProperty( "MaxFitVariance", m_maxFitVariance = 200 );
}

// Destructor
Moni::~Moni() {}

//  Initialize
StatusCode Moni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  debug() << "Monitoring Trackless rings " << m_ringLoc << endreq;

  return sc;
}

// Main execution
StatusCode Moni::execute()
{
  // Check event status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // Retrieve rings
  if ( !exist<LHCb::RichRecRings>(m_ringLoc) ) return StatusCode::SUCCESS;
  const LHCb::RichRecRings * rings = get<LHCb::RichRecRings>( m_ringLoc );
  debug() << "Found " << rings->size() << " rings at " << m_ringLoc << endreq;

  // Rich Histo ID
  const RichHistoID hid;

  // loop
  for ( LHCb::RichRecRings::const_iterator iR = rings->begin();
        iR != rings->end(); ++iR )
  {
    LHCb::RichRecRing * ring = *iR;

    // Radiator info
    const Rich::RadiatorType rad = ring->radiator();
    const std::string& RAD = Rich::text(rad);

    // Ring knows if it is associated or not
    RichRecSegment * nearestSeg = ring->richRecSegment();

    // Is MC available
    const bool mcTrackOK = richRecMCTool()->trackToMCPAvailable();
    // Get the Cherenkov angle for the nearest segment and ring radius
    Rich::ParticleIDType mcType = Pion;
    bool hasMC = false;
    if ( mcTrackOK )
    {
      // True particle type
      mcType = richRecMCTool()->mcParticleType(nearestSeg);
      hasMC  = ( Rich::Unknown != mcType );
      //if ( Rich::Unknown  == mcType ) continue; // skip tracks with unknown MC type
      if ( Rich::Unknown  == mcType ) mcType = Pion;
      if ( Rich::Electron == mcType ) continue; // skip electrons which are reconstructed badly.
    }

    const RichRecRing::FloatType rRadius = ring->radius();//RADIUS IN RADIANS!

    // radius plot
    plot1D( rRadius, hid(rad,"ringRadii"), RAD+" Trackless Ring Radii",
            m_ckThetaMin[rad], m_ckThetaMax[rad], m_nBins );

    if ( hasMC && nearestSeg )
    {
      const RichRecSegment::FloatType avChTheta = nearestSeg->averageCKTheta(mcType);
      // Plot nearest segment Ch theta against ring radius (both in radians)
      plot2D( rRadius, avChTheta, hid(rad,"MCChThetaVRingRadius"),
              RAD+" MCChThetaVRingRadius",
              m_ckThetaMin[rad], m_ckThetaMax[rad],
              m_ckThetaMin[rad], m_ckThetaMax[rad],
              m_nBins, m_nBins );
    }

    // photon yield
    plot1D( (*iR)->richRecPixels().size(),
            hid(rad,"totalPhotons"),
            RAD+" Photon Yield : Trackless Rings",
            -0.5, 50.5, 51 );

    // refit the ring ...
    //FastRingFitter fitter(**iR);
    FastRingFitter fitter;
    // loop over hits on the ring
    for ( LHCb::RichRecPixelOnRing::Vector::iterator iP = (*iR)->richRecPixels().begin();
          iP != (*iR)->richRecPixels().end(); ++iP )
    {
      // get pixel from pixelOnRing
      LHCb::RichRecPixel * pixel = (*iP).pixel();
      // pixel hit
      //const Gaudi::XYZPoint & PixelLocal = pixel->localPosition();
      const Gaudi::XYZPoint & PixelLocal = pixel->radCorrLocalPositions().position(rad);
      // add (x,y) point to the fitter
      fitter.addPoint( PixelLocal.x(), PixelLocal.y() );
    }
    // run the fit
    fitter.fit();
    // plot the fitted radius
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Fit "<< fitter.result().Status
                << " " << fitter.result().Radius
                << " " << fitter.result().Variance
                << " " << fitter.numberOfPoints()
                << endreq;
    if ( fitter.result().Status == 0 &&
         fitter.result().Variance < m_maxFitVariance )
    {
      plot1D( fitter.result().Radius, hid(rad,"ringRadiiRefitted"),
              RAD+" Refitted Trackless Ring Radii (mm on HPD plane)",
              0, 200, m_nBins );
    }

  }//outer ring loop

  return StatusCode::SUCCESS;
}
