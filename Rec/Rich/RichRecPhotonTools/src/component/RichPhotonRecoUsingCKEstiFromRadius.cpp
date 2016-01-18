
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingCKEstiFromRadius.cpp
 *
 * Implementation file for class : Rich::Rec::PhotonRecoUsingCKEstiFromRadius
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @author Antonis Papanestis
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonRecoUsingCKEstiFromRadius.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhotonRecoUsingCKEstiFromRadius::
PhotonRecoUsingCKEstiFromRadius( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : PhotonRecoBase       ( type, name, parent       ),
    m_minCalibRingRadius ( Rich::NRadiatorTypes, -1 )
{
  // Job options
  declareProperty( "UseLightestHypoOnly", m_useLightestHypoOnly = false );
  declareProperty( "MinFracCKTheta", m_minFracCKtheta = { 1.0, 0.05, 0.05 } );

  // Corrections for the intrinsic biases
  m_ckBiasCorrs = { 0.0, 4.80e-6, 6.33e-5 };
}

//=============================================================================
// Destructor
//=============================================================================
PhotonRecoUsingCKEstiFromRadius::~PhotonRecoUsingCKEstiFromRadius() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode PhotonRecoUsingCKEstiFromRadius::initialize()
{
  // initialise base class
  const StatusCode sc = PhotonRecoBase::initialize();
  if ( sc.isFailure() ) return sc;

  acquireTool( "RichSmartIDTool", m_idTool, nullptr, true  );
  acquireTool( "RichMassHypoRings",        m_massHypoRings );
  acquireTool( "RichCherenkovAngle",       m_ckAngle       );
  acquireTool( "RichParticleProperties",   m_richPartProp  );

  m_pidTypes = m_richPartProp->particleTypes();
  if ( m_useLightestHypoOnly )
  {
    m_pidTypes = { m_pidTypes.front() }; // Shrink list to only the first entry
    Warning( "Forcing use of only lightest active mass hypothesis for calibration points",
             StatusCode::SUCCESS );
  }
  _ri_debug << "Particle types considered for calibration points = " << m_pidTypes
            << endmsg;

  return sc;
}

//-------------------------------------------------------------------------
//  reconstruct a photon from track segment and pixel
//-------------------------------------------------------------------------
StatusCode
PhotonRecoUsingCKEstiFromRadius::
reconstructPhoton ( const LHCb::RichRecSegment * segment,
                    const LHCb::RichRecPixel * pixel,
                    LHCb::RichGeomPhoton& gPhoton ) const
{
  // return status. Default is OK until proved otherwise
  StatusCode sc = StatusCode::SUCCESS;

  // track segment
  const LHCb::RichTrackSegment& trSeg = segment->trackSegment();

  // Detector information (RICH, radiator and HPD panel)
  //const Rich::DetectorType rich     = trSeg.rich();
  const Rich::RadiatorType radiator = trSeg.radiator();
  const Rich::Side side             = pixel->panel().panel();

  // Emission point to use for photon reconstruction
  Gaudi::XYZPoint & emissionPoint = gPhoton.emissionPoint();
  emissPoint()->emissionPoint( segment, pixel, emissionPoint );

  // Cannot set these yet - Could be done if needed, but at what CPU cost ?
  //Gaudi::XYZPoint & sphReflPoint = gPhoton.sphMirReflectionPoint();
  //Gaudi::XYZPoint & secReflPoint = gPhoton.flatMirReflectionPoint();

  // fraction of segment path length accessible to the photon
  // cannot determine this here so set to 1
  const float fraction(1);

  // flag to say if this photon candidate is un-ambiguous
  // cannot determine this here so set to false
  const bool unambigPhoton(false);

  // Projected track position in local coords on the detector plane
  const auto & segPSide = segment->pdPanelHitPointLocal(side);

  // Pixel position in local coords on detector plane
  const auto & pixPRad  = pixel->radCorrLocalPositions().position(radiator);

  // x,y differences
  const auto diff_x = ( segPSide.x() - pixPRad.x() );
  const auto diff_y = ( segPSide.y() - pixPRad.y() );
  // The track - pixel seperation^2
  const auto track_pix_sep2 = std::pow(diff_x,2) + std::pow(diff_y,2);

  // estimate phi from these hits
  // use full atan2
  //const auto phiCerenkov = (float)( Gaudi::Units::pi + std::atan2(diff_y,diff_x) );
  // Use VDT
  const auto phiCerenkov = (float) ( Gaudi::Units::pi + vdt::fast_atan2f(diff_y,diff_x) );

  // Find the best ring and point to use as the calibration point
  const LHCb::RichRecRing        * ring  = nullptr;
  const LHCb::RichRecPointOnRing * point = nullptr;
  float sep2_calib = std::numeric_limits<float>::max();
  float sep2_diff  = std::numeric_limits<float>::max();
  for ( const auto& pid : m_pidTypes )
  {

    // Load the ring and select the point for this PID type
    const auto * ring_tmp = m_massHypoRings->massHypoRing( segment, pid );

    // check how close to threshold the ring is. Do not use if too close.
    if ( ring_tmp && ring_tmp->radius() > minCalibRingRadius(radiator) )
    {

      // select the calibration point to use from this ring
      const auto * point_tmp = ring_tmp->getPointClosestInAzimuth(phiCerenkov);

      // if found see if this point is better than the last one tried
      if ( point_tmp && sameSide( radiator, pixPRad, point_tmp->localPosition() ) )
      {
        // local pixel (x,y)
        const auto lPix_x = point_tmp->localPosition().x();
        const auto lPix_y = point_tmp->localPosition().y();
        // pixel - calibration point seperation^2
        const auto pix_calib_sep2 = ( std::pow( pixPRad.x() - lPix_x, 2 ) +
                                      std::pow( pixPRad.y() - lPix_y, 2 ) );
        // Is this point a better calibration point to use ?
        if ( pix_calib_sep2 < sep2_diff )
        {
          // updated decision variable
          sep2_diff  = pix_calib_sep2;
          // update best ring and pixel pointers
          ring       = ring_tmp;
          point      = point_tmp;
          // update track - calibration point separation^2
          sep2_calib = ( std::pow( segPSide.x() - lPix_x, 2 ) +
                         std::pow( segPSide.y() - lPix_y, 2 ) );
        }
        else if ( ring )
        {
          // a ring is already found, and we are getting further away, so break out ...
          break;
        }

      } // point found

    } // ring saturation check

  } // loop over PID types

  // If a good calibration point was found, use its info to fill the photon
  if ( ring )
  {

    // Compute CK theta from reference point + fudge factor correction
    const float thetaCerenkov =
      ( ckThetaCorrection(radiator) +
        ( ring->radius() * std::sqrt(track_pix_sep2/sep2_calib) ) );

    // --------------------------------------------------------------------------------------
    // Set (remaining) photon parameters
    // --------------------------------------------------------------------------------------
    gPhoton.setCherenkovTheta         ( thetaCerenkov            );
    gPhoton.setCherenkovPhi           ( phiCerenkov              );
    gPhoton.setActiveSegmentFraction  ( fraction                 );
    gPhoton.setDetectionPoint         ( pixel->globalPosition()  );
    gPhoton.setSmartID                ( pixel->hpdPixelCluster().primaryID() );
    gPhoton.setUnambiguousPhoton      ( unambigPhoton            );
    gPhoton.setPrimaryMirror          ( point->primaryMirror()   );
    gPhoton.setSecondaryMirror        ( point->secondaryMirror() );
    // --------------------------------------------------------------------------------------

    // Print the photon
    _ri_verbo << "Created photon " << gPhoton << endmsg;

  }
  else
  {
    _ri_verbo << "No closest point found !" << endmsg;
    sc = StatusCode::FAILURE;
  }

  // Return final status
  return sc;
}

//=============================================================================
// Declaration of the Algorithm Factory
//=============================================================================

DECLARE_TOOL_FACTORY ( PhotonRecoUsingCKEstiFromRadius )

//=============================================================================

