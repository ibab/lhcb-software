// $Id: ParabolaTraj.cpp,v 1.4 2006-02-10 12:28:22 graven Exp $
// Include files

// local
#include "Kernel/ParabolaTraj.h"
using namespace LHCb;
using namespace ROOT::Math;

/// Constructor from a (middle) point, a (unit) direction vector and a curvature
ParabolaTraj::ParabolaTraj( const Gaudi::XYZPoint& middle,
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
  m_range = Range(solA,solB);
};

/// Point on the trajectory at arclength from the starting point    
Gaudi::XYZPoint ParabolaTraj::position( double arclength ) const
{
  return m_pos + arclength * (m_dir + 0.5 * arclength * m_curv);
};

/// First derivative of the trajectory at arclength from the starting point   
Gaudi::XYZVector ParabolaTraj::direction( double arclength ) const
{
  return m_dir + arclength * m_curv;
};

/// Second derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector ParabolaTraj::curvature( double /* arclength */) const 
{
  return m_curv;
};

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void ParabolaTraj::expansion( double arclength,
                                    Gaudi::XYZPoint& p,
                                    Gaudi::XYZVector& dp,
                                    Gaudi::XYZVector& ddp ) const
{
  ddp = m_curv;
  dp  = m_dir + arclength*m_curv;
  p   = m_pos + arclength* (m_dir + 0.5 * arclength * m_curv);
};

/// Retrieve the derivative of the parabolic approximation to the trajectory
/// with respect to the state parameters
SMatrix<double,3,ParabolaTraj::kSize>
ParabolaTraj::derivative( double arclength ) const
{
  SMatrix<double,3,ParabolaTraj::kSize> deriv;
  deriv(0,0) = deriv(1,1) = deriv(1,1) = 1.0;
  deriv(0,3) = deriv(1,4) = deriv(2,5) = arclength;
  deriv(0,6) = deriv(1,7) = deriv(2,8) = 0.5 * arclength * arclength;
  return deriv;       
};

  /// Determine the distance in arclenghts to the
  /// closest point on the trajectory to a given point
double ParabolaTraj::arclength( const Gaudi::XYZPoint& /*point*/ ) const
{
// FIXME: Not yet implemented
  return 0.;
};

/// Number of arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double ParabolaTraj::distTo1stError( double , double , int ) const 
{
// FIXME: Not yet implemented
  return 10*km;  
};

/// Number of arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double ParabolaTraj::distTo2ndError( double , double , int ) const
{
//FIXME: Not yet implemented
  return 10*km;  
};

/// Range in arclength w.r.t. the starting point
/// over which the trajectory is valid
Trajectory::Range ParabolaTraj::range() const
{
  return m_range;
};

/// Length of trajectory
double ParabolaTraj::length() const
{
  return m_range.second-m_range.first;
};

