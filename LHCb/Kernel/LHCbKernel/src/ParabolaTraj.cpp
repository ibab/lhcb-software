// Include files

// local
#include "Kernel/ParabolaTraj.h"
#include "GaudiKernel/SystemOfUnits.h"
using namespace LHCb;
using namespace ROOT::Math;

std::unique_ptr<Trajectory> ParabolaTraj::clone() const
{
        return std::unique_ptr<Trajectory>(new ParabolaTraj(*this));
}

#ifdef __INTEL_COMPILER         // Disable ICC remark from ROOT
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
#endif

/// Constructor from a (middle) point, a (unit) direction vector and a curvature
ParabolaTraj::ParabolaTraj( const Point& point,
                            const Vector& dir,
                            const Vector& curv,
                            const Range& range)
  : Trajectory(range),
    m_pos(point),
    m_dir(dir.unit()),
    m_curv(curv)
{
}

/// Point on the trajectory at arclength from the starting point
Trajectory::Point ParabolaTraj::position( double arclength ) const
{
  return m_pos + arclength * (m_dir + 0.5 * arclength * m_curv);
}

/// First derivative of the trajectory at arclength from the starting point
Trajectory::Vector ParabolaTraj::direction( double arclength ) const
{
  return m_dir + arclength * m_curv;
}

/// Second derivative of the trajectory at arclength from the starting point
Trajectory::Vector ParabolaTraj::curvature( double /* arclength */) const
{
  return m_curv;
}

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void ParabolaTraj::expansion( double arclength,
                              Point& p,
                              Vector& dp,
                              Vector& ddp ) const
{
  ddp = m_curv;
  dp  = m_dir + arclength*m_curv;
  p   = m_pos + arclength* (m_dir + 0.5 * arclength * m_curv);
}

  /// Determine the arclenghts of the
  /// closest point on this trajectory to a given point
double ParabolaTraj::muEstimate( const Point& point ) const
{
  auto r = point - m_pos;
  if (m_curv.R()<0.01*m_dir.R()) { // small curvature limit: neglect curvature
     return r.Dot(m_dir);
  }
  // get vector from m_pos to point projected into plane of parabola
  auto normal = m_dir.Cross(m_curv).unit();
  r -= normal.Dot(r)*normal;
  // get normalized 'x' and 'y' coordinates of this vector by projecting onto the
  // axis. In terms of these, the parabola is parameterized as (arclen, arclen^2/2)
  // (i.e. arclen is actually the distance along the 'x' coordinate!)
  auto x = r.Dot(m_dir);
  auto y = r.Dot(m_curv);
  //  now we need to minimize the distance between (x,y) and (s,s*s/2)
  //  where s is the arclen (well, not quite, but that is what we really
  //  use in 'point(arclen)' ;-)
  //  This requires solving a 3rd order polynomial, so we assume m_curve<<1
  //  and solve a linear equation instead.
  return x/(1-y);
}

/// arclengths until deviation of the trajectory from the expansion
/// reaches the given tolerance.
double ParabolaTraj::distTo1stError( double , double tolerance , int ) const
{
  return std::sqrt(2*tolerance/m_curv.R());
}

/// 2nd order is OK everywhere...
double ParabolaTraj::distTo2ndError( double , double , int ) const
{
  return 10*Gaudi::Units::km;
}
