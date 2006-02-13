// $Id: CircleTraj.cpp,v 1.4 2006-02-13 11:00:51 graven Exp $
// Include files

// local
#include "Kernel/CircleTraj.h"

#include "Math/GenVector/AxisAngle.h"
#include <math.h>

using namespace LHCb;
using namespace ROOT::Math;
using namespace Gaudi;

CircleTraj*
CircleTraj::clone() const
{
        return new CircleTraj(*this);
}

CircleTraj::CircleTraj( const Gaudi::XYZPoint& origin,
                        const Gaudi::XYZVector& dir1,
                        const Gaudi::XYZVector& dir2,
                        double radius)
        :m_origin(origin),
         m_normal(dir1.Cross(dir2).unit()),
         m_dirStart(dir1.unit()),
         m_range(0,radius*std::asin(m_dirStart.Cross(dir2.unit()).r())),
         m_radius(radius)
{
};

CircleTraj::CircleTraj( const Gaudi::XYZPoint& origin,
                        const Gaudi::XYZVector& normal,
                        const Gaudi::XYZVector& origin2point,
                        const Range& range)
        :m_origin(origin),
         m_normal(normal.unit()),
         m_dirStart(origin2point-origin2point.Dot(m_normal)*m_normal),
         m_range(range),
         m_radius(m_dirStart.r())
{
};

/// Point on the trajectory at arclength from the starting point    
Gaudi::XYZPoint CircleTraj::position( double s ) const
{
  return m_origin+m_radius*AxisAngle(m_normal,s/m_radius)(m_dirStart);
};

/// First derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector CircleTraj::direction( double s ) const
{
  return m_normal.Cross(AxisAngle(m_normal,s/m_radius)(m_dirStart));
   
};

/// Second derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector CircleTraj::curvature( double s ) const 
{
  return (-1.0/m_radius)*AxisAngle(m_normal,s/m_radius)(m_dirStart);
};

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void CircleTraj::expansion( double s,
                            Gaudi::XYZPoint& p,
                            Gaudi::XYZVector& dp,
                            Gaudi::XYZVector& ddp ) const
{
  Gaudi::XYZVector r(  AxisAngle(m_normal,s/m_radius)(m_dirStart) );
  ddp =  (-1.0/m_radius)*r;
  dp  = m_normal.Cross(r);
  p   = m_origin+m_radius*r;
};

/// Retrieve the derivative of point at fixed arclength 'arclenght'
/// with respect to the circle parameters
//CircleTraj::Derivative
//CircleTraj::derivative( double/* arclength */) const
//{
  //Derivative deriv;  
  //// FIXME: Not done yet!!!
  //return deriv;       
//};

/// Determine the closest point on the circle to a
/// given point, and return the corresponding arclength
double CircleTraj::arclength( const Gaudi::XYZPoint& point ) const
{
  // get vector from origin, to point after projecting it 
  // into the plane of the circle. (i.e. this vector is normal
  // to m_normal)
  Gaudi::XYZVector r( (point - m_normal.Dot(point-m_origin)*m_normal)-m_origin );
  // determine the delta-phi with the start direction, properly signed!!!
  double dphi=std::asin( m_dirStart.Cross(r.unit()).Dot(m_normal) );
  return m_radius*dphi;
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
  return cbrt(6*tolerance*m_radius*m_radius);
};

  /// Range in arclength w.r.t. the starting point
  /// over which the trajectory is valid
Trajectory::Range CircleTraj::range() const
{
  return m_range;
};
