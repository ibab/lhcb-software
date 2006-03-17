// $Id: LineTraj.cpp,v 1.6 2006-03-17 13:10:18 ocallot Exp $
// Include files

// local
#include "Kernel/LineTraj.h"
using namespace LHCb;
using namespace ROOT::Math;

LineTraj*
LineTraj::clone() const
{
        return new LineTraj(*this);
}

/// Constructor from the middle point and a unit direction vector
LineTraj::LineTraj( const Gaudi::XYZPoint& middle,
                    const Gaudi::XYZVector& dir,
                    const Range& range ) 
        : m_dir(dir.Unit()),m_pos(middle),m_range(range)
{
};

/// Constructor from a begin and an end point
LineTraj::LineTraj( const Gaudi::XYZPoint& begPoint,
                    const Gaudi::XYZPoint& endPoint )
  : m_dir(endPoint-begPoint)
  , m_pos(begPoint+0.5*m_dir)
{
  double d = m_dir.r();
  m_range = Range(-d,d);
  m_dir = m_dir.Unit();
};

/// Point on the trajectory at arclength from the starting point    
Gaudi::XYZPoint LineTraj::position( double arclength ) const
{
  return m_pos + arclength * m_dir;
};

/// First derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector LineTraj::direction( double /* arclength*/ ) const
{
  return m_dir;
};

/// Second derivative of the trajectory at arclength from the starting point
Gaudi::XYZVector LineTraj::curvature( double /* arclength */ ) const 
{
  return Gaudi::XYZVector(0,0,0);
};

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void LineTraj::expansion( double arclength,
                          Gaudi::XYZPoint& p,
                          Gaudi::XYZVector& dp,
                          Gaudi::XYZVector& ddp ) const
{
  ddp = Gaudi::XYZVector(0,0,0);
  dp  = m_dir;
  p   = m_pos + arclength * m_dir;
};

/// Retrieve the derivative of the parabolic approximation to the trajectory
/// with respect to the state parameters
LineTraj::Derivative
LineTraj::derivative( double arclength ) const
{
  Derivative deriv;
  deriv(0,0) = deriv(1,1) = deriv(2,2) = 1.0;
  deriv(0,3) = deriv(1,4) = deriv(2,5) = arclength;
  return deriv;
};

/// Determine the distance in arclenghts to the
/// closest point on the trajectory to a given point
double LineTraj::arclength( const Gaudi::XYZPoint& point ) const
{
  return m_dir.Dot(point-m_pos);
};

// 1st order approx OK everywhere
double LineTraj::distTo1stError( double , double , int ) const 
{
  return 10*km;
};

// 2nd order approx OK everywhere
double LineTraj::distTo2ndError( double , double , int ) const
{
  return 10*km;
};

/// Range in arclength w.r.t. the starting point
/// over which the trajectory is valid
Trajectory::Range LineTraj::range() const
{
  return m_range;
};
