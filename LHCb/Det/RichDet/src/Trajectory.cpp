// $Id: Trajectory.cpp,v 1.1 2001-10-31 16:50:13 rihill Exp $

#include <cmath>
#include <cassert>

#include "RichDet/Trajectory.h"

HepVector3D Trajectory::rotateDirection (const double distance, 
                                         const double theta,
                                         const double phi,
                                         const HepVector3D &axis) const
{
  return rotation(distance,axis) * HepVector3D(sin(theta)*cos(phi),
                                              sin(theta)*sin(phi),
                                              cos(theta));
}

Trajectory::~Trajectory(){}

void Trajectory::angleToDirection (const double distance, 
                                   const HepVector3D &direction, 
                                   const HepVector3D &axis, 
                                   double &theta, 
                                   double &phi)
{
  HepVector3D rotDirection = rotation(distance,axis).inverse() * direction;
  theta = rotDirection.theta();
  phi   = rotDirection.phi();
  if ( phi < 0 ) phi += 2 * M_PI;
}

HepRotation & Trajectory::rotation (const double distance, 
                                    const HepVector3D &axis) const
{
  if ( distance != m_lastDistance &&
       axis     != m_lastAxis ) {
    HepVector3D z = this->direction(/*distance*/);
    //      assert( abs(z*axis) < 1.);

    // temporary fix
    HepVector3D y;
    if ( abs(z * axis ) < 1. ) {
      y = z.cross(HepVector3D(0.,1.,0.));
    } else {
      y = z.cross(axis);
    }
    y.setMag(1.);
    HepVector3D x = y.cross(z);

    m_lastRotation = HepRotation();
    m_lastRotation.rotateAxes(x,y,z);
    m_lastDistance = distance;
    m_lastAxis     = axis;
  }
  return m_lastRotation;
}

HepPoint3D Trajectory::position (const double distance) const
{
  return m_position + distance * m_direction;
}

HepVector3D Trajectory::direction () const
{
  return m_direction;
}

bool Trajectory::intersect (const HepPlane3D &plane,
                        double &distance) const
{
  HepNormal3D normal = plane.normal();
  double d = normal * m_direction;
  if ( d == 0 ) return false;
  distance = - ( normal * m_position + plane.d() ) / d;
  return true;
}

bool Trajectory::intersect (const HepPoint3D &center,
                        const double radius,
                        double &distance1,
                        double &distance2) const
{
  HepVector3D delta = m_position - center;
  double a = m_direction.mag2();
  double b = 2. * m_direction * delta;
  double c = delta.mag2() - radius * radius;
  double disc = b * b - 4. * a * c;
  if ( disc < 0. ) return false;
  assert( a != 0 );
  disc = sqrt(disc);
  distance1 = 0.5 * ( - b + disc ) / a;
  distance2 = 0.5 * ( - b - disc ) / a;
  return true;
}

