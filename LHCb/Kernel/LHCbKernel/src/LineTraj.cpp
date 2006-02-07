// $Id: LineTraj.cpp,v 1.3 2006-02-07 11:07:10 erodrigu Exp $
// Include files

// local
#include "Kernel/LineTraj.h"

/// Constructor from the middle point and a unit direction vector
LHCb::LineTraj::LineTraj( const Gaudi::XYZPoint& middle,
                          const Gaudi::XYZVector& dir,
                          const std::pair<double,double> range ) 
{
  m_pos = middle;
  m_dir = dir.Unit();
  m_range = range;
};

/// Constructor from a begin and an end point
LHCb::LineTraj::LineTraj( const Gaudi::XYZPoint& begPoint,
                          const Gaudi::XYZPoint& endPoint )
{
  m_pos = Gaudi::XYZPoint( (begPoint.X()+endPoint.X()) / 2.,
                           (begPoint.Y()+endPoint.Y()) / 2.,
                           (begPoint.Z()+endPoint.Z()) / 2. );
  
  Gaudi::XYZVector vec = Gaudi::XYZVector();
  vec.SetXYZ(endPoint.X()-begPoint.X(),endPoint.Y()-begPoint.Y(),endPoint.Z()-begPoint.Z());
  m_dir = vec.Unit();
  
  std::pair<double,double> range = std::pair<double,double>(-sqrt(vec.Mag2()),sqrt(vec.Mag2()));
  m_range = range;
};

/// Point on the trajectory at arclength from the starting point    
Gaudi::XYZPoint LHCb::LineTraj::position( const double& arclength ) const
{
  return m_pos + arclength * m_dir;
};

/// First derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector LHCb::LineTraj::direction( const double& arclength ) const
{
  return m_dir;
};

/// Second derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector LHCb::LineTraj::curvature( const double& arclength ) const 
{
  return Gaudi::XYZVector();
};

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void LHCb::LineTraj::expansion( const double& arclength,
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
ROOT::Math::SMatrix<double,3,LHCb::LineTraj::kSize>
LHCb::LineTraj::derivative( const double& arclength ) const
{
  ROOT::Math::SMatrix<double,3,LHCb::LineTraj::kSize> deriv =
    ROOT::Math::SMatrix<double,3,LHCb::LineTraj::kSize>();
  
  deriv(0,0) = 1.0;
  deriv(0,3) = arclength;
  deriv(1,1) = 1.0;
  deriv(1,4) = arclength;
  deriv(2,2) = 1.0;
  deriv(2,5) = arclength;

  return deriv;
};

/// Determine the distance in arclenghts to the
/// closest point on the trajectory to a given point
double LHCb::LineTraj::distanceToPoint( const Gaudi::XYZPoint& point ) const
{
  Gaudi::XYZVector vec = Gaudi::XYZVector();
  vec.SetXYZ( point.X()-m_pos.X(), point.Y()-m_pos.Y(), point.Z()-m_pos.Z() );
  
  return m_dir.Dot(vec);
};

// Not yet implemented
double LHCb::LineTraj::distTo1stError( double& , const double& , int ) const 
{
  return 10*km;
};

// Not yet implemented
double LHCb::LineTraj::distTo2ndError( double& , const double& , int ) const
{
  return 10*km;
};

/// Range in arclength w.r.t. the starting point
/// over which the trajectory is valid
std::pair<double,double> LHCb::LineTraj::range() const
{
  return m_range;
};

/// Length of trajectory
double LHCb::LineTraj::length() const
{
  return m_range.second-m_range.first;
};

