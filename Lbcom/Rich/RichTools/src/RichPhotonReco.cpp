// $Id: RichPhotonReco.cpp,v 1.1.1.1 2004-06-17 12:04:09 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/GaudiException.h"

// LHCbKernel
#include "Kernel/RichSide.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"

#include "RichKernel/IRichRefractiveIndex.h"

// local
#include "RichPhotonReco.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPhotonReco
//
// 2003-11-14 Antonis Papanestis
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  ToolFactory<RichPhotonReco>          Factory ;
const        IToolFactory& RichPhotonRecoFactory = Factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichPhotonReco::RichPhotonReco( const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : RichToolBase ( type, name, parent ),
    m_mirrorSegFinder ( 0 ),
    m_rayTracing      ( 0 ),
    m_idTool          ( 0 )
{

  declareInterface<IRichPhotonReconstruction>(this);

  //  Nominal z positions of states at RICHes
  //  m_nomZstates.push_back( 99.0*cm   );
  //  m_nomZstates.push_back( 216.5*cm  );
  //  m_nomZstates.push_back( 945.0*cm  );
  //  m_nomZstates.push_back( 1190.0*cm );

}

//=============================================================================
// Destructor
//=============================================================================
RichPhotonReco::~RichPhotonReco() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode RichPhotonReco::initialize() {

  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  DeRich1 * rich1 = getDet<DeRich1>( DeRich1Location::Default );
  DeRich2 * rich2 = getDet<DeRich2>( DeRich2Location::Default );

  // load the nominal centre of curvature and flat mirror plane
  m_nominalCoC[Rich::Rich1][Rich::top] = rich1->
    nominalCentreOfCurvature(Rich::top);
  m_nominalCoC[Rich::Rich1][Rich::bottom] = rich1->
    nominalCentreOfCurvature(Rich::bottom);

  m_nominalCoC[Rich::Rich2][Rich::left] = rich2->
    nominalCentreOfCurvature(Rich::left);
  m_nominalCoC[Rich::Rich2][Rich::right] = rich2->
    nominalCentreOfCurvature(Rich::right);

  m_nominalFlatMirrorPlane[Rich::Rich1][Rich::top] = rich1->
    nominalPlane(Rich::top);
  m_nominalFlatMirrorPlane[Rich::Rich1][Rich::bottom]= rich1->
    nominalPlane(Rich::bottom);

  m_nominalFlatMirrorPlane[Rich::Rich2][Rich::left] = rich2->
    nominalPlane(Rich::left);
  m_nominalFlatMirrorPlane[Rich::Rich2][Rich::right] = rich2->
    nominalPlane(Rich::right);

  m_nomSphMirrorRadius[Rich::Rich1] = rich1->sphMirrorRadius();
  m_nomSphMirrorRadius[Rich::Rich2] = rich2->sphMirrorRadius();

  m_rich[Rich::Rich1] = rich1;
  m_rich[Rich::Rich2] = rich2;

  acquireTool( "RichMirrorSegFinder", m_mirrorSegFinder );
  acquireTool( "RichRayTracing", m_rayTracing );
  acquireTool( "RichSmartIDTool", m_idTool );
  acquireTool( "RichRefractiveIndex", m_refIndex );

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichPhotonReco::finalize() {

  debug() << "Finalize" << endreq;

  return RichToolBase::finalize();
}

//=========================================================================
//  reconstruct a photon from track segment and smartID
//=========================================================================
StatusCode RichPhotonReco::reconstructPhoton (const RichTrackSegment& trSeg,
                                              const RichSmartID& smartID,
                                              RichGeomPhoton& gPhoton ) const
{
  HepPoint3D detectionPoint;
  const StatusCode sc = m_idTool->globalPosition(smartID, detectionPoint);
  return ( sc.isFailure() ? sc : reconstructPhoton(trSeg,detectionPoint,gPhoton) );
}

//=========================================================================
//  reconstruct a photon from track segment and detection point
//=========================================================================
StatusCode RichPhotonReco::reconstructPhoton (const RichTrackSegment& trSeg,
                                              const HepPoint3D& detectionPoint,
                                              RichGeomPhoton& gPhoton ) const {

  const Rich::DetectorType rich = trSeg.rich();
  const Rich::RadiatorType radiator = trSeg.radiator();
  const Rich::Side side = m_rich[rich]->side( detectionPoint );

  // Working objects
  HepPoint3D emissionPoint;
  double fraction = 1;
  int sphMirror(0), flatMirror(0);
  DeRichSphMirror* sphSegment = 0;
  DeRichFlatMirror* flatSegment = 0;
  HepPoint3D virtDetPoint;
  double distance(0.0);

  bool foundMirrorSeg(false);

  if ( radiator == Rich::Aerogel ) {

    // thin radiator, assume full track segment is seen by pixel
    // and only one possible solution for mirror segments.
    emissionPoint = trSeg.bestPoint();

  } else {   // C4F10 or CF4

    // find the reflection of the detection point in the flat mirror
    // (virtual detection point) starting with nominal values
    distance = ( m_nominalFlatMirrorPlane[rich][side].
                 distance(detectionPoint) );
    virtDetPoint = ( detectionPoint - 2.0 * distance *
                     m_nominalFlatMirrorPlane[rich][side].normal() );

    // solve quartic equation with nominal values and find spherical
    // mirror reflection point
    HepPoint3D emissionPoint1 = trSeg.bestPoint( 0.01 );
    HepPoint3D sphReflPoint1;
    if ( !solveQuarticEq( emissionPoint1, m_nominalCoC[rich][side],
                          virtDetPoint, m_nomSphMirrorRadius[rich],
                          sphReflPoint1 ) ) return StatusCode::FAILURE;

    // find the spherical mirror segment
    DeRichSphMirror* sphSegment1 = m_mirrorSegFinder->
      findSphMirror(rich, side, sphReflPoint1);
    int sphMirror1 = sphSegment1->mirrorNumber();
    HepPoint3D flatReflPoint1;

    // find the flat mirror intersction point and flat mirror segment
    m_rayTracing->intersectPlane( sphReflPoint1,
                                  virtDetPoint - sphReflPoint1,
                                  m_nominalFlatMirrorPlane[rich][side],
                                  flatReflPoint1);

    DeRichFlatMirror* flatSegment1 = m_mirrorSegFinder->
      findFlatMirror(rich, side, flatReflPoint1 );
    int flatMirror1 = flatSegment1->mirrorNumber();

    // now do it again for emission point #2
    HepPoint3D emissionPoint2 = trSeg.bestPoint( 0.99 );

    // find the spherical mirror segment
    HepPoint3D sphReflPoint2;
    if ( !solveQuarticEq( emissionPoint2, m_nominalCoC[rich][side],
                          virtDetPoint, m_nomSphMirrorRadius[rich],
                          sphReflPoint2 ) ) return StatusCode::FAILURE;

    DeRichSphMirror* sphSegment2=m_mirrorSegFinder->
      findSphMirror(rich, side, sphReflPoint2);
    int sphMirror2 = sphSegment2->mirrorNumber();
    HepPoint3D flatReflPoint2;

    // find the flat mirror intersction point and flat mirror segment
    m_rayTracing->intersectPlane( sphReflPoint2,
                                  virtDetPoint - sphReflPoint2,
                                  m_nominalFlatMirrorPlane[rich][side],
                                  flatReflPoint2);
    DeRichFlatMirror* flatSegment2=m_mirrorSegFinder->
      findFlatMirror(rich, side, flatReflPoint2 );
    int flatMirror2 = flatSegment2->mirrorNumber();

    if ( radiator == Rich::C4F10 ) {
      // First reflection and hit point on same y side ?
      bool sameSide1 = ( sphReflPoint1.y() * detectionPoint.y() > 0 );
      bool sameSide2 = ( sphReflPoint2.y() * detectionPoint.y() > 0 );
      if ( sameSide1 && sameSide2 ) {
        emissionPoint = trSeg.bestPoint();
      } else if ( sameSide1 ) {
        fraction = fabs( sphReflPoint1.y() / (sphReflPoint1.y() -
                                              sphReflPoint2.y()) );
        emissionPoint = trSeg.bestPoint( fraction/2 );
      } else if ( sameSide2 ) {
        fraction = fabs( sphReflPoint2.y() / (sphReflPoint1.y() -
                                              sphReflPoint2.y()) );
        emissionPoint = trSeg.bestPoint( 1-fraction/2 );
      } else {
        //std::cout << "Rich1:Both hits opposite side to track hit"<<std::endl;
        return StatusCode::FAILURE;
      }

    } else {
      // First sphReflPoint and hit point on same y side ?
      bool sameSide1 = ( sphReflPoint1.x() * detectionPoint.x() > 0 );
      bool sameSide2 = ( sphReflPoint2.x() * detectionPoint.x() > 0 );
      if ( sameSide1 && sameSide2 ) {
        emissionPoint = trSeg.bestPoint();
      } else if ( sameSide1 ) {
        fraction = fabs( sphReflPoint1.x()/(sphReflPoint1.x() -
                                            sphReflPoint2.x()));
        emissionPoint = trSeg.bestPoint( fraction/2 );
      } else if ( sameSide2 ) {
        fraction = fabs( sphReflPoint2.x()/(sphReflPoint1.x() -
                                            sphReflPoint2.x()));
        emissionPoint = trSeg.bestPoint( 1-fraction/2 );
      } else {
        //std::cout <<"Rich2:Both hits opposite side to track hit"<<std::endl;
        return StatusCode::FAILURE;
      }
    }

    if ( (sphMirror1 == sphMirror2) && (flatMirror1 == flatMirror2) ) {
      sphMirror = sphMirror1;
      flatMirror = flatMirror1;
      gPhoton.setMirrorNumValid( true );
      sphSegment = sphSegment1;
      flatSegment = flatSegment1;
      foundMirrorSeg = true;
    }

  }

  HepPoint3D sphReflPoint, flatReflPoint;

  if ( !foundMirrorSeg ) {  // mirror segments different or aerogel
    // start again trying to find the correct mirror segment

    distance =  m_nominalFlatMirrorPlane[rich][side].distance(detectionPoint);
    virtDetPoint = detectionPoint - 2.0 * distance *
      m_nominalFlatMirrorPlane[rich][side].normal();

    if ( !solveQuarticEq( emissionPoint, m_nominalCoC[rich][side],
                          virtDetPoint, m_nomSphMirrorRadius[rich],
                          sphReflPoint ) ) return StatusCode::FAILURE;

    m_rayTracing->intersectPlane( sphReflPoint,
                                  virtDetPoint - sphReflPoint,
                                  m_nominalFlatMirrorPlane[rich][side],
                                  flatReflPoint);


    flatSegment=m_mirrorSegFinder->findFlatMirror(rich, side, flatReflPoint );
    flatMirror = flatSegment->mirrorNumber();

    sphSegment=m_mirrorSegFinder->findSphMirror(rich, side, sphReflPoint);
    sphMirror = sphSegment->mirrorNumber();

    gPhoton.setMirrorNumValid( false );
  }

  // solve for best emission point with best mirror segments
  distance =  flatSegment->mirrorPlane().distance(detectionPoint);
  virtDetPoint = detectionPoint - 2.0 * distance *
    flatSegment->mirrorPlane().normal();

  if ( !solveQuarticEq( emissionPoint, sphSegment->centreOfCurvature(),
                        virtDetPoint, sphSegment->radius(),
                        sphReflPoint ) ) return StatusCode::FAILURE;

  // check that reflection point is on the same side
  if ( radiator == Rich::CF4 ) {
    if ( sphReflPoint.x() * virtDetPoint.x() < 0.0 ) return StatusCode::FAILURE;
  } else if ( sphReflPoint.y() * virtDetPoint.y() < 0.0 ) return StatusCode::FAILURE;

  m_rayTracing->intersectPlane( sphReflPoint,
                                virtDetPoint - sphReflPoint,
                                flatSegment->mirrorPlane(),
                                flatReflPoint);

  // calculate angles
  HepVector3D photonDirection = sphReflPoint - emissionPoint;
  double thetaCerenkov, phiCerenkov;
  trSeg.angleToDirection( photonDirection, thetaCerenkov, phiCerenkov );

  // Correct theta Cerenkov for refraction at exit of aerogel
  if ( trSeg.radiator() == Rich::Aerogel ) {
    photonDirection.setMag(1);
    HepVector3D trackDir = trSeg.bestMomentum().unit();
    const double refAero  = m_refIndex->refractiveIndex( Rich::Aerogel, trSeg.avPhotonEnergy() );
    const double refc4f10 = m_refIndex->refractiveIndex( Rich::C4F10,   trSeg.avPhotonEnergy() );
    const double RratioSq = (refAero*refAero)/(refc4f10*refc4f10);
    HepVector3D newDir;
    newDir.setZ( sqrt(1. - ( 1. - photonDirection.z()*photonDirection.z() )/RratioSq ));
    double R = photonDirection.y()/photonDirection.x();
    newDir.setX( sqrt( (1.-newDir.z()*newDir.z())/(1. + R*R) ) );
    if ( photonDirection.x() < 0 ) newDir.setX( -1 * newDir.x() );
    newDir.setY( R*newDir.x() );
    double ctc = newDir.dot( trackDir );
    thetaCerenkov = ( ctc>1 ? 0 : acos(ctc) );
  }

  // Set photon parameters
  gPhoton.setCherenkovTheta( static_cast<float>(thetaCerenkov) );
  gPhoton.setCherenkovPhi( static_cast<float>(phiCerenkov) );
  gPhoton.setActiveSegmentFraction( static_cast<float>(fraction) );
  gPhoton.setDetectionPoint( detectionPoint );
  gPhoton.setFlatMirReflectionPoint( flatReflPoint );
  gPhoton.setEmissionPoint( emissionPoint );
  gPhoton.setSphMirReflectionPoint( sphReflPoint);
  gPhoton.setSphMirrorNum( sphMirror );
  gPhoton.setFlatMirrorNum( flatMirror );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  setup and solve quartic equation
//=========================================================================
bool RichPhotonReco::solveQuarticEq (const HepPoint3D& emissionPoint,
                                     const HepPoint3D& CoC,
                                     const HepPoint3D& virtDetPoint,
                                     const double radius,
                                     HepPoint3D& sphReflPoint) const {

  const HepVector3D evec = emissionPoint - CoC;
  const double e2 = evec.mag2();
  if ( 0 == e2 )  return 0;

  const HepVector3D dvec = virtDetPoint - CoC;
  const double d2 = dvec.mag2();
  if ( 0 == d2 )  return 0;

  const double e  = sqrt(e2);
  const double d  = sqrt(d2);
  const double gamma = evec.angle(dvec);
  const double dx = d * cos(gamma);
  const double dy = d * sin(gamma);
  const double r2 = radius * radius;
  const double r  = radius;

  // Array for quartic equation
  double a[5];
  a[0] =    4 * e2 * d2;
  a[1] =  - 4 * e2 * dy * r;
  a[2] =   dy * dy * r2 + (e+dx) * (e+dx) * r2 - a[0];
  a[3] =   2 * e * dy * (e-dx) * radius;
  a[4] =   ( e2 - r2 ) * dy * dy ;

  gsl_complex x[4];
  if ( !quarticEquation(a, x) ) return false;

  HepVector3D nvec = evec.cross(dvec); // normal vector to reflection plane

  HepVector3D delta[2];
  int j = 0;
  for( int i = 0; i<4 && j<2; ++i ) {
    if ( 0 == GSL_IMAG(x[i]) ) {
      double beta = asin( GSL_REAL(x[i]) );
      delta[j] = evec;
      delta[j].setMag(radius);
      delta[j++].rotate(beta, nvec);
    }
  }

  if ( delta[0].z() > delta[1].z() ) {
    sphReflPoint  = CoC + delta[0];
  } else {
    sphReflPoint  = CoC + delta[1];
  }

  return true;
}
