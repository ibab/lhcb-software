
#include "RichDet/Rich.h"
#include "RichDet/PhotonSpectrum.h"
#include "RichDet/PhotonReflector.h"
#include "RichDet/RichParameters.h"
#include "RichDet/Trajectory.h"

//#include "RichRec/Photon.h"

extern "C" void drteq4_(double*,double*,double*,double*,double*,double*,int*);

PhotonReflector::PhotonReflector (const Rich &rich)
  : rich_(&rich), m_center(4) 
{
}

PhotonReflector::~PhotonReflector() {}

bool PhotonReflector::intersectMirror (const Trajectory &trajectory,
                                           double &distance) const
{
  for( int side = 0; side<nsides(); side++ ) {

    double dist1, dist2;
    if ( trajectory.intersect(m_center[side],m_radius,dist1,dist2) ) {

      HepPoint3D point1 = trajectory.position(dist1);
      HepPoint3D point2 = trajectory.position(dist2);

      // Intersection with maximum z and on the same side as the mirror

      HepPoint3D point;
      double dist;
      if ( point1.z() > point2.z() ) {
        point = point1;
        dist = dist1;
      } else {
        point = point2;
        dist = dist2;
      }

      // take into account multiple mirrors
      switch ( this->rich().id() ) {
      case Rich::Rich1 :
        if ( point.x() * m_center[side].x() > 0. &&
             point.y() * m_center[side].y() > 0. ) {
          distance = dist;
          return true;
        }
        break;
      case Rich::Rich2 :
        if ( point.x() * m_center[side].x() > 0. ) {
          distance = dist;
          return true;
        }

      }
    }
  }
  return false;
}

HepPoint3D PhotonReflector::setCenter (const double radius,
                                           const double tiltx,
                                           const double tilty,
                                           const double zPos)
{
  HepPoint3D mirrorAxis(0., 0., -radius);
  mirrorAxis.transform( HepRotateY3D(tilty) );
  mirrorAxis.transform( HepRotateX3D(tiltx) );
  mirrorAxis.transform( HepTranslateZ3D(zPos) );

  return mirrorAxis;
}

void PhotonReflector::updateTrajectory(Trajectory& trajectory,
                                       const HepPoint3D& pos,
                                       const HepVector3D& dir) const
{ 
  trajectory.m_position = pos;
  trajectory.m_direction = dir;
  trajectory.m_direction.setMag(1.0);
}


bool PhotonReflector::reflect (Trajectory& photon) const
{
  /*
  assert( photon.status() == Photon::Emitted ||
          photon.status() == Photon::Scattered );
  */

  // reflection on spherical mirror

  double dist;
  if ( ! this->intersectMirror(photon,dist) ) {
    //    photon.absorbed();
    return false;
  }

  HepPoint3D pos = photon.position(dist);

  int side = PhotonReflector::side(pos);

  // photon could be absorbed if detector is physically split
  // Assume here only Rich2 has this in x
  if ( Rich::Rich2 == this->rich().id() && 
       pos.x() * photon.position().x() < 0. ) {
    //    photon.absorbed();
    return false;
  }
  
  HepVector3D dir    = photon.direction(/*dist*/);
  HepNormal3D normal = pos - m_center[side];

  dir = dir - normal * ( 2.0 * (normal * dir ) / normal.mag2() );

  //photon.reflected(pos,dir);
  updateTrajectory(photon, pos, dir);

  return true;
}

bool PhotonReflector::reflectionPoint (const HepPoint3D &emission,
                                           const HepPoint3D &detection,
                                           HepPoint3D &reflection) const
{

  int side = PhotonReflector::side(detection);

  HepVector3D evec = emission  - m_center[side];
  HepVector3D dvec = detection - m_center[side];
  double gamma     = evec.angle(dvec);

  double e2 = evec.mag2();
  double d2 = dvec.mag2();
  double e  = sqrt(e2);
  double d  = sqrt(d2);
  double dx = d * cos(gamma);
  double dy = d * sin(gamma);
  double r2 = m_radius * m_radius;
  double r  = m_radius;

  double a[5];
  a[0] =    4 * e2 * d2;
  a[1] =  - 4 * e2 * dy * r;
  a[2] =   dy * dy * r2 + (e+dx) * (e+dx) * r2 - a[0];
  a[3] =   2 * e * dy * (e-dx) * m_radius;
  a[4] =   ( e2 - r2 ) * dy * dy ;

  std::complex<double> x[4];
#ifndef NDEBUG
  assert( quarticEquation(a,x) );
#else
  quarticEquation(a,x);
#endif

  HepVector3D nvec = evec.cross(dvec); // normal vector to reflection plane

  HepVector3D delta[2];
  int j = 0;
  for( int i = 0; i<4 && j<2; ++i ) {
    if ( std::imag(x[i]) == 0 ) {
      double beta = asin(std::real(x[i]));
      delta[j] = evec;
      delta[j].setMag(m_radius);
      delta[j++].rotate(beta,nvec);
    }
  }

  if ( delta[0].z() > delta[1].z() ) {
    reflection = m_center[side] + delta[0];
  } else {
    reflection = m_center[side] + delta[1];
  }

  switch ( this->rich().id() ) {
  case Rich::Rich1 :
    return ( reflection.x() * detection.x() > 0. &&
             reflection.y() * detection.y() > 0 );
    break;
  case Rich::Rich2 :  
    return ( reflection.x() * detection.x() > 0. );
    break;
  };

  return false;
}

bool PhotonReflector::quarticEquation (const double a[5],
                                           std::complex<double> z[4])
{
  double b[4], c[8];

  int ierr;
  double resolv;

  if ( a[0] == 0 ) {
    return false;
  }
  { //VC++
    for(int i=0; i<4; ++i) {
      b[i] = a[i+1] / a[0];
    }}

  drteq4_(&b[0],&b[1],&b[2],&b[3],c,&resolv,&ierr);

  int j = 0;
  { //VC++
    for(int i = 0; i<4; ++i) {
      z[i] = std::complex<double>(c[j],c[j+1]);
      j += 2;
    }}

  return true;
}
