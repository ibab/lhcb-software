
#ifndef RICHDET_PHOTONREFLECTOR_H
#define RICHDET_PHOTONREFLECTOR_H 1

#include <cassert>
#include <vector>
#include <complex>
#include "CLHEP/Geometry/Point3D.h"

#include "RichDet/CLHEP_Geometry.h"

class Rich;
class PhotonSpectrum;
class RichParameters;
class Trajectory;

//class Photon;

class PhotonReflector {

public:
  PhotonReflector (const Rich &rich);

  virtual ~PhotonReflector();

  virtual void update (const RichParameters &parameter) = 0;

  const Rich & rich () const;

  int nsides () const;

  virtual PhotonSpectrum * newPhotonEfficiency () const = 0;

  // Provide the intersection with the main Mirror
  virtual bool intersectMirror (const Trajectory &trajectory,
                                double &distance) const;

  virtual bool reflect (Trajectory& photon) const;
  
  virtual bool reflectionPoint (const HepPoint3D &emission,
                                const HepPoint3D &detection,
                                HepPoint3D &reflection) const;

protected:

  void updateTrajectory(Trajectory& trajectory, 
                        const HepPoint3D& pos,
                        const HepVector3D& dir) const;

  static HepPoint3D setCenter (const double radius,
                               const double tiltx,
                               const double tilty,
                               const double zPos);

  int side (const HepPoint3D &point) const;

  // Data Members for Class Attributes

  std::vector<HepPoint3D> m_center;

  int m_Nsides;
  // CRJ: Note that this value is used for both spherical and flat mirrors
  //      in Rich2.
  double m_reflectivity;

  double m_radius;

  double m_tiltx;

  double m_tilty;

  double m_zPos;

  // Note the following variables are replacements for the tilts 
  // and zPos for SICB
  double m_xCen, m_yCen, m_zCen;

  double m_xMinFlatMirror, m_xMaxFlatMirror;
  double m_zMinFlatMirror, m_zMaxFlatMirror;

private:
  PhotonReflector();

  PhotonReflector(const PhotonReflector &right);

  PhotonReflector & operator=(const PhotonReflector &right);

  static bool quarticEquation (const double a[5], std::complex<double> z[4]);

private:

  const Rich *rich_;

};

inline const Rich & PhotonReflector::rich () const {
  assert( rich_ != NULL );
  return *rich_;
}

inline int PhotonReflector::nsides () const {
  assert (m_Nsides != 0);
  return m_Nsides;
}

inline int PhotonReflector::side (const HepPoint3D &position) const
{
  return ( position.x() > 0 ? 0 : 1 ) +
    ( 4 == this->nsides() ? ( position.y() > 0 ? 0 : 2 ) : 0 );
}

#endif // RICHDET_PHOTONREFLECTOR_H
