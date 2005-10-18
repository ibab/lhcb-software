
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoUsingQuarticSoln.cpp
 *
 * Implementation file for class : RichPhotonRecoUsingQuarticSoln
 *
 * CVS Log :-
 * $Id: RichPhotonRecoUsingQuarticSoln.cpp,v 1.2 2005-10-18 13:04:28 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @author Antonis Papanestis
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonRecoUsingQuarticSoln.h"

// Declaration of the Algorithm Factory
static const  ToolFactory<RichPhotonRecoUsingQuarticSoln>          Factory ;
const        IToolFactory& RichPhotonRecoUsingQuarticSolnFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichPhotonRecoUsingQuarticSoln::
RichPhotonRecoUsingQuarticSoln( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : RichToolBase      ( type, name, parent ),
    m_mirrorSegFinder ( 0 ),
    m_rayTracing      ( 0 ),
    m_idTool          ( 0 ),
    m_refIndex        ( 0 )
{

  // declare interface
  declareInterface<IRichPhotonReconstruction>(this);

  // job options

  declareProperty( "FindUnambiguousPhotons",    m_testForUnambigPhots = true );
  declareProperty( "UseMirrorSegmentAllignment", m_useAlignedMirrSegs = true );

}

//=============================================================================
// Destructor
//=============================================================================
RichPhotonRecoUsingQuarticSoln::~RichPhotonRecoUsingQuarticSoln() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode RichPhotonRecoUsingQuarticSoln::initialize()
{
  // initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get the detector elements
  const DeRich * r1 = getDet<DeRich>( DeRichLocation::Rich1 );
  const DeRich * r2 = getDet<DeRich>( DeRichLocation::Rich2 );
  m_rich[Rich::Rich1] = r1;
  m_rich[Rich::Rich2] = r2;

  // Get tools
  acquireTool( "RichMirrorSegFinder", m_mirrorSegFinder );
  acquireTool( "RichRayTracing",      m_rayTracing      );
  acquireTool( "RichSmartIDTool",     m_idTool, 0, true );
  acquireTool( "RichRefractiveIndex", m_refIndex        );

  // load the nominal centre of curvature
  m_nominalCoC[Rich::Rich1][Rich::top]    = r1->nominalCentreOfCurvature(Rich::top);
  m_nominalCoC[Rich::Rich1][Rich::bottom] = r1->nominalCentreOfCurvature(Rich::bottom);
  m_nominalCoC[Rich::Rich2][Rich::left]   = r2->nominalCentreOfCurvature(Rich::left);
  m_nominalCoC[Rich::Rich2][Rich::right]  = r2->nominalCentreOfCurvature(Rich::right);

  // load the nominal flat mirror planes
  m_nominalFlatMirrorPlane[Rich::Rich1][Rich::top]    = r1->nominalPlane(Rich::top);
  m_nominalFlatMirrorPlane[Rich::Rich1][Rich::bottom] = r1->nominalPlane(Rich::bottom);
  m_nominalFlatMirrorPlane[Rich::Rich2][Rich::left]   = r2->nominalPlane(Rich::left);
  m_nominalFlatMirrorPlane[Rich::Rich2][Rich::right]  = r2->nominalPlane(Rich::right);

  // load the nominal radii of curvature
  m_nomSphMirrorRadius[Rich::Rich1] = r1->sphMirrorRadius();
  m_nomSphMirrorRadius[Rich::Rich2] = r2->sphMirrorRadius();

  if ( m_testForUnambigPhots )
  {
    info() << "Will test for unambiguous photons" << endreq;
  }
  else
  {
    info() << "Will not test for unambiguous photons" << endreq;
  }
  if ( m_useAlignedMirrSegs )
  {
    info() << "Will use fully alligned mirror segments for reconstruction" << endreq;
  }
  else
  {
    info() << "Will use nominal mirrors for reconstruction" << endreq;
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichPhotonRecoUsingQuarticSoln::finalize()
{
  return RichToolBase::finalize();
}

//=========================================================================
//  reconstruct a photon from track segment and smartID
//=========================================================================
StatusCode
RichPhotonRecoUsingQuarticSoln::reconstructPhoton ( const RichTrackSegment& trSeg,
                                                    const RichSmartID& smartID,
                                                    RichGeomPhoton& gPhoton ) const
{
  return reconstructPhoton( trSeg, m_idTool->globalPosition(smartID), gPhoton );
}

//-------------------------------------------------------------------------
//  reconstruct a photon from track segment and detection point
//-------------------------------------------------------------------------
StatusCode
RichPhotonRecoUsingQuarticSoln::reconstructPhoton ( const RichTrackSegment& trSeg,
                                                    const HepPoint3D& detectionPoint,
                                                    RichGeomPhoton& gPhoton ) const
{

  // Detector information (RICH, radiator and HPD panel)
  const Rich::DetectorType rich     = trSeg.rich();
  const Rich::RadiatorType radiator = trSeg.radiator();
  const Rich::Side side             = m_rich[rich]->side( detectionPoint );

  // Emission point to use for photon reconstruction
  // operate directly on photon data member for efficiency
  // Starting value is the "best" point on the track segment
  HepPoint3D & emissionPoint = gPhoton.emissionPoint();
  emissionPoint = trSeg.bestPoint();

  // fraction of segment path length accessible to the photon
  double fraction(1);

  // Pointers to best flat and spherical mirror segments
  const DeRichSphMirror  * sphSegment  = 0;
  const DeRichFlatMirror * flatSegment = 0;

  // flag to say if this photon candidate is un-ambiguous - default to false
  bool unambigPhoton( false );

  // find the reflection of the detection point in the flat mirror
  // (virtual detection point) starting with nominal values
  double distance = m_nominalFlatMirrorPlane[rich][side].distance(detectionPoint);
  HepPoint3D virtDetPoint =
    ( detectionPoint - 2.0 * distance * m_nominalFlatMirrorPlane[rich][side].normal() );

  // --------------------------------------------------------------------------------------
  // For gas radiators, try start and end points to see if photon is unambiguous
  // --------------------------------------------------------------------------------------
  if ( m_testForUnambigPhots )
  {
    if ( radiator == Rich::Aerogel )
    {
      // use default emission point and assume unambiguous since path length is so short..
      unambigPhoton = true;
    }
    else  // gas radiators
    {

      // First emission point, at start of track segment
      const HepPoint3D emissionPoint1 = trSeg.bestPoint( 0.01 );

      // Find mirror segments for this emission point
      const DeRichSphMirror* sphSegment1   = 0;
      const DeRichFlatMirror* flatSegment1 = 0;
      HepPoint3D sphReflPoint1, flatReflPoint1;
      if ( !findMirrorData( rich, side, virtDetPoint, emissionPoint1,
                            sphSegment1, flatSegment1, sphReflPoint1, flatReflPoint1 ) )
      {
        //return Warning( "Failed to reconstruct photon for start of segment" );
        return StatusCode::FAILURE;
      }

      // now do it again for emission point #2, at end of segment
      const HepPoint3D emissionPoint2 = trSeg.bestPoint( 0.99 );

      // Find mirror segments for this emission point
      const DeRichSphMirror* sphSegment2   = 0;
      const DeRichFlatMirror* flatSegment2 = 0;
      HepPoint3D sphReflPoint2, flatReflPoint2;
      if ( !findMirrorData( rich, side, virtDetPoint, emissionPoint2,
                            sphSegment2, flatSegment2, sphReflPoint2, flatReflPoint2 ) )
      {
        //return Warning( "Failed to reconstruct photon for end of segment" );
        return StatusCode::FAILURE;
      }

      // Get gas emission point
      if ( !getBestGasEmissionPoint( radiator, sphReflPoint1, sphReflPoint2,
                                     detectionPoint, trSeg,
                                     emissionPoint, fraction ) )
      {
        return StatusCode::FAILURE;
      }

      // Is this an unambiguous photon - I.e. only one possible mirror combination
      if ( (sphSegment1 == sphSegment2) && (flatSegment1 == flatSegment2) )
      {
        // Set pointers to the mirror detector objects
        sphSegment  = sphSegment1;
        flatSegment = flatSegment1;
        // photon is not unambiguous
        unambigPhoton = true;
      }

    } // end radiator type if

  } // end do test if
  // --------------------------------------------------------------------------------------

  // Final reflection points on flat and spherical mirrors
  // operate directly on photon data
  HepPoint3D & sphReflPoint  = gPhoton.sphMirReflectionPoint();
  HepPoint3D & flatReflPoint = gPhoton.flatMirReflectionPoint();

  // --------------------------------------------------------------------------------------
  // if aerogel or ambiguous gas photon, try again using best emission point and
  // nominal mirror geometries to get the spherical and flat mirrors
  // Also, force this reconstruction if the above unambiguous test was skipped
  // --------------------------------------------------------------------------------------
  if ( !m_testForUnambigPhots || Rich::Aerogel == radiator || !unambigPhoton )
  {

    if ( !solveQuarticEq( emissionPoint,
                          m_nominalCoC[rich][side],
                          virtDetPoint,
                          m_nomSphMirrorRadius[rich],
                          sphReflPoint ) )
    {
      //return Warning( "Failed to reconstruct photon using nominal mirrors" );
      return StatusCode::FAILURE;
    }

    // Get flat mirror reflection point
    m_rayTracing->intersectPlane( sphReflPoint,
                                  virtDetPoint - sphReflPoint,
                                  m_nominalFlatMirrorPlane[rich][side],
                                  flatReflPoint);

    // Get pointers to the spherical and flat mirror detector objects
    flatSegment = m_mirrorSegFinder->findFlatMirror ( rich, side, flatReflPoint );
    sphSegment  = m_mirrorSegFinder->findSphMirror  ( rich, side, sphReflPoint  );

  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // Finally, reconstruct the photon using best emission point and
  // the best mirror segments
  // --------------------------------------------------------------------------------------
  if ( m_useAlignedMirrSegs )
  {

    // re-find the reflection of the detection point in the flat mirror
    // (virtual detection point) with the final (best) detector information
    distance     = flatSegment->mirrorPlane().distance(detectionPoint);
    virtDetPoint = detectionPoint - 2.0 * distance * flatSegment->mirrorPlane().normal();

    // solve the quartic
    if ( !solveQuarticEq( emissionPoint,
                          sphSegment->centreOfCurvature(),
                          virtDetPoint,
                          sphSegment->radius(),
                          sphReflPoint ) )
    {
      //return Warning( "Failed to reconstruct photon using mirror segments" );
      return StatusCode::FAILURE;
    }

  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // check that spherical mirror reflection point is on the same side as track point
  // --------------------------------------------------------------------------------------
  if ( radiator == Rich::CF4 )
  {
    if ( sphReflPoint.x() * virtDetPoint.x() < 0.0 )
    {
      //return Warning( "RICH2 : Reflection point on wrong side" );
      return StatusCode::FAILURE;
    }
  }
  else
    // RICH 1
    if ( sphReflPoint.y() * virtDetPoint.y() < 0.0 )
    {
      //return Warning( "RICH1 : Reflection point on wrong side" );
      return StatusCode::FAILURE;
    }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // If using alligned mirror segments, get the final flat mirror reflection
  // point using the best mirror segments available at this point
  // --------------------------------------------------------------------------------------
  if ( m_useAlignedMirrSegs )
  {
    m_rayTracing->intersectPlane( sphReflPoint,
                                  virtDetPoint - sphReflPoint,
                                  flatSegment->mirrorPlane(),
                                  flatReflPoint );
  }
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // calculate the cherenkov angles
  // --------------------------------------------------------------------------------------
  HepVector3D photonDirection = sphReflPoint - emissionPoint;
  double thetaCerenkov, phiCerenkov;
  trSeg.angleToDirection( photonDirection, thetaCerenkov, phiCerenkov );
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // Correct Cherenkov theta for refraction at exit of aerogel
  // --------------------------------------------------------------------------------------
  if ( Rich::Aerogel == radiator )
    correctAeroRefraction( trSeg, photonDirection, thetaCerenkov );
  // --------------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------------
  // Set (remaining) photon parameters
  // --------------------------------------------------------------------------------------
  gPhoton.setCherenkovTheta         ( static_cast<float>(thetaCerenkov) );
  gPhoton.setCherenkovPhi           ( static_cast<float>(phiCerenkov)   );
  gPhoton.setActiveSegmentFraction  ( static_cast<float>(fraction)      );
  gPhoton.setDetectionPoint         ( detectionPoint );
  gPhoton.setMirrorNumValid         ( unambigPhoton  );
  gPhoton.setSphMirrorNum           ( sphSegment  ? sphSegment->mirrorNumber()  : 0 );
  gPhoton.setFlatMirrorNum          ( flatSegment ? flatSegment->mirrorNumber() : 0 );
  // --------------------------------------------------------------------------------------

  return StatusCode::SUCCESS;
}

//=========================================================================
// Find mirror segments and reflection points for given data
//=========================================================================
bool
RichPhotonRecoUsingQuarticSoln::findMirrorData( const Rich::DetectorType rich,
                                                const Rich::Side side,
                                                const HepPoint3D & virtDetPoint,
                                                const HepPoint3D & emissionPoint,
                                                const DeRichSphMirror *& sphSegment,
                                                const DeRichFlatMirror *& flatSegment,
                                                HepPoint3D & sphReflPoint,
                                                HepPoint3D & flatReflPoint ) const
{
  // solve quartic equation with nominal values and find spherical mirror reflection point
  if ( !solveQuarticEq( emissionPoint,
                        m_nominalCoC[rich][side],
                        virtDetPoint,
                        m_nomSphMirrorRadius[rich],
                        sphReflPoint ) ) { return false; }

  // find the spherical mirror segment
  sphSegment = m_mirrorSegFinder->findSphMirror(rich, side, sphReflPoint );

  // find the flat mirror intersction point and flat mirror segment
  m_rayTracing->intersectPlane( sphReflPoint,
                                virtDetPoint - sphReflPoint,
                                m_nominalFlatMirrorPlane[rich][side],
                                flatReflPoint);
  flatSegment = m_mirrorSegFinder->findFlatMirror(rich, side, flatReflPoint );

  return true;
}

//=========================================================================
// Compute the best emission point for the gas radiators using
// the given spherical mirror reflection points
//=========================================================================
bool RichPhotonRecoUsingQuarticSoln::
getBestGasEmissionPoint( const Rich::RadiatorType radiator,
                         const HepPoint3D & sphReflPoint1,
                         const HepPoint3D & sphReflPoint2,
                         const HepPoint3D & detectionPoint,
                         const RichTrackSegment& trSeg,
                         HepPoint3D & emissionPoint,
                         double & fraction ) const
{

  if ( radiator == Rich::C4F10 )
  {
    // First reflection and hit point on same y side ?
    const bool sameSide1 = ( sphReflPoint1.y() * detectionPoint.y() > 0 );
    const bool sameSide2 = ( sphReflPoint2.y() * detectionPoint.y() > 0 );
    if ( sameSide1 && sameSide2 )
    {
      emissionPoint = trSeg.bestPoint();
    }
    else if ( sameSide1 )
    {
      fraction = fabs( sphReflPoint1.y() / (sphReflPoint1.y() - sphReflPoint2.y()) );
      emissionPoint = trSeg.bestPoint( fraction/2 );
    }
    else if ( sameSide2 )
    {
      fraction = fabs( sphReflPoint2.y() / (sphReflPoint1.y() - sphReflPoint2.y()) );
      emissionPoint = trSeg.bestPoint( 1-fraction/2 );
    }
    else
    {
      //Warning( "C4F10 : Both RICH hits opposite side to track hit" );
      return false;
    }

  }
  else
  {
    // First sphReflPoint and hit point on same x side ?
    const bool sameSide1 = ( sphReflPoint1.x() * detectionPoint.x() > 0 );
    const bool sameSide2 = ( sphReflPoint2.x() * detectionPoint.x() > 0 );
    if ( sameSide1 && sameSide2 )
    {
      emissionPoint = trSeg.bestPoint();
    }
    else if ( sameSide1 )
    {
      fraction = fabs( sphReflPoint1.x()/(sphReflPoint1.x()-sphReflPoint2.x()) );
      emissionPoint = trSeg.bestPoint( fraction/2 );
    }
    else if ( sameSide2 )
    {
      fraction = fabs( sphReflPoint2.x()/(sphReflPoint1.x()-sphReflPoint2.x()) );
      emissionPoint = trSeg.bestPoint( 1-fraction/2 );
    }
    else
    {
      //Warning( "CF4 : Both RICH hits opposite side to track hit" );
      return false;
    }
  }

  return true;
}

//=========================================================================
// Correct Aerogel Cherenkov angle theta for refraction at exit of aerogel
//=========================================================================
void
RichPhotonRecoUsingQuarticSoln::correctAeroRefraction( const RichTrackSegment& trSeg,
                                                       HepVector3D& photonDirection,
                                                       double & thetaCerenkov ) const
{
  photonDirection.setMag(1);
  const double refAero  = m_refIndex->refractiveIndex( Rich::Aerogel, trSeg.avPhotonEnergy() );
  const double refc4f10 = m_refIndex->refractiveIndex( Rich::C4F10,   trSeg.avPhotonEnergy() );
  const double RratioSq = (refAero*refAero)/(refc4f10*refc4f10);
  HepVector3D newDir;
  newDir.setZ( sqrt(1. - ( 1. - photonDirection.z()*photonDirection.z() )/RratioSq ));
  const double R = photonDirection.y()/photonDirection.x();
  newDir.setX( sqrt( (1.-newDir.z()*newDir.z())/(1. + R*R) ) );
  if ( photonDirection.x() < 0 ) newDir.setX( -newDir.x() );
  newDir.setY( R*newDir.x() );
  const double ctc = newDir.dot( trSeg.bestMomentum().unit() );
  thetaCerenkov = ( ctc>1 ? 0 : acos(ctc) );
}

//=========================================================================
//  setup and solve quartic equation
//=========================================================================
bool
RichPhotonRecoUsingQuarticSoln::solveQuarticEq (const HepPoint3D& emissionPoint,
                                                const HepPoint3D& CoC,
                                                const HepPoint3D& virtDetPoint,
                                                const double radius,
                                                HepPoint3D& sphReflPoint) const
{

  // solve quartic in the form  x^4 + a x^3 + b x^2 + c x + d = 0

  // vector from mirro centre of curvature to assumed emission point
  HepVector3D evec = emissionPoint - CoC;
  const double e2 = evec.mag2();
  if ( 0 == e2 )  { return false; }

  // vector from mirror centre of curvature to virtual detection point
  const HepVector3D dvec = virtDetPoint - CoC;
  const double d2 = dvec.mag2();
  if ( 0 == d2 )  { return false; }

  // various quantities needed to create quartic equation
  // see LHCB/98-040 section 3, equation 3
  const double e     = sqrt(e2);
  const double d     = sqrt(d2);
  const double gamma = evec.angle(dvec);
  const double dx    = d * cos(gamma);
  const double dy    = d * sin(gamma);
  const double r2    = radius * radius;

  // Fill array for quartic equation
  double a[5];
  a[0] =    4 * e2 * d2;
  a[1] =  - 4 * e2 * dy * radius;
  a[2] =   (dy * dy * r2) + ((e+dx) * (e+dx) * r2) - a[0] ;
  a[3] =   2 * e * dy * (e-dx) * radius;
  a[4] =   ( e2 - r2 ) * dy * dy ;

  /*
  // -----------------------------------------------------------------------
  // use full 'GSL' function
  gsl_complex solutions[4];
  if ( 0 == gsl_poly_complex_solve_quartic( a[1]/a[0], // a
                                            a[2]/a[0], // b
                                            a[3]/a[0], // c
                                            a[4]/a[0], // d
                                            &solutions[0],
                                            &solutions[1],
                                            &solutions[2],
                                            &solutions[3] ) ) { return false; }

  // normal vector to reflection plane
  const HepVector3D nvec1 = evec.cross(dvec);

  // get the required results
  HepVector3D delta[2];
  int j = 0;
  for ( int i = 0; i<4 && j<2; ++i )
  {
    if ( 0 == GSL_IMAG(solutions[i]) )
    {
      delta[j] = evec;
      delta[j].setMag(radius);
      delta[j++].rotate( asin(GSL_REAL(solutions[i])), nvec1);
    }
  }

  // Finally, form the reflection point on the spherical mirror
  sphReflPoint = ( delta[0].z() > delta[1].z() ? CoC + delta[0] : CoC + delta[1] );

  // -----------------------------------------------------------------------
  */

  // -----------------------------------------------------------------------
  // use 'hacked' RICH version
  const double sinbeta = solve_quartic_RICH( a[1]/a[0], // a
                                             a[2]/a[0], // b
                                             a[3]/a[0], // c
                                             a[4]/a[0]  // d
                                             );

  // normal vector to reflection plane
  const HepVector3D nvec2 = evec.cross(dvec);

  // use results to form reflection point
  evec.setMag( radius );
  evec.rotate( asin(sinbeta), nvec2 );
  sphReflPoint = CoC + evec;

  // -----------------------------------------------------------------------

  return true;
}
