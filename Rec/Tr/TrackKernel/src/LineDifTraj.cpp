// Include files

// local
#include "TrackKernel/LineDifTraj.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <iostream>
using namespace std;
using namespace LHCb;
using namespace ROOT::Math;

#ifdef __INTEL_COMPILER         // Disable ICC remark from ROOT GenVector classes
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
#endif

std::unique_ptr<Trajectory> LineDifTraj::clone() const
{
  return std::unique_ptr<Trajectory>(new LineDifTraj(*this));
}


/// Constructor from the middle point and a direction vector
LineDifTraj::LineDifTraj( const Point& middle,
                          const Vector& dir,
                          const Range& range)
  : DifTraj<4>(range),
    m_dir(dir.Unit()),
    m_pos(middle)
{
}


/// Constructor from a begin and an end point
LineDifTraj::LineDifTraj( const Point& begPoint,
                          const Point& endPoint )
  : DifTraj<4>(-(XYZVector(endPoint-begPoint)).r()/2.,(XYZVector(endPoint-begPoint)).r()/2.),
    m_dir( (endPoint-begPoint).Unit() ),
    m_pos(begPoint+0.5*m_dir)
{
}

/// Point on the trajectory at arclength from the starting point    
Trajectory::Point LineDifTraj::position( double arclength ) const
{
  return m_pos + arclength * m_dir;
}

/// First derivative of the trajectory at arclength from the starting point
Trajectory::Vector LineDifTraj::direction( double /* arclength*/ ) const
{
  return m_dir;
}

/// Second derivative of the trajectory at arclength from the starting point
Trajectory::Vector LineDifTraj::curvature( double /* arclength */ ) const 
{
  return Vector(0,0,0);
}

/// Create a parabolic approximation to the trajectory
/// at arclength from the starting point
void LineDifTraj::expansion( double arclength,
                             Point& p,
                             Vector& dp,
                             Vector& ddp ) const
{
  ddp = Vector(0,0,0);
  dp  = m_dir;
  p   = m_pos + arclength * m_dir;
}

/// Determine the distance in arclenghts to the
/// closest point on the trajectory to a given point
double LineDifTraj::muEstimate( const Point& point ) const
{
  return m_dir.Dot(point-m_pos);
}

// 1st order approx OK everywhere
double LineDifTraj::distTo1stError( double , double , int ) const 
{
  return 10*Gaudi::Units::km;
}

// 2nd order approx OK everywhere
double LineDifTraj::distTo2ndError( double , double , int ) const
{
  return 10*Gaudi::Units::km;
}



LineDifTraj::Parameters LineDifTraj::parameters() const 
{
    return { m_pos.x(), m_pos.y(), 
             m_dir.x()/m_dir.z(), m_dir.y()/m_dir.z() };
}

LineDifTraj&
LineDifTraj::operator+=(const Parameters& delta) 
{
   m_pos += XYZVector(delta(0),delta(1),0);
   m_dir += m_dir.Z()*XYZVector(delta(2),delta(3),0);
   m_dir = m_dir.unit(); // renormalize direction...
   return *this;
}

LineDifTraj::Derivative 
LineDifTraj::derivative(double s) const 
{
// derivative is at FIXED value of s.
// this implies that z is NOT constant.
// so first compute z(s;x0,y0,tx,ty):
//   note that dz/ds = 1/sqrt(1^2+tx^2+ty^2) = dir.z
//             z0    = z(s=0)
//
//   z = z0 + s * dir.z
//   
//   dz/dtx = s * d(dir.z)/dtx = s*( -tx * dir.z^3) = s*dir.z * (-tx*dir.z^2)
//   dz/dty = s * d(dir.z)/dty = s*( -ty * dir.z^3) = s*dir.z * (-ty*dir.z^2)
//
// next, compute x(z) and y(z) as follows:
//   x = x0 + (z-z0) * tx
//   y = y0 + (z-z0) * ty
//
// and then compute the derivatives, using the chain rule, using
// the fact we already computed the derivatives of z:
//
//   dx/dx0 = 1                  dx/dy0 = 0
//   dy/dx0 = 0                  dy/dy0 = 1
//
//   dx/dtx = (z-z0) + tx dz/dtx ; dx/dty =          tx dz/dty
//   dy/dtx =          ty dz/dtx ; dy/dty = (z-z0) + ty dz/dty

    double n  = 1.0/m_dir.z();
    double dz = s*n; double dz2 = dz*n*n;
    auto p = parameters();
    Derivative d;
    // first z
    d(2,0) = 0; d(2,1) = 0; d(2,2) = -dz2*p(2)       ;  d(2,3) = -dz2*p(3);
    // then x and y...
    d(0,0) = 1; d(0,1) = 0; d(0,2) = dz + p(2)*d(2,2);  d(0,3) =      p(2)*d(2,3);
    d(1,0) = 0; d(1,1) = 1; d(1,2) =      p(3)*d(2,2);  d(1,3) = dz + p(3)*d(2,3);
    return d;
}

std::ostream& 
LineDifTraj::print(std::ostream& out) const {
   out << "LineDifTraj: " << m_pos << " + s * " << m_dir << endl;
   out << parameters() << endl;
   return out;
}
