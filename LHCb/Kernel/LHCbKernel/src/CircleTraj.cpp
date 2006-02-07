// $Id: CircleTraj.cpp,v 1.2 2006-02-07 11:07:10 erodrigu Exp $
// Include files

// local
#include "Kernel/CircleTraj.h"

  /// Constructor from an origin, a radius
  /// and a range in angle w.r.t. angle of origin point
LHCb::CircleTraj::CircleTraj( const Gaudi::XYZPoint& origin,
                              const double radius,
                              const std::pair<double,double> angularRange ) 
{
  m_origin = origin;
  m_radius = radius;
  m_angularRange = angularRange;
  m_range = std::pair<double,double>(m_angularRange.first*m_radius,m_angularRange.first*m_radius);
};

/// Point on the trajectory at arclength from the starting point    
Gaudi::XYZPoint LHCb::CircleTraj::position( const double& arclength ) const
{
  double phi = m_angularRange.first + arclength/m_radius;
  Gaudi::XYZPoint point = Gaudi::XYZPoint();
  point.SetXYZ(m_radius*cos(phi)+m_origin.X(),m_radius*sin(phi)+m_origin.Y(),m_origin.Z());
  
  return point;
};

/// First derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector LHCb::CircleTraj::direction( const double& arclength ) const
{
  double phi = m_angularRange.first + arclength/m_radius;
  Gaudi::XYZVector vec = Gaudi::XYZVector();
  vec.SetXYZ(-m_radius*sin(phi),m_radius*cos(phi),0.);

  return vec;
};

/// Second derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector LHCb::CircleTraj::curvature( const double& arclength ) const 
{
  double phi = m_angularRange.first + arclength/m_radius;
  Gaudi::XYZVector vec = Gaudi::XYZVector();
  vec.SetXYZ(-m_radius*sin(phi),-m_radius*sin(phi),0.);

  return vec;
};

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void LHCb::CircleTraj::expansion( const double& arclength,
                                  Gaudi::XYZPoint& p,
                                  Gaudi::XYZVector& dp,
                                  Gaudi::XYZVector& ddp ) const
{
  p   = position(arclength);
  dp  = direction(arclength);
  ddp = curvature(arclength);
};

/// Retrieve the derivative of the parabolic approximation to the trajectory
/// with respect to the state parameters
ROOT::Math::SMatrix<double,3,LHCb::CircleTraj::kSize>
LHCb::CircleTraj::derivative( const double& arclength ) const
{
  ROOT::Math::SMatrix<double,3,LHCb::CircleTraj::kSize> deriv =
    ROOT::Math::SMatrix<double,3,LHCb::CircleTraj::kSize>();

  // Not done yet!!!
  return deriv;       
};

/// Determine the distance in arclenghts to the
/// closest point on the trajectory to a given point
double LHCb::CircleTraj::distanceToPoint( const Gaudi::XYZPoint& point ) const
{
  // Not done yet!!!
  return 0.;
};

/// Number of arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double LHCb::CircleTraj::distTo1stError( double& , const double& , int ) const 
{
  // Not done yet!!!
  return 10*km;  
};

/// Number of arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double LHCb::CircleTraj::distTo2ndError( double& , const double& , int ) const
{
  // Not done yet!!!
  return 10*km;  
};

  /// Range in arclength w.r.t. the starting point
  /// over which the trajectory is valid
std::pair<double,double> LHCb::CircleTraj::range() const
{
  return m_range;
};

/// Length of trajectory
double LHCb::CircleTraj::length() const
{
  return (m_angularRange.second-m_angularRange.first)*m_radius;
};
