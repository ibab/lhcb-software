// $Id: RichReflector.cpp,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
// Include files

// local
#include "RichReflector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichReflector
//
// 18/05/2002 :
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichReflector::RichReflector( )
  : m_deFlatSegments         (),
    m_deSphericalSegments    (),
    m_nominalFlatMirrorPlane (),
    m_nominalSphMirrorCentre (),
    m_sphMirrorRadius        (0),
    m_sphMirrorRadius2       (0) { }

RichReflector:: RichReflector( FlatMirrorPtrVector& deFlatSegments,
                               SphMirrorPtrVector& deSphSegments,
                               const HepPlane3D& nominalFlatMirror,
                               const HepPoint3D& nominalSphMirrorCentre,
                               const double nominalSphMirrorRadius )
  : m_deFlatSegments               ( deFlatSegments ),
    m_deSphericalSegments          ( deSphSegments ),
    m_nominalFlatMirrorPlane       ( nominalFlatMirror ),
    m_nominalFlatMirrorPlaneNormal ( nominalFlatMirror.normal() ),
    m_nominalSphMirrorCentre       ( nominalSphMirrorCentre ),
    m_sphMirrorRadius              ( nominalSphMirrorRadius ),
    m_sphMirrorRadius2             ( nominalSphMirrorRadius*nominalSphMirrorRadius ) { }

RichReflector::~RichReflector(  ) {}

RichReflector::RichReflector( const RichReflector& source )
  : m_deFlatSegments         ( source.m_deFlatSegments ),
    m_deSphericalSegments    ( source.m_deSphericalSegments ),
    m_nominalFlatMirrorPlane ( source.m_nominalFlatMirrorPlane ),
    m_nominalFlatMirrorPlaneNormal ( source.m_nominalFlatMirrorPlaneNormal ),
    m_nominalSphMirrorCentre ( source.m_nominalSphMirrorCentre ),
    m_sphMirrorRadius        ( source.m_sphMirrorRadius ),
    m_sphMirrorRadius2       ( source.m_sphMirrorRadius2 ) { }

RichReflector& RichReflector::operator=(const RichReflector& source) {

  if ( this != &source ) {
    m_deFlatSegments               = source.m_deFlatSegments;
    m_deSphericalSegments          = source.m_deSphericalSegments;
    m_nominalFlatMirrorPlane       = source.m_nominalFlatMirrorPlane;
    m_nominalFlatMirrorPlaneNormal = source.m_nominalFlatMirrorPlaneNormal;
    m_nominalSphMirrorCentre       = source.m_nominalSphMirrorCentre;
    m_sphMirrorRadius              = source.m_sphMirrorRadius;
    m_sphMirrorRadius2             = source.m_sphMirrorRadius2;
  }

  return *this;
}

int RichReflector::reflectionPoint( const HepPoint3D& emissionPoint,
                                    const HepPoint3D& detectionPoint,
                                    HepPoint3D& reflectionPointSphMirror,
                                    HepPoint3D& reflectionPointFlatMirror )
  const {

  // need to reflect the detectionPoint on the flat mirror here...
  const double distance = m_nominalFlatMirrorPlane.distance(detectionPoint);
  const HepPoint3D mirrorDetectionPoint = 
    detectionPoint - 2.0 * distance * m_nominalFlatMirrorPlaneNormal;

  const HepVector3D evec = emissionPoint - m_nominalSphMirrorCentre;
  const HepVector3D dvec = mirrorDetectionPoint - m_nominalSphMirrorCentre;

  const double e2 = evec.mag2();
  if ( 0 == e2 )  return 0; 
  const double d2 = dvec.mag2();
  if ( 0 == d2 )  return 0; 
  const double e  = sqrt(e2);
  const double d  = sqrt(d2);
  const double gamma = evec.angle(dvec);
  const double dx = d * cos(gamma);
  const double dy = d * sin(gamma);

  // Array for quartic equation
  double a[5];
  a[0] = 4 * e2 * d2;
  a[1] =  - 4 * e2 * dy * m_sphMirrorRadius;
  a[2] =   dy * dy * m_sphMirrorRadius2 + (e+dx) * (e+dx) * m_sphMirrorRadius2 - a[0];
  a[3] =   2 * e * dy * (e-dx) * m_sphMirrorRadius;
  a[4] =   ( e2 - m_sphMirrorRadius2 ) * dy * dy ;

  gsl_complex x[4];
  if ( !quarticEquation(a, x) ) return 0;

  const HepVector3D nvec = evec.cross(dvec); // normal vector to reflection plane

  HepVector3D delta[2];
  int j = 0;
  for( int i = 0; i<4 && j<2; ++i ) {
    if ( 0 == GSL_IMAG(x[i]) ) {
      double beta = asin( GSL_REAL(x[i]) );
      delta[j] = evec;
      delta[j].setMag(m_sphMirrorRadius);
      delta[j++].rotate(beta, nvec);
    }
  }

  if ( delta[0].z() > delta[1].z() ) {
    reflectionPointSphMirror = m_nominalSphMirrorCentre + delta[0];
  } else {
    reflectionPointSphMirror = m_nominalSphMirrorCentre + delta[1];
  }

  if (!intersectNominalFlatMirror(reflectionPointSphMirror,
                                  mirrorDetectionPoint-reflectionPointSphMirror,
                                  reflectionPointFlatMirror) ) {
    return 0;
  }

  return 1;
}

// Returns reflection of Spherical mirrors
int RichReflector::reflectSph( HepPoint3D& position,
                               HepVector3D& direction ) const {

  HepPoint3D tmpPosition = position;
  HepVector3D tmpDirection = direction;
  HepPoint3D intersectionPoint;

  // first, reflection on spherical mirror
  if ( !intersectNominalSphMirror( position,
                                   direction,
                                   intersectionPoint ) ) return 0;

  HepNormal3D normal = intersectionPoint - m_nominalSphMirrorCentre;

  position = intersectionPoint;
  direction -= 2.0 * ((normal*direction)/normal.mag2()) * normal;

  return 1;
}

// Returns reflection of flat mirrors
int RichReflector::reflectFlat( HepPoint3D& position,
                                HepVector3D& direction ) const {

  HepPoint3D tmpPosition = position;
  HepVector3D tmpDirection = direction;
  HepPoint3D intersectionPoint;

  // reflection on flat mirror
  if ( !intersectNominalFlatMirror( tmpPosition,
                                    tmpDirection,
                                    intersectionPoint ) ) return 0;

  position = intersectionPoint;
  direction = ( tmpDirection - 2.0 * 
                (m_nominalFlatMirrorPlaneNormal*tmpDirection) * 
                m_nominalFlatMirrorPlaneNormal );

  return 1;
}

int RichReflector::reflect( HepPoint3D& position,
                            HepVector3D& direction ) const {

  HepPoint3D tmpPosition = position;
  HepVector3D tmpDirection = direction;
  HepPoint3D intersectionPoint;

  // first, reflection on spherical mirror
  if ( !intersectNominalSphMirror( position,
                                   direction,
                                   intersectionPoint) ) return 0;

  HepNormal3D normal = intersectionPoint - m_nominalSphMirrorCentre;

  tmpPosition = intersectionPoint;
  tmpDirection = direction - 2.0 * ((normal*direction)/normal.mag2()) * normal;

  // then, reflection on flat mirror:
  if ( !intersectNominalFlatMirror(tmpPosition,tmpDirection,intersectionPoint) )
    return 0;

  position = intersectionPoint;
  direction = ( tmpDirection - 2.0 * 
                (m_nominalFlatMirrorPlaneNormal*tmpDirection) * 
                m_nominalFlatMirrorPlaneNormal );

  return 1;
}

int RichReflector::intersectNominalSphMirror( const HepPoint3D& position,
                                              const HepVector3D& direction,
                                              HepPoint3D& intersection ) const {

  const HepVector3D delta = position - m_nominalSphMirrorCentre;
  const double a = direction.mag2();
  const double b = 2. * direction * delta;
  const double c = delta.mag2() - m_sphMirrorRadius2;
  const double disc = b*b - 4.*a*c;
  if ( disc < 0. ) return 0;

  const double distance1 = 0.5 * ( sqrt(disc) - b ) / a;
  intersection = position + distance1 * direction;

  return 1;
}


int RichReflector::intersectNominalFlatMirror( const HepPoint3D& position,
                                               const HepVector3D& direction,
                                               HepPoint3D& intersection ) const {

  const double d = m_nominalFlatMirrorPlaneNormal * direction;
  if ( d == 0 ) return 0;
  const double distance = - ( m_nominalFlatMirrorPlaneNormal * position + 
                              m_nominalFlatMirrorPlane.d() ) / d;

  intersection = position + distance * direction;
  return 1;
}
