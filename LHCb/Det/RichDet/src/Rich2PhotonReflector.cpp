
#include <cassert>

#include "RichDet/Rich.h"
#include "RichDet/PhotonSpectrum.h"
#include "RichDet/PhotonDetector.h"
#include "RichDet/RichParameters.h"
#include "RichDet/Trajectory.h"
//#include "RichRec/Photon.h"
#include "RichDet/Rich2PhotonReflector.h"


Rich2PhotonReflector::Rich2PhotonReflector (const Rich &rich)
  : PhotonReflector(rich),
    flatMirror_(2)
{
}

Rich2PhotonReflector::~Rich2PhotonReflector(){}

void Rich2PhotonReflector::update (const RichParameters &parameter) 
{
  //m_Nsides       = parameter.getValue("Rich2.Nsides",   2 );
  //m_radius       = parameter.getValue("Rich2.Refl.Radius",     820.   );
  //m_reflectivity = parameter.getValue("Rich2.Refl.Reflectivity", 0.95  );

  m_reflectivity = parameter.getValue("CDF.RIC2.REFMIRSPH2", 0.95  );
  m_Nsides       = 2;

  // spherical Mirror
  m_radius       = parameter.getValue("CDF.RIC2.RCMIR2",      820.   );
 
  //m_zPos         = parameter.getValue("Rich2.Refl.ZPos",      1110.   );
  //m_tiltx        = parameter.getValue("Rich2.Refl.Tiltx",        0. );
  //m_tilty        = parameter.getValue("Rich2.Refl.Tilty",        0.37 );
  //m_center[0] = setCenter(m_radius, m_tiltx,-m_tilty, m_zPos);
  //m_center[1] = setCenter(m_radius, m_tiltx, m_tilty, m_zPos);

  m_xCen         = parameter.getValue("CDF.RIC2.XMIR2",   50.0 );
  m_yCen         = parameter.getValue("CDF.RIC2.YMIR2",   11.0 );
  m_zCen         = parameter.getValue("CDF.RIC2.ZMIR2",   27.5 );
  m_center[0] = HepPoint3D( m_xCen, m_yCen, m_zCen);
  m_center[1] = HepPoint3D(-m_xCen, m_yCen, m_zCen);

  // flat Mirror
  //zPos1_        = parameter.getValue("Rich2.Refl.ZPos1",     1012.   );
  //tilt1_        = parameter.getValue("Rich2.Refl.Tilt1",     -0.24 );

  m_xMinFlatMirror = parameter.getValue("CDF.RIC2.XREFB2",   118.9);
  m_xMaxFlatMirror = parameter.getValue("CDF.RIC2.XREFA2",   291.7);
  m_zMinFlatMirror = parameter.getValue("CDF.RIC2.ZREFB2",   986.3);
  m_zMaxFlatMirror = parameter.getValue("CDF.RIC2.ZREFA2",   961.95);

  // set spherical and flat mirror centers
  flatMirror_[0] = setPlane( m_xMinFlatMirror, m_xMaxFlatMirror, 
                             m_zMinFlatMirror, m_zMaxFlatMirror);
  flatMirror_[1] = setPlane( -m_xMinFlatMirror, -m_xMaxFlatMirror, 
                             m_zMinFlatMirror, m_zMaxFlatMirror);

}

PhotonSpectrum * Rich2PhotonReflector::newPhotonEfficiency () const
{

  // this reflector has two mirrors

  PhotonSpectrum * spectrum = rich().detector().photonEfficiency();
  assert( spectrum != NULL );
  (*spectrum) *= m_reflectivity * m_reflectivity;
  return spectrum;

}

bool Rich2PhotonReflector::reflect (Trajectory &photon) const
{

  // reflection on spherical mirror

  if ( ! this->PhotonReflector::reflect(photon) ) {
    // if ( photon.status() == Photon::Absorbed ) return;
    return false;
  }
  //  assert( photon.status() == Photon::Reflected );

  // reflexion on flat mirror

  int side = this->side( photon.position() );

  double dist;
  if ( ! photon.intersect(flatMirror_[side],dist) ) {
    // photon.absorbed();
    return false;
  }

  HepPoint3D pos    = photon.position(dist);
  HepVector3D dir    = photon.direction(/*dist*/);
  HepNormal3D normal = flatMirror_[side].normal();

  dir = dir - 2.0 * (normal * dir ) * normal;

  //  photon.reflected(pos,dir);
  PhotonReflector::updateTrajectory(photon, pos, dir);

  return true;
}

HepPlane3D Rich2PhotonReflector::setPlane (const double xMin,
                                           const double xMax,
                                           const double zMin,
                                           const double zMax)
{
  HepPoint3D point1(xMin, 0., zMin);
  HepPoint3D point2(xMax, 0., zMax);
  HepPoint3D point3(xMax, 100., zMax);
  
  HepPlane3D plane(point1, point2, point3);
  plane.normalize();

  return plane;
}

bool Rich2PhotonReflector::reflectionPoint (const HepPoint3D &emission,
                                            const HepPoint3D &detection,
                                            HepPoint3D &reflection) const
{
  int side = this->side( detection );

  double distance = flatMirror_[side].distance(detection);
  HepPoint3D mirrorDetection = detection -
    2. * distance * flatMirror_[side].normal();
  return PhotonReflector::reflectionPoint(emission,
                                              mirrorDetection,
                                              reflection);
}
