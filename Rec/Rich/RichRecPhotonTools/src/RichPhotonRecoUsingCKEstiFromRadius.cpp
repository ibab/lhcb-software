
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

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY ( PhotonRecoUsingCKEstiFromRadius )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhotonRecoUsingCKEstiFromRadius::
PhotonRecoUsingCKEstiFromRadius( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : PhotonRecoBase  ( type, name, parent ),
    m_idTool        ( NULL ),
    m_massHypoRings ( NULL ),
    m_ckAngle       ( NULL ),
    m_richPartProp  ( NULL ),
    m_scale         ( Rich::NRadiatorTypes, 0 ),
    m_ckThetaMax    ( Rich::NRadiatorTypes, 0 ),
    m_sepGMax       ( Rich::NRadiatorTypes, 0 )
{
  m_ckThetaMax[Rich::Aerogel]  = 0.24;
  m_ckThetaMax[Rich::Rich1Gas] = 0.052;
  m_ckThetaMax[Rich::Rich2Gas] = 0.03;
  declareProperty( "CKthetaMax", m_ckThetaMax );

  m_sepGMax[Rich::Aerogel]   = 342;
  m_sepGMax[Rich::Rich1Gas]  = 75;
  m_sepGMax[Rich::Rich2Gas]  = 130;
  declareProperty( "SepGMax", m_sepGMax );
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

  acquireTool( "RichSmartIDTool",     m_idTool, 0, true  );
  acquireTool( "RichMassHypoRings",   m_massHypoRings    );
  acquireTool( "RichCherenkovAngle",  m_ckAngle          );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endmsg;

  // loop over radiators
  for ( Rich::Radiators::const_iterator irad = Rich::radiators().begin();
        irad != Rich::radiators().end(); ++irad )
  {
    // scale factor for each
    m_scale[*irad] = m_ckThetaMax[*irad] / m_sepGMax[*irad];
  }

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
  // return status. Default is failed until proved otherwise
  StatusCode sc = StatusCode::FAILURE;

  // track segment
  const LHCb::RichTrackSegment& trSeg = segment->trackSegment();

  // Detector information (RICH, radiator and HPD panel)
  //const Rich::DetectorType rich     = trSeg.rich();
  const Rich::RadiatorType radiator = trSeg.radiator();
  const Rich::Side side             = pixel->panel().panel();

  // Emission point to use for photon reconstruction
  Gaudi::XYZPoint & emissionPoint = gPhoton.emissionPoint();
  emissionPoint = trSeg.bestPoint();

  // Cannot set these yet - Could be done if needed, but at what CPU cost ?
  //Gaudi::XYZPoint & sphReflPoint = gPhoton.sphMirReflectionPoint();
  //Gaudi::XYZPoint & secReflPoint = gPhoton.flatMirReflectionPoint();

  // fraction of segment path length accessible to the photon
  // cannot determine this here so set to 1
  const float fraction(1);

  // flag to say if this photon candidate is un-ambiguous
  // cannot determine this here so set to false
  const bool unambigPhoton(false);

  // Positions on the the local det plane
  const Gaudi::XYZPoint & segPSide = segment->pdPanelHitPointLocal(side);
  const Gaudi::XYZPoint & pixPRad  = pixel->radCorrLocalPositions().position(radiator);

  // x,y differences
  const float diff_x = (float) ( segPSide.x() - pixPRad.x() );
  const float diff_y = (float) ( segPSide.y() - pixPRad.y() );

  // estimate phi from these hits
  // use full atan2
  //const float phiCerenkov = (float)( Gaudi::Units::pi + std::atan2(diff_y,diff_x) );
  // Use VDT
  const float phiCerenkov = (float) ( Gaudi::Units::pi + vdt::fast_atan2f(diff_y,diff_x) );

  // use ring info to determine CK theta
  LHCb::RichRecSegment           * seg   = const_cast<LHCb::RichRecSegment*>(segment); // need to remove this
  const LHCb::RichRecRing        * ring  = m_massHypoRings->massHypoRing( seg, m_pidTypes.front() );
  const LHCb::RichRecPointOnRing * point = ( ring ? ring->getPointClosestInAzimuth(phiCerenkov) : NULL );
  if ( point )
  {

    // Check if the point is OK for uses as a reference
    if ( sameSide( radiator, pixPRad, point->localPosition() ) )
    {

      // Start with CK fudge factor
      float thetaCerenkov( (float)(m_ckFudge[radiator]) );

      // estimate CK theta from reference point
      const double sep2_tmp = ( gsl_pow_2(segPSide.x()-point->localPosition().x()) +
                                gsl_pow_2(segPSide.y()-point->localPosition().y()) );
      thetaCerenkov += (float) ( ring->radius() *
                                 std::sqrt( (gsl_pow_2(diff_x)+gsl_pow_2(diff_y)) / sep2_tmp ) );

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

      // photon reco worked !
      sc = StatusCode::SUCCESS;

    }

  } // reference point located OK

  // if we get here photon reco failed
  return sc;
}
