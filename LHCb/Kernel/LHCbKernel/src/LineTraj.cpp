// $Id: LineTraj.cpp,v 1.9 2006-05-03 15:00:47 graven Exp $
// Include files

// local
#include "Kernel/LineTraj.h"
using namespace LHCb;
using namespace ROOT::Math;

std::auto_ptr<Trajectory> LineTraj::clone() const
{
        return std::auto_ptr<Trajectory>(new LineTraj(*this));
}

/// Constructor from the middle point and a unit direction vector
LineTraj::LineTraj( const Point& middle,
                    const Vector& dir,
                    const Range& range ) 
  : DifTraj<kSize>(range.first,range.second),
    m_dir(dir.Unit()),
    m_pos(middle)
{
};

/// Constructor from a begin and an end point
LineTraj::LineTraj( const Point& begPoint,
                    const Point& endPoint )
  : DifTraj<kSize>(-(XYZVector(endPoint-begPoint)).r()/2.,(XYZVector(endPoint-begPoint)).r()/2.),
    m_dir(endPoint-begPoint),
    m_pos(begPoint+0.5*m_dir)
{
  m_dir = m_dir.Unit();
};

/// Point on the trajectory at arclength from the starting point    
Trajectory::Point LineTraj::position( double arclength ) const
{
  return m_pos + arclength * m_dir;
};

/// First derivative of the trajectory at arclength from the starting point
Trajectory::Vector LineTraj::direction( double /* arclength*/ ) const
{
  return m_dir;
};

/// Second derivative of the trajectory at arclength from the starting point
Trajectory::Vector LineTraj::curvature( double /* arclength */ ) const 
{
  return Vector(0,0,0);
};

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void LineTraj::expansion( double arclength,
                          Point& p,
                          Vector& dp,
                          Vector& ddp ) const
{
  ddp = Vector(0,0,0);
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
double LineTraj::arclength( const Point& point ) const
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
