
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingQuarticSoln.cpp
 *
 * Implementation file for class : Rich::Rec::PhotonRecoUsingQuarticSoln
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @author Antonis Papanestis
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonRecoUsingQuarticSoln.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhotonRecoUsingQuarticSoln::
PhotonRecoUsingQuarticSoln( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
: PhotonRecoBase        ( type, name, parent ),
  m_testForUnambigPhots ( Rich::NRadiatorTypes, true  ),
  m_rejectAmbigPhots    ( Rich::NRadiatorTypes, false ),
  m_useAlignedMirrSegs  ( Rich::NRadiatorTypes, true  ),
  m_nQits               ( Rich::NRadiatorTypes, 3     ),
  m_checkBeamPipe       ( Rich::NRadiatorTypes, true  ),
  m_checkPrimMirrSegs   ( Rich::NRadiatorTypes, false ),
  m_minActiveFrac       ( Rich::NRadiatorTypes, 0.2   ),
  m_minSphMirrTolIt     ( Rich::NRadiatorTypes        )
{

  // job options
  declareProperty( "FindUnambiguousPhotons",    m_testForUnambigPhots         );
  declareProperty( "UseMirrorSegmentAllignment", m_useAlignedMirrSegs         );
  declareProperty( "AssumeFlatSecondaries",     m_forceFlatAssumption = false );
  declareProperty( "NQuarticIterationsForSecMirrors",         m_nQits         );
  declareProperty( "UseSecondaryMirrors",                m_useSecMirs = true  );
  declareProperty( "RejectAmbiguousPhotons",       m_rejectAmbigPhots         );
  declareProperty( "CheckBeamPipe",                   m_checkBeamPipe         );
  declareProperty( "CheckPrimaryMirrorSegments",  m_checkPrimMirrSegs         );
  declareProperty( "CorrectAeroRefract",       m_applyAeroRefractCorr = true  );
  declareProperty( "MinActiveFraction",               m_minActiveFrac         );
  m_minSphMirrTolIt[Rich::Aerogel]  = std::pow( 0.10 * Gaudi::Units::mm, 2 );
  m_minSphMirrTolIt[Rich::Rich1Gas] = std::pow( 0.08 * Gaudi::Units::mm, 2 );
  m_minSphMirrTolIt[Rich::Rich2Gas] = std::pow( 0.05 * Gaudi::Units::mm, 2 );
  declareProperty( "MinSphMirrTolIt", m_minSphMirrTolIt );

  // Corrections for the intrinsic biases
  m_ckBiasCorrs = { -0.000358914, -7.505e-5, -4.287e-5 };

  //setProperty( "OutputLevel", 2 );
}

//=============================================================================
// Destructor
//=============================================================================
PhotonRecoUsingQuarticSoln::~PhotonRecoUsingQuarticSoln() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode PhotonRecoUsingQuarticSoln::initialize()
{
  // initialise base class
  const StatusCode sc = PhotonRecoBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get the detector elements
  m_rich[Rich::Rich1] = getDet<DeRich>( DeRichLocations::Rich1 );
  m_rich[Rich::Rich2] = getDet<DeRich>( DeRichLocations::Rich2 );

  // Get tools
  acquireTool( "RichMirrorSegFinder", m_mirrorSegFinder, nullptr, true );
  acquireTool( "RichRayTracing",      m_rayTracing,      nullptr, true );
  acquireTool( "RichSmartIDTool",     m_idTool, nullptr, true );

  // loop over radiators
  for ( const auto& rad : Rich::radiators() )
  {

    // If rejection of ambiguous photons is turned on make sure test is turned on
    if ( m_rejectAmbigPhots[rad] && !m_testForUnambigPhots[rad] )
    {
      Warning( "Unambigous photon check will be enabled in order to reject ambiguous photons",
               StatusCode::SUCCESS );
      m_testForUnambigPhots[rad] = true;
    }

    // If we are testing for photons that hit the beam pipe, turn on ambig photon test
    if ( m_checkBeamPipe[rad] && !m_testForUnambigPhots[rad] )
    {
      Warning( "Unambigous photon check will be enabled for beampipe check",
               StatusCode::SUCCESS );
      m_testForUnambigPhots[rad] = true;
    }

    // information printout about configuration
    if ( m_testForUnambigPhots[rad] )
    {      _ri_debug << "Will test for unambiguous     " << rad << " photons" << endmsg; }
    else { _ri_debug << "Will not test for unambiguous " << rad << " photons" << endmsg; }

    if ( m_rejectAmbigPhots[rad] )
    {      _ri_debug << "Will reject ambiguous " << rad << " photons" << endmsg; }
    else { _ri_debug << "Will accept ambiguous " << rad << " photons" << endmsg; }

    if ( m_useAlignedMirrSegs[rad] )
    {      _ri_debug << "Will use fully alligned mirror segments for " << rad << " reconstruction" << endmsg;  }
    else { _ri_debug << "Will use nominal mirrors for " << rad << " reconstruction" << endmsg; }

    if ( m_checkBeamPipe[rad] )
    {      _ri_debug << "Will check for " << rad << " photons that hit the beam pipe" << endmsg; }

    if ( m_checkPrimMirrSegs[rad] )
    {      _ri_debug << "Will check for full intersecton with mirror segments for " << rad << endmsg; }

    _ri_debug << "Minimum active " << rad << " segment fraction = " << m_minActiveFrac[rad] << endmsg;

  }

  if ( !m_useSecMirs )
  {
    Warning( "Will ignore secondary mirrors", StatusCode::SUCCESS).ignore();
    // no point doing interations
    m_forceFlatAssumption = true;
  }
  else
  {
    if ( m_forceFlatAssumption )
    {
      Warning( "Assuming perfectly flat secondary mirrors", StatusCode::SUCCESS ).ignore();
    }
    else
    {
      // check iterations
      if ( m_nQits[Rich::Aerogel]  < 1 ) return Error( "# Aerogel  iterations < 1" );
      if ( m_nQits[Rich::Rich1Gas] < 1 ) return Error( "# Rich1Gas iterations < 1" );
      if ( m_nQits[Rich::Rich2Gas] < 1 ) return Error( "# Rich2Gas iterations < 1" );
      _ri_debug << "Assuming spherical secondary mirrors : # iterations (Aero/R1Gas/R2Gas) = "
                << m_nQits << endmsg;
    }
  }

  return sc;
}

//=========================================================================
//  reconstruct a photon from track segment and pixel
//=========================================================================
StatusCode
PhotonRecoUsingQuarticSoln::
reconstructPhoton ( const LHCb::RichRecSegment * segment,
                    const LHCb::RichRecPixel * pixel,
                    LHCb::RichGeomPhoton& gPhoton ) const
{

  // the geometrical track segment
  const LHCb::RichTrackSegment & trSeg  = segment->trackSegment();

  // detection point
  const Gaudi::XYZPoint& detectionPoint = pixel->globalPosition();

  // SmartIDs
  const Rich::HPDPixelCluster& smartIDs = pixel->hpdPixelCluster();

  // Detector information (RICH, radiator and HPD panel)
  const Rich::DetectorType rich     = trSeg.rich();
  const Rich::RadiatorType radiator = trSeg.radiator();
  const Rich::Side         side     = m_rich[rich]->side( detectionPoint );

  // Emission point to use for photon reconstruction
  // operate directly on photon data member for efficiency
  Gaudi::XYZPoint & emissionPoint = gPhoton.emissionPoint();
  emissPoint()->emissionPoint( segment, pixel, emissionPoint );

  // Photon direction at emission point
  // Again, operator directly on data member
  Gaudi::XYZVector & photonDirection = gPhoton.emissionDir();

  // Final reflection points on sec and spherical mirrors
  // operate directly on photon data
  Gaudi::XYZPoint & sphReflPoint = gPhoton.sphMirReflectionPoint();
  Gaudi::XYZPoint & secReflPoint = gPhoton.flatMirReflectionPoint();

  // fraction of segment path length accessible to the photon
  float fraction(1);

  // Pointers to best sec and spherical mirror segments
  const DeRichSphMirror * sphSegment = nullptr;
  const DeRichSphMirror * secSegment = nullptr;

  // flag to say if this photon candidate is un-ambiguous - default to false
  bool unambigPhoton( false );

  // find the reflection of the detection point in the sec mirror
  // (virtual detection point) starting with nominal values
  // At this we are assuming a flat nominal mirror common to all segments
  double distance = m_rich[rich]->nominalPlane(side).Distance(detectionPoint);
  Gaudi::XYZPoint virtDetPoint =
    ( m_useSecMirs ?
      detectionPoint - 2.0 * distance * m_rich[rich]->nominalPlane(side).Normal() :
      detectionPoint
      );

  // --------------------------------------------------------------------------------------
  // For gas radiators, try start and end points to see if photon is unambiguous
  // NOTE : For this we are using the virtual detection point determined using
  // the noimnal flat secondary mirror plane. Now the secondary mirrors are actually
  // spherical this may be introducing some additional uncertainties.
  // --------------------------------------------------------------------------------------
  if ( m_testForUnambigPhots[radiator] )
  {
    if ( UNLIKELY( radiator == Rich::Aerogel ) )
    {
      // use default emission point and assume unambiguous since path length is so short..
      unambigPhoton = true;
    }
    else  // gas radiators
    {

      // flag for beam pipe check. Default is OK
      bool beamTestOK(true);

      // -------------------------------------------------------------------------------
      // First emission point, at start of track segment
      Gaudi::XYZPoint emissionPoint1;
      emissPoint()->emissionPoint( segment, pixel, 0.01, emissionPoint1 );
      // Find mirror segments for this emission point
      const DeRichSphMirror* sphSegment1 = nullptr;
      const DeRichSphMirror* secSegment1 = nullptr;
      Gaudi::XYZPoint sphReflPoint1, secReflPoint1;
      if ( !findMirrorData( rich, side, virtDetPoint, emissionPoint1,
                            sphSegment1, secSegment1, sphReflPoint1, secReflPoint1 ) )
      {
        //_ri_debug << radiator << " : Failed to reconstruct photon for start of segment" << endmsg;
        return StatusCode::FAILURE;
      }
      if ( m_checkBeamPipe[radiator] )
      {
        beamTestOK = !deBeam(rich)->testForIntersection( emissionPoint1,
                                                         sphReflPoint1-emissionPoint1 );
      }
      // -------------------------------------------------------------------------------

      // -------------------------------------------------------------------------------
      // now do it again for emission point #2, at end of segment
      Gaudi::XYZPoint emissionPoint2;
      emissPoint()->emissionPoint( segment, pixel, 0.99, emissionPoint2 );
      // Find mirror segments for this emission point
      const DeRichSphMirror* sphSegment2 = nullptr;
      const DeRichSphMirror* secSegment2 = nullptr;
      Gaudi::XYZPoint sphReflPoint2, secReflPoint2;
      if ( !findMirrorData( rich, side, virtDetPoint, emissionPoint2,
                            sphSegment2, secSegment2, sphReflPoint2, secReflPoint2 ) )
      {
        //_ri_debug << radiator << " : Failed to reconstruct photon for end of segment" << endmsg;
        return StatusCode::FAILURE;
      }
      if ( !beamTestOK && m_checkBeamPipe[radiator] )
      {
        beamTestOK = !deBeam(rich)->testForIntersection( emissionPoint2,
                                                         sphReflPoint2-emissionPoint2 );
      }
      // -------------------------------------------------------------------------------

      // -------------------------------------------------------------------------------
      if ( !beamTestOK )
      {
        // both start and end points failed beam pipe test -> reject
        //_ri_debug << radiator << " : Failed ambiguous photon beampipe intersection checks" << endmsg;
        return StatusCode::FAILURE;
      }
      // -------------------------------------------------------------------------------

      // -------------------------------------------------------------------------------
      // Test to see if photon direction hits the real physical mirror segments
      // -------------------------------------------------------------------------------
      if ( m_checkPrimMirrSegs[radiator] )
      {
        // primary mirrors
        const bool ok = ( sphSegment1->intersects( emissionPoint1,
                                                   sphReflPoint1-emissionPoint1 ) ||
                          sphSegment2->intersects( emissionPoint2,
                                                   sphReflPoint2-emissionPoint2 ) );
        if ( !ok )
        {
          //_ri_debug << radiator << " : Failed mirror segment intersection checks" << endmsg;
          return StatusCode::FAILURE;
        }
      }
      // -------------------------------------------------------------------------------

      // -------------------------------------------------------------------------------
      // Get the best gas emission point
      if ( !getBestGasEmissionPoint( radiator,
                                     sphReflPoint1,
                                     sphReflPoint2,
                                     detectionPoint,
                                     segment, pixel,
                                     emissionPoint,
                                     fraction ) )
      {
        //_ri_debug << radiator << " : Failed to compute best gas emission point" << endmsg;
        return StatusCode::FAILURE;
      }
      // -------------------------------------------------------------------------------

      // -------------------------------------------------------------------------------
      // Is this an unambiguous photon - I.e. only one possible mirror combination
      if ( (sphSegment1 == sphSegment2) && (secSegment1 == secSegment2) )
      {
        // Set pointers to the mirror detector objects
        sphSegment = sphSegment1;
        secSegment = secSegment1;
        // rough guesses at reflection points (improved later on)
        sphReflPoint = sphReflPoint1 + 0.5*(sphReflPoint2-sphReflPoint1);
        secReflPoint = secReflPoint1 + 0.5*(secReflPoint2-secReflPoint1);
        // photon is not unambiguous
        unambigPhoton = true;
      }
      // -------------------------------------------------------------------------------

    } // end radiator type if

    // if configured to do so reject ambiguous photons
    if ( m_rejectAmbigPhots[radiator] && !unambigPhoton )
    {
      //_ri_debug << radiator << " : Failed ambiguous photon test" << endmsg;
      return StatusCode::FAILURE;
    }

  } // end unambiguous photon check
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // Active segment fraction cut
  // --------------------------------------------------------------------------------------
  if ( fraction < m_minActiveFrac[radiator] )
  {
    //_ri_debug << radiator << " : Failed active segment fraction cut" << endmsg;
    return StatusCode::FAILURE;
  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // if aerogel (which hasn't yet been treated at all) or ambiguous gas photon, or if
  // ambiguous photon check above has been skipped, try again
  // using best emission point and nominal mirror geometries to get the spherical and sec
  // mirrors. Also, force this reconstruction if the above unambiguous test was skipped
  // --------------------------------------------------------------------------------------
  if ( UNLIKELY( !m_testForUnambigPhots[radiator] || 
                 Rich::Aerogel == radiator        || 
                 !unambigPhoton ) )
  {
    if ( !findMirrorData(rich,side,
                         virtDetPoint,emissionPoint,
                         sphSegment,secSegment,sphReflPoint,secReflPoint ) )
    {
      //_ri_debug << radiator << " : Failed backup photon reconstruction" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // Finally reconstruct the photon using best emission point and the best mirror segments
  // --------------------------------------------------------------------------------------
  if ( m_useAlignedMirrSegs[radiator] )
  {
    if ( UNLIKELY(m_forceFlatAssumption) )
    {
      // assume secondary mirrors are perfectly flat

      if ( m_useSecMirs )
      {
        distance     = secSegment->centreNormalPlane().Distance(detectionPoint);
        virtDetPoint = detectionPoint - 2.0 * distance * secSegment->centreNormal();
      }

      // solve the quartic
      m_quarticSolver.solve<double>( emissionPoint,
                                     sphSegment->centreOfCurvature(),
                                     virtDetPoint,
                                     sphSegment->radius(),
                                     sphReflPoint );

    }
    else
    {
      // Default mode. Use full spherical nature of secondaries

      // Iterate to final solution, improving the secondary mirror info
      int iIt(0);
      Gaudi::XYZPoint last_mirror_point(0,0,0);
      while ( iIt < m_nQits[radiator] )
      {

        // Get secondary mirror reflection point,
        // using the best actual secondary mirror segment at this point
        const bool sc = m_rayTracing->intersectPlane( sphReflPoint,
                                                      virtDetPoint - sphReflPoint,
                                                      secSegment->centreNormalPlane(),
                                                      secReflPoint );
        if ( !sc )
        {
          //_ri_debug << radiator << " : Failed to intersect nominal secondary mirror plane" << endmsg;
          return StatusCode::FAILURE;
        }
        // (re)find the secondary mirror
        secSegment = m_mirrorSegFinder->findSecMirror( rich, side, secReflPoint );

        // Construct plane tangential to secondary mirror passing through reflection point
        const Gaudi::Plane3D plane( secSegment->centreOfCurvature() - secReflPoint,
                                    secReflPoint );

        // re-find the reflection of the detection point in the sec mirror
        // (virtual detection point) with this mirror plane
        distance     = plane.Distance(detectionPoint);
        virtDetPoint = detectionPoint - 2.0 * distance * plane.Normal();

        // solve the quartic using the new data
        m_quarticSolver.solve<double>( emissionPoint,
                                       sphSegment->centreOfCurvature(),
                                       virtDetPoint,
                                       sphSegment->radius(),
                                       sphReflPoint );

        // (re)find the spherical mirror segment
        sphSegment = m_mirrorSegFinder->findSphMirror( rich, side, sphReflPoint );

        // for iterations after the first, see if we are still moving
        // If not, abort iterations early
        if ( iIt > 0 )
        {
          if ( (last_mirror_point-secReflPoint).Mag2() < m_minSphMirrTolIt[radiator] ) break;
        }

        // store last sec mirror point
        last_mirror_point = secReflPoint;

        // increment iteration counter
        ++iIt;
      }

    } // end mirror type if
  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // check that spherical mirror reflection point is on the same side as detection point
  // and (if configured to do so) photon does not cross between detector sides
  // --------------------------------------------------------------------------------------
  if ( UNLIKELY( !sameSide(radiator,sphReflPoint,virtDetPoint) ) )
  {
    //_ri_debug << radiator << " : Reflection point on wrong side" << endmsg;
    return StatusCode::FAILURE;
  }
  if ( m_checkPhotCrossSides[radiator] && !sameSide(radiator,sphReflPoint,emissionPoint) )
  {
    //_ri_debug << radiator << " : Photon crosses between detector sides" << endmsg;
    return StatusCode::FAILURE;
  }
  //else { return StatusCode::FAILURE; } // uncomment to select ONLY crossing photons
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // For aerogel, and gas radiators if ambiguous photon checks are disabled (since this is
  // already done for these photons during those checks), check if the photon would have
  // intersected with the beampipe
  // --------------------------------------------------------------------------------------
  if ( UNLIKELY( m_checkBeamPipe[radiator] &&
                 ( radiator == Rich::Aerogel || !m_testForUnambigPhots[radiator] ) ) )
  {
    if ( UNLIKELY( deBeam(rich)->testForIntersection( emissionPoint,
                                                      sphReflPoint-emissionPoint ) ) )
    {
      //_ri_debug << radiator << " : Failed final beampipe intersection checks" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // If using aligned mirror segments, get the final sec mirror reflection
  // point using the best mirror segments available at this point
  // --------------------------------------------------------------------------------------
  if ( m_useSecMirs && m_useAlignedMirrSegs[radiator] )
  {
    const bool sc = m_rayTracing->intersectPlane( sphReflPoint,
                                                  virtDetPoint - sphReflPoint,
                                                  secSegment->centreNormalPlane(),
                                                  secReflPoint );
    if ( !sc )
    {
      //_ri_debug << radiator << " : Failed final secondary mirror plane intersection" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // calculate the cherenkov angles using the photon and track vectors
  // --------------------------------------------------------------------------------------
  photonDirection = (sphReflPoint-emissionPoint).Unit();
  double thetaCerenkov(0), phiCerenkov(0);
  trSeg.angleToDirection( photonDirection, thetaCerenkov, phiCerenkov );
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // Correct Cherenkov theta for refraction at exit of aerogel
  // --------------------------------------------------------------------------------------
  if ( UNLIKELY( Rich::Aerogel == radiator && m_applyAeroRefractCorr ) )
  {
    correctAeroRefraction( trSeg, photonDirection, thetaCerenkov );
  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // Test final data to see if photon direction hits the real physical mirror segments
  // --------------------------------------------------------------------------------------
  //if ( m_checkPrimMirrSegs[radiator] )
  //{
  //  // primary mirrors
  //  const bool ok = ( sphSegment->intersects(emissionPoint,photonDirection) );
  //  if ( !ok ) return StatusCode::FAILURE;
  //}
  // --------------------------------------------------------------------------------------

  //---------------------------------------------------------------------------------------
  // Apply fudge factor correction for small biases in CK theta
  //---------------------------------------------------------------------------------------
  thetaCerenkov += ckThetaCorrection(radiator);
  //---------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // Set (remaining) photon parameters
  // --------------------------------------------------------------------------------------
  gPhoton.setCherenkovTheta         ( (float)(thetaCerenkov) );
  gPhoton.setCherenkovPhi           ( (float)(phiCerenkov)   );
  gPhoton.setActiveSegmentFraction  ( fraction               );
  gPhoton.setDetectionPoint         ( detectionPoint         );
  gPhoton.setSmartID                ( smartIDs.primaryID()   );
  gPhoton.setUnambiguousPhoton      ( unambigPhoton          );
  gPhoton.setPrimaryMirror          ( sphSegment             );
  gPhoton.setSecondaryMirror        ( secSegment             );
  //_ri_verbo << "Created photon " << gPhoton << endmsg;
  // --------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;
}

//=========================================================================
// Find mirror segments and reflection points for given data
//=========================================================================
bool
PhotonRecoUsingQuarticSoln::
findMirrorData( const Rich::DetectorType rich,
                const Rich::Side side,
                const Gaudi::XYZPoint& virtDetPoint,
                const Gaudi::XYZPoint& emissionPoint,
                const DeRichSphMirror*& sphSegment,
                const DeRichSphMirror*& secSegment,
                Gaudi::XYZPoint& sphReflPoint,
                Gaudi::XYZPoint& secReflPoint ) const
{
  // solve quartic equation with nominal values and find spherical mirror reflection point
  m_quarticSolver.solve<double>( emissionPoint,
                                 m_rich[rich]->nominalCentreOfCurvature(side),
                                 virtDetPoint,
                                 m_rich[rich]->sphMirrorRadius(),
                                 sphReflPoint );
  // return status
  bool OK = true;
  // find the spherical mirror segment
  sphSegment = m_mirrorSegFinder->findSphMirror( rich, side, sphReflPoint );
  // If we are using full secondary mirrors, search for the secondary segment
  if ( m_useSecMirs )
  {
    // find the sec mirror intersction point and secondary mirror segment
    OK = m_rayTracing->intersectPlane( sphReflPoint,
                                       virtDetPoint - sphReflPoint,
                                       m_rich[rich]->nominalPlane(side),
                                       secReflPoint );
    if ( OK )
    {
      // find the secondary mirror
      secSegment = m_mirrorSegFinder->findSecMirror( rich, side, secReflPoint );
    }
  }
  // return
  return OK;
}

//=========================================================================
// Correct Aerogel Cherenkov angle theta for refraction at exit of aerogel
//=========================================================================
void
PhotonRecoUsingQuarticSoln::
correctAeroRefraction( const LHCb::RichTrackSegment& trSeg,
                       Gaudi::XYZVector& photonDirection,
                       double & thetaCerenkov ) const
{
  // apply Snell's Law
  snellsLaw()->gasToAerogel( photonDirection, trSeg );
  // update CK theta
  const double ctc = photonDirection.Dot( trSeg.bestMomentum().Unit() );
  thetaCerenkov = ( ctc>1 ? 0 : vdt::fast_acos(ctc) );
}

//=========================================================================
// Compute the best emission point for the gas radiators using
// the given spherical mirror reflection points
//=========================================================================
bool
PhotonRecoUsingQuarticSoln::
getBestGasEmissionPoint( const Rich::RadiatorType radiator,
                         const Gaudi::XYZPoint& sphReflPoint1,
                         const Gaudi::XYZPoint& sphReflPoint2,
                         const Gaudi::XYZPoint& detectionPoint,
                         const LHCb::RichRecSegment * segment,
                         const LHCb::RichRecPixel * pixel,
                         Gaudi::XYZPoint & emissionPoint,
                         float & fraction ) const
{
  double alongTkFrac = 0.5;

  if ( radiator == Rich::Rich1Gas )
  {
    // First reflection and hit point on same y side ?
    const bool sameSide1 = ( sphReflPoint1.y() * detectionPoint.y() > 0 );
    const bool sameSide2 = ( sphReflPoint2.y() * detectionPoint.y() > 0 );
    if ( sameSide1 && sameSide2 )
    {
      emissPoint()->emissionPoint( segment, pixel, emissionPoint );
    }
    else if ( sameSide1 )
    {
      fraction = (float)(std::fabs(sphReflPoint1.y()/(sphReflPoint1.y()-sphReflPoint2.y())));
      alongTkFrac = fraction/2.0;
      emissPoint()->emissionPoint( segment, pixel, alongTkFrac, emissionPoint );
    }
    else if ( sameSide2 )
    {
      fraction = (float)(std::fabs(sphReflPoint2.y()/(sphReflPoint1.y()-sphReflPoint2.y())));
      alongTkFrac = 1.0-fraction/2.0;
      emissPoint()->emissionPoint( segment, pixel, alongTkFrac, emissionPoint );
    }
    else
    {
      //Warning( "Rich1Gas : Both RICH spherical mirror hits opposite side to detection point" );
      return false;
    }

  }
  else if ( radiator == Rich::Rich2Gas )
  {
    // First sphReflPoint and hit point on same x side ?
    const bool sameSide1 = ( sphReflPoint1.x() * detectionPoint.x() > 0 );
    const bool sameSide2 = ( sphReflPoint2.x() * detectionPoint.x() > 0 );
    if ( sameSide1 && sameSide2 )
    {
      emissPoint()->emissionPoint( segment, pixel, emissionPoint );
    }
    else if ( sameSide1 )
    {
      fraction = (float)(std::fabs(sphReflPoint1.x()/(sphReflPoint1.x()-sphReflPoint2.x())));
      alongTkFrac = fraction/2.0;
      emissPoint()->emissionPoint( segment, pixel, alongTkFrac, emissionPoint );
    }
    else if ( sameSide2 )
    {
      fraction = (float)(std::fabs(sphReflPoint2.x()/(sphReflPoint1.x()-sphReflPoint2.x())));
      alongTkFrac = 1.0-fraction/2.0;
      emissPoint()->emissionPoint( segment, pixel, alongTkFrac, emissionPoint );
    }
    else
    {
      //Warning( "Rich2Gas : Both RICH spherical mirror hits opposite side to detection point" );
      return false;
    }
  }
  else { Error( "::getBestGasEmissionPoint() called for Aerogel segment !!" ); }

  // if ( msgLevel(MSG::VERBOSE) )
  // {
  //   verbose() << radiator << " best emission point correction :- " << endmsg
  //             << " -> Photon detection point = " << detectionPoint << endmsg
  //             << " -> Sph. Mirror ptns       = " << sphReflPoint1 << " " << sphReflPoint2 << endmsg
  //             << " -> Segment entry/exit     = " << trSeg.entryPoint() << " " << trSeg.exitPoint() << endmsg
  //             << " -> Segment fraction       = " << fraction << endmsg
  //             << " -> Emm. Ptn. Along traj   = " << alongTkFrac << endmsg
  //             << " -> Best Emission point    = " << emissionPoint << endmsg;
  // }

  return true;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY ( PhotonRecoUsingQuarticSoln )

//=============================================================================
