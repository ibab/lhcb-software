// $Id: Trajectory.h,v 1.2 2001-11-16 15:54:55 jonrob Exp $
#ifndef RICHDET_TRAJECTORY_H
#define RICHDET_TRAJECTORY_H 1

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

class Trajectory {

public:
  virtual ~Trajectory();

  virtual HepPoint3D position (const double distance = 0.) const;

  virtual HepVector3D direction () const;

  virtual bool intersect (const HepPlane3D &plane,
                          double &distance) const;

  virtual bool intersect (const HepPoint3D &center,
                          const double radius,
                          double &distance1,
                          double &distance2) const;

  virtual HepVector3D rotateDirection (const double distance,
                                       const double theta,
                                       const double phi,
                                       const HepVector3D &axis) const;

  virtual void angleToDirection (const double distance,
                                 const HepVector3D &direction,
                                 const HepVector3D &axis,
                                 double &theta,
                                 double &phi) const; 

protected:

  // ajb 191001

  //  void setPosition(const HepPoint3D& pos);
  //  void setDirection(const HepVector3D& dir);

  HepPoint3D m_position;
  HepVector3D m_direction;

  friend class PhotonDetector;
  friend class PhotonReflector;
  friend class PhotonRadiator;

private:

  virtual HepRotation & rotation (const double distance,
                                  const HepVector3D &axis) const;

  mutable double m_lastDistance;

  mutable HepVector3D m_lastAxis;

  mutable HepRotation m_lastRotation;




};

#endif // RICHDET_TRAJECTORY_H
