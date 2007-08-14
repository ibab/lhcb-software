
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingCKEstiFromRadius.cpp
 *
 * Implementation file for class : Rich::Rec::PhotonRecoUsingCKEstiFromRadius
 *
 * CVS Log :-
 * $Id: RichPhotonRecoUsingCKEstiFromRadius.cpp,v 1.4 2007-08-14 15:10:38 jonrob Exp $
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
DECLARE_TOOL_FACTORY ( PhotonRecoUsingCKEstiFromRadius );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhotonRecoUsingCKEstiFromRadius::
PhotonRecoUsingCKEstiFromRadius( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent )
  : Rich::Rec::ToolBase ( type, name, parent ),
    m_idTool        ( NULL ),
    m_massHypoRings ( NULL ),
    m_ckAngle       ( NULL ),
    m_richPartProp  ( NULL ),
    m_scale         ( Rich::NRadiatorTypes, 0 ),
    m_ckThetaMax    ( Rich::NRadiatorTypes, 0 ),
    m_sepGMax       ( Rich::NRadiatorTypes, 0 ),
    m_ckFudge       ( Rich::NRadiatorTypes, 0 )
{

  // declare interface
  declareInterface<IPhotonReconstruction>(this);

  m_ckThetaMax[Rich::Aerogel]  = 0.24;
  m_ckThetaMax[Rich::Rich1Gas] = 0.052;
  m_ckThetaMax[Rich::Rich2Gas] = 0.03;
  declareProperty( "CKthetaMax", m_ckThetaMax );

  m_sepGMax[Rich::Aerogel]   = 342;
  m_sepGMax[Rich::Rich1Gas]  = 75;
  m_sepGMax[Rich::Rich2Gas]  = 130;
  declareProperty( "SepGMax", m_sepGMax );

  // Default fudge factors for DC06
  m_ckFudge[Rich::Aerogel] =  0.0009376;
  m_ckFudge[Rich::C4F10]   =  0.0003628;
  m_ckFudge[Rich::CF4]     =  0.0001462;
  declareProperty( "CKThetaQuartzRefractCorrections", m_ckFudge );

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
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  acquireTool( "RichSmartIDTool",     m_idTool, 0, true  );
  acquireTool( "RichMassHypoRings",   m_massHypoRings    );
  acquireTool( "RichCherenkovAngle",  m_ckAngle          );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endreq;

  // loop over radiators
  for ( Rich::Radiators::const_iterator irad = Rich::radiators().begin();
        irad != Rich::radiators().end(); ++irad )
  {
    const Rich::RadiatorType rad = *irad;
    // scale factor
    m_scale[rad] = m_ckThetaMax[rad] / m_sepGMax[rad];
    // fudge factor warning
    if ( fabs(m_ckFudge[rad]) > 1e-7 )
    {
      std::ostringstream mess;
      mess << "Applying " << Rich::text(rad)
           << " CK theta correction factor : " << m_ckFudge[rad];
      Warning( mess.str(), StatusCode::SUCCESS );
    }
  }

  return sc;
}

//=========================================================================
//  reconstruct a photon from track segment and smartID
//=========================================================================
StatusCode
PhotonRecoUsingCKEstiFromRadius::
reconstructPhoton ( const LHCb::RichTrackSegment& /* trSeg */,
                    const Rich::HPDPixelCluster& /* smartIDs */,
                    LHCb::RichGeomPhoton& /* gPhoton */ ) const
{
  return Warning( "Method Not implemented for this photon reconstruction tool" );
}

//-------------------------------------------------------------------------
//  reconstruct a photon from track segment and detection point
//-------------------------------------------------------------------------
StatusCode
PhotonRecoUsingCKEstiFromRadius::
reconstructPhoton ( const LHCb::RichTrackSegment& /* trSeg */,
                    const Gaudi::XYZPoint& /* detectionPoint */,
                    LHCb::RichGeomPhoton& /* gPhoton */,
                    const Rich::HPDPixelCluster& /* smartIDs */ ) const
{
  return Warning( "Method Not implemented for this photon reconstruction tool" );
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
  // track segment
  const LHCb::RichTrackSegment& trSeg = segment->trackSegment();

  // Detector information (RICH, radiator and HPD panel)
  //const Rich::DetectorType rich     = trSeg.rich();
  const Rich::RadiatorType radiator = trSeg.radiator();
  const Rich::Side side             = pixel->panel().panel();

  // Emission point to use for photon reconstruction
  Gaudi::XYZPoint & emissionPoint = gPhoton.emissionPoint();
  emissionPoint = trSeg.bestPoint();

  // Cannot set these ??
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
  const float diff_x = segPSide.x() - pixPRad.x();
  const float diff_y = segPSide.y() - pixPRad.y();

  // estimate phi from these hits
  // CRJ : The atan2 here is a significant fraction of the time taken by this method
  //       Only need it approximately, so scope for use of some sort of 'fast' version here ?
  const float phiCerenkov = Gaudi::Units::pi + atan2( diff_y, diff_x );

  // seg - pixel separation
  const double sep2 = gsl_pow_2(diff_x) + gsl_pow_2(diff_y);

  // Start with CK fudge factor
  float thetaCerenkov( m_ckFudge[radiator] );

  // use ring info to determine CK theta
  LHCb::RichRecSegment * seg             = const_cast<LHCb::RichRecSegment*>(segment); // need to remove this
  const LHCb::RichRecRing        * ring  = m_massHypoRings->massHypoRing( seg, m_pidTypes.front() );
  const LHCb::RichRecPointOnRing * point = ( ring ? ring->getPointClosestInAzimuth(phiCerenkov) : NULL );
  if ( point )
  {
    const double sep2_tmp = ( gsl_pow_2(segPSide.x()-point->localPosition().x()) +
                              gsl_pow_2(segPSide.y()-point->localPosition().y()) );
    thetaCerenkov += ring->radius() * sqrt( sep2 / sep2_tmp );
  }
  else
  {
    thetaCerenkov += sqrt(sep2) * m_scale[radiator];
  }

  // --------------------------------------------------------------------------------------
  // Set (remaining) photon parameters
  // --------------------------------------------------------------------------------------
  gPhoton.setCherenkovTheta         ( thetaCerenkov  );
  gPhoton.setCherenkovPhi           ( phiCerenkov    );
  gPhoton.setActiveSegmentFraction  ( fraction       );
  gPhoton.setDetectionPoint         ( pixel->globalPosition()  );
  gPhoton.setSmartID                ( pixel->hpdPixelCluster().primaryID() );
  gPhoton.setMirrorNumValid         ( unambigPhoton  );
  gPhoton.setSphMirrorNum           ( 0 );
  gPhoton.setFlatMirrorNum          ( 0 );
  //if ( msgLevel(MSG::VERBOSE) )
  //{
  // verbose() << "Created photon " << gPhoton << endreq;
  //}
  // --------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;
}
