// $Id: CircleTraj.cpp,v 1.10 2006-07-11 09:49:54 mneedham Exp $
// Include files

// local
#include "Kernel/CircleTraj.h"

#include "Math/GenVector/AxisAngle.h"
#include <math.h>

using namespace LHCb;
using namespace ROOT::Math;
using namespace Gaudi;

std::auto_ptr<Trajectory> CircleTraj::clone() const
{
  return std::auto_ptr<Trajectory>(new CircleTraj(*this));
}

CircleTraj::CircleTraj( const Point& origin,
                        const Vector& dir1,
                        const Vector& dir2,
                        double radius)
  : DifTraj<kSize>(0.,radius*std::asin((dir1.unit()).Cross(dir2.unit()).r())),
    m_origin(origin),
    m_normal(dir1.Cross(dir2).unit()),
    m_dirStart(dir1.unit()),
    m_radius(radius)
{
};

CircleTraj::CircleTraj( const Point& origin,
                        const Vector& normal,
                        const Vector& origin2point,
                        const Range& range)
  : DifTraj<kSize>(range),
    m_origin(origin),
    m_normal(normal.unit()),
    m_dirStart(origin2point-origin2point.Dot(m_normal)*m_normal),
    m_radius(m_dirStart.r())
{
};

/// Point on the trajectory at arclength from the starting point    
Trajectory::Point CircleTraj::position( double s ) const
{
  return m_origin+m_radius*AxisAngle(m_normal,s/m_radius)(m_dirStart);
};

/// First derivative of the trajectory at arclength from the starting point
Trajectory::Vector CircleTraj::direction( double s ) const
{
  return m_normal.Cross(AxisAngle(m_normal,s/m_radius)(m_dirStart));
};

/// Second derivative of the trajectory at arclength from the starting point
Trajectory::Vector CircleTraj::curvature( double s ) const 
{
  return (-1.0/m_radius)*AxisAngle(m_normal,s/m_radius)(m_dirStart);
};

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void CircleTraj::expansion( double s,
                            Point& p,
                            Vector& dp,
                            Vector& ddp ) const
{
  Vector r(  AxisAngle(m_normal,s/m_radius)(m_dirStart) );
  ddp =  (-1.0/m_radius)*r;
  dp  = m_normal.Cross(r);
  p   = m_origin+m_radius*r;
};

/// Retrieve the derivative of point at fixed arclength 'arclenght'
/// with respect to the circle parameters
CircleTraj::Derivative
CircleTraj::derivative( double/* arclength */) const
{
Derivative deriv;  
//// FIXME: Not done yet!!!
return deriv;       
};

/// Determine the closest point on the circle to a
/// given point, and return the corresponding arclength
double CircleTraj::arclength( const Point& point ) const
{
  // get vector from origin, to point after projecting it 
  // into the plane of the circle. (i.e. this vector is normal
  // to m_normal)
  Vector r( (point - m_normal.Dot(point-m_origin)*m_normal)-m_origin );

  // Determine delta phi angle between arclength=0 angle and angle of r
  double dphi = r.phi() - m_dirStart.phi();
  
  // Check whether angle outside of -pi/2 till +pi/2
  double check = m_dirStart.Dot( r );
  if( 0. > check ) {
    if( dphi > M_PI ) dphi -= 2.*M_PI;
    else dphi += 2.*M_PI;    
  }

  return m_radius * dphi;
};

/// arclength until deviation of the trajectory from the expansion
/// reaches the specified tolerance.
double CircleTraj::distTo1stError( double /*arclen*/, double tolerance, int /*direction*/) const 
{
  // require 2nd order term to be less than tolerance...
  return std::sqrt(2*tolerance*m_radius);
};

/// arclength until deviation of the trajectory from the expansion
/// reaches the specified tolerance.
double CircleTraj::distTo2ndError( double /*arclen*/, double tolerance , int /*direction*/ ) const
{
  // require 3rd order term to be less than tolerance
  // cbrt is in the C99 standard -- hope it is available on all platforms...
  // return cbrt(6*tolerance*m_radius*m_radius);
  // cbrt is NOT available on windows at this time...
  return pow(6*tolerance*m_radius*m_radius,double(1)/3);
};
