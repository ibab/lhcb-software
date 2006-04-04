// $Id: ParabolaTraj.cpp,v 1.6 2006-04-04 16:04:04 ebos Exp $
// Include files

// local
#include "Kernel/ParabolaTraj.h"
using namespace LHCb;
using namespace ROOT::Math;

std::auto_ptr<Trajectory> ParabolaTraj::clone() const
{
        return std::auto_ptr<Trajectory>(new ParabolaTraj(*this));
}

/// Constructor from a (middle) point, a (unit) direction vector and a curvature
ParabolaTraj::ParabolaTraj( const Gaudi::XYZPoint& point,
                            const Gaudi::XYZVector& dir,
                            const Gaudi::XYZVector& curv,
                            const Range& range)
        :m_pos(point),m_dir(dir.unit()),m_curv(curv),m_range(range)
{
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
ParabolaTraj::Derivative
ParabolaTraj::derivative( double arclength ) const
{
  Derivative deriv;
  deriv(0,0) = deriv(1,1) = deriv(1,1) = 1.0;
  deriv(0,3) = deriv(1,4) = deriv(2,5) = arclength;
  deriv(0,6) = deriv(1,7) = deriv(2,8) = 0.5 * arclength * arclength;
  return deriv;       
};

  /// Determine the arclenghts of the
  /// closest point on this trajectory to a given point
double ParabolaTraj::arclength( const Gaudi::XYZPoint& point ) const
{
  // for now, return 0th order approximantion, i.e. assume |m_curv|<<|m_dir|
  return m_dir.Dot(point-m_pos);

// until we are sure that the code below is OK for |m_curv|<<|m_dir|!!
//
//  // get vector from m_pos to point projected into plane of parabola
//  Gaudi::XYZVector normal = m_dir.Cross(m_curv).unit();
//  Gaudi::XYZPoint r( ( point - normal.Dot(point-m_pos)*normal )-m_pos);
//  // get normalized 'x' and 'y' coordinates of this vector by projecting onto the
//  // axis. In terms of these, the parabola is parameterized as (arclen, arclen^2/2)
//  // (i.e. arclen is actually the distance along the 'x' coordinate!)
//  double x = m_dir.Dot(r);
//  double y = m_curv.Dot(r);
//  //  now we need to minimize the distance between (x,y) and (s,s*s/2)
//  //  where s is the arclen (well, not quite, but that is what we really
//  //  use in 'point(arclen)' ;-)
//  //  This requires solving a 3rd order polynomial, so we assume m_curve<<1
//  //  and solve a linear equation instead.
//  return x/(1-y);
};

/// arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double ParabolaTraj::distTo1stError( double , double tolerance , int ) const 
{
  return std::sqrt(2*tolerance/m_curv.r());
};

/// 2nd order is OK everywhere...
double ParabolaTraj::distTo2ndError( double , double , int ) const
{
  return 10*km;  
};

/// Range in arclength w.r.t. the starting point
/// over which the trajectory is valid
Trajectory::Range ParabolaTraj::range() const
{
  return m_range;
};
