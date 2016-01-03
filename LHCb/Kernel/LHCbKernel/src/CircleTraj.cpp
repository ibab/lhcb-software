// Include files

// local
#ifdef __INTEL_COMPILER         // Disable ICC remark from ROOT
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
#endif

#include "LHCbMath/FastRoots.h"
#include "Kernel/CircleTraj.h"

#include "Math/GenVector/AxisAngle.h"
#include "vdt/exp.h"
#include "vdt/log.h"
#include "vdt/atan2.h"

using namespace LHCb;
using namespace ROOT::Math;
using namespace Gaudi;

std::unique_ptr<Trajectory> CircleTraj::clone() const
{
  return std::unique_ptr<Trajectory>(new CircleTraj(*this));
}

CircleTraj::CircleTraj( const Point& origin,
                        const Vector& dir1,
                        const Vector& dir2,
                        double radius)
  : Trajectory(0.,radius*std::asin((dir1.unit()).Cross(dir2.unit()).r())),
    m_origin(origin),
    m_normal(dir1.Cross(dir2).unit()),
    m_dirStart(dir1.unit()),
    m_radius(radius),
    m_cbrt6radius2(std::cbrt(6. * radius * radius))
{}

CircleTraj::CircleTraj( const Point& origin,
                        const Vector& normal,
                        const Vector& origin2point,
                        const Range& range)
  : Trajectory(range),
    m_origin(origin),
    m_normal(normal.unit()),
    m_dirStart(origin2point-origin2point.Dot(m_normal)*m_normal),
    m_radius(m_dirStart.r()),
    m_cbrt6radius2(std::cbrt(6. * m_radius * m_radius))
{}

/// Point on the trajectory at arclength from the starting point
Trajectory::Point CircleTraj::position( double s ) const
{
  return m_origin + m_radius*AxisAngle(m_normal,s / m_radius)(m_dirStart);
}

/// First derivative of the trajectory at arclength from the starting point
Trajectory::Vector CircleTraj::direction( double s ) const
{
  return m_normal.Cross(AxisAngle(m_normal,s / m_radius)(m_dirStart));
}

/// Second derivative of the trajectory at arclength from the starting point
Trajectory::Vector CircleTraj::curvature( double s ) const
{
  return (-1.0 / m_radius)*AxisAngle(m_normal,s / m_radius)(m_dirStart);
}

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void CircleTraj::expansion( double s,
                            Point& p,
                            Vector& dp,
                            Vector& ddp ) const
{
  const auto r = AxisAngle(m_normal, s / m_radius)(m_dirStart);
  ddp =  (-1.0 / m_radius) * r;
  dp  = m_normal.Cross(r);
  p   = m_origin + m_radius * r;
}

/// Determine the closest point on the circle to a
/// given point, and return the corresponding arclength
double CircleTraj::muEstimate( const Point& point ) const
{
  // get vector from origin, to point after projecting it
  // into the plane of the circle. (i.e. this vector is normal
  // to m_normal)
  const auto d = point - m_origin;
  const auto r = d - m_normal.Dot(d) * m_normal;
  // use trigonometric addition theorems to avoid one expensive atan2 call and
  // the annoying if statements to clamp things into the allowed range
  const auto cosdphi = r.x() * m_dirStart.x() + r.y() * m_dirStart.y();
  const auto sindphi = r.y() * m_dirStart.x() - r.x() * m_dirStart.y();

  // replace the other expensive atan2 call with the vdt version
  return m_radius * vdt::fast_atan2(sindphi, cosdphi);
}

/// arclength until deviation of the trajectory from the expansion
/// reaches the specified tolerance.
double CircleTraj::distTo1stError( double /*arclen*/, double tolerance, int /*direction*/) const
{
  // require 2nd order term to be less than tolerance...
  return std::sqrt(2*tolerance*m_radius);
}

/// arclength until deviation of the trajectory from the expansion
/// reaches the specified tolerance.
double CircleTraj::distTo2ndError( double /*arclen*/, double tolerance , int /*direction*/ ) const
{
  // require 3rd order term to be less than tolerance
  return m_cbrt6radius2 * FastRoots::cbrt(tolerance);
}
