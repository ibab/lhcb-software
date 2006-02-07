// $Id: ParabolaTraj.cpp,v 1.3 2006-02-07 11:07:10 erodrigu Exp $
// Include files

// local
#include "Kernel/ParabolaTraj.h"

/// Constructor from a (middle) point, a (unit) direction vector and a curvature
LHCb::ParabolaTraj::ParabolaTraj( const Gaudi::XYZPoint& middle,
                                  const Gaudi::XYZVector& dir,
                                  const Gaudi::XYZVector& curv,
                                  const std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> endPoints ) 
{
  m_pos  = middle;
  m_dir  = dir.Unit();       
  m_curv = curv;

  double s1 = (-m_dir.X()-sqrt(m_dir.X()*m_dir.X()+2*(endPoints.first).X())*m_curv.X()-2*m_pos.X()*m_curv.X()) / 
m_curv.X();
  double s2 = (-m_dir.X()+sqrt(m_dir.X()*m_dir.X()+2*(endPoints.first).X())*m_curv.X()-2*m_pos.X()*m_curv.X()) / 
m_curv.X();
  double solA = (s1 < s2) ? s1 : s2;
  double s3 = (-m_dir.X()-sqrt(m_dir.X()*m_dir.X()+2*(endPoints.second).X())*m_curv.X()-2*m_pos.X()*m_curv.X()) / 
m_curv.X();
  double s4 = (-m_dir.X()+sqrt(m_dir.X()*m_dir.X()+2*(endPoints.second).X())*m_curv.X()-2*m_pos.X()*m_curv.X()) / 
m_curv.X();
  double solB = (s3 > s4) ? s1 : s2;
  std::pair<double,double> range = std::pair<double,double>(solA,solB);
  m_range = range;
};

/// Point on the trajectory at arclength from the starting point    
Gaudi::XYZPoint LHCb::ParabolaTraj::position( const double& arclength ) const
{
  return m_pos + arclength * m_dir + 0.5 * arclength * arclength * m_curv;
};

/// First derivative of the trajectory at arclength from the starting point   
Gaudi::XYZVector LHCb::ParabolaTraj::direction( const double& arclength ) const
{
  return m_dir + arclength * m_curv;
};

/// Second derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector LHCb::ParabolaTraj::curvature( const double& arclength ) const 
{
  return m_curv;
};

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void LHCb::ParabolaTraj::expansion( const double& arclength,
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
ROOT::Math::SMatrix<double,3,LHCb::ParabolaTraj::kSize>
LHCb::ParabolaTraj::derivative( const double& arclength ) const
{
  ROOT::Math::SMatrix<double,3,LHCb::ParabolaTraj::kSize> deriv =
    ROOT::Math::SMatrix<double,3,LHCb::ParabolaTraj::kSize>();
  
  deriv(0,0) = 1.0;
    deriv(1,1) = 1.0;
    deriv(1,1) = 1.0;
    deriv(0,3) = arclength;
    deriv(1,4) = arclength;
    deriv(2,5) = arclength;
    deriv(0,6) = 0.5 * arclength * arclength;
    deriv(1,7) = 0.5 * arclength * arclength;
    deriv(2,8) = 0.5 * arclength * arclength;

  return deriv;       
};

  /// Determine the distance in arclenghts to the
  /// closest point on the trajectory to a given point
double LHCb::ParabolaTraj::distanceToPoint( const Gaudi::XYZPoint& point ) const
{
// Not yet implemented
  return 0.;
};

/// Number of arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double LHCb::ParabolaTraj::distTo1stError( double& , const double& , int ) const 
{
// Not yet implemented
  return 10*km;  
};

/// Number of arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double LHCb::ParabolaTraj::distTo2ndError( double& , const double& , int ) const
{
// Not yet implemented
  return 10*km;  
};

/// Range in arclength w.r.t. the starting point
/// over which the trajectory is valid
std::pair<double,double> LHCb::ParabolaTraj::range() const
{
  return m_range;
};

/// Length of trajectory
double LHCb::ParabolaTraj::length() const
{
  return 0.;
};

