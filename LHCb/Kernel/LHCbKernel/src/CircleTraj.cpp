// $Id: CircleTraj.cpp,v 1.3 2006-02-10 12:28:22 graven Exp $
// Include files

// local
#include "Kernel/CircleTraj.h"
#include "Kernel/Vector3DTypes.h"

using namespace LHCb;
using namespace ROOT::Math;

  /// Constructor from an origin, a radius
  /// and a range in angle w.r.t. angle of origin point
CircleTraj::CircleTraj( const Gaudi::XYZPoint& origin,
                        double radius,
                        const Range& angularRange ) 
{
  m_origin = origin;
  m_radius = radius;
  m_angularRange = angularRange;
  m_range = Range(m_angularRange.first*m_radius,m_angularRange.first*m_radius);
};

/// Point on the trajectory at arclength from the starting point    
Gaudi::XYZPoint CircleTraj::position( double arclength ) const
{
  double fi = phi(arclength);
  return Gaudi::XYZPoint(m_radius*cos(fi)+m_origin.X(),m_radius*sin(fi)+m_origin.Y(),m_origin.Z());
};

/// First derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector CircleTraj::direction( double arclength ) const
{
  double fi = phi(arclength);
  return Gaudi::XYZVector(-m_radius*sin(fi),m_radius*cos(fi),0.);
};

/// Second derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector CircleTraj::curvature( double arclength ) const 
{
  double fi = phi(arclength);
  return Gaudi::XYZVector(-m_radius*sin(fi),-m_radius*sin(fi),0.); // FIXME: sin,sin? should point to center of circle...
};

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void CircleTraj::expansion( double arclength,
                                  Gaudi::XYZPoint& p,
                                  Gaudi::XYZVector& dp,
                                  Gaudi::XYZVector& ddp ) const
{
  ddp = curvature(arclength);
  dp  = direction(arclength);
  p   = position(arclength);
};

/// Retrieve the derivative of the parabolic approximation to the trajectory
/// with respect to the state parameters
SMatrix<double,3,CircleTraj::kSize>
CircleTraj::derivative( double/* arclength */) const
{
  SMatrix<double,3,CircleTraj::kSize> deriv;
  // FIXME: Not done yet!!!
  return deriv;       
};

/// Determine the distance in arclenghts to the
/// closest point on the trajectory to a given point
double CircleTraj::arclength( const Gaudi::XYZPoint& /*point*/ ) const
{
  // FIXME: Not done yet!!!
  return 0.;
};

/// Number of arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double CircleTraj::distTo1stError( double , double , int ) const 
{
  // FIXME: Not done yet!!!
  return 10*km;  
};

/// Number of arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double CircleTraj::distTo2ndError( double , double , int ) const
{
  // FIXME: Not done yet!!!
  return 10*km;  
};

  /// Range in arclength w.r.t. the starting point
  /// over which the trajectory is valid
Trajectory::Range CircleTraj::range() const
{
  return m_range;
};

/// Length of trajectory
double CircleTraj::length() const
{
  return (m_angularRange.second-m_angularRange.first)*m_radius;
};
