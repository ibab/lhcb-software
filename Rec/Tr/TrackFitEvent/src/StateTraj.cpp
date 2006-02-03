// $Id: StateTraj.cpp,v 1.2 2006-02-03 09:16:58 ebos Exp $
// Include files

// local
#include "Event/StateTraj.h"

using namespace Gaudi;
using namespace LHCb;

StateTraj::StateTraj( const State& state,
                      const XYZVector& bField )
{
  m_pos = state.position();
  // True when approximating the trajectory as a straight line
  m_dir = state.slopes().unit();
  m_qOverP = state.qOverP();
  m_bField = bField;   
  m_curv = kappa * m_qOverP * ( m_dir.Cross(m_bField) );
}

StateTraj::StateTraj( const TrackVector& stateVector,
                      const double& z,
                      const XYZVector& bField )
{
  XYZVector slopes = XYZVector( stateVector(2), stateVector(3), 1. );
  m_pos = XYZPoint( stateVector(0), stateVector(1), z );
  // True when approximating the trajectory as a straight line
  m_dir = slopes.unit();
  m_qOverP = stateVector(4);
  m_bField = bField;
  m_curv = kappa * m_qOverP * ( m_dir.Cross(m_bField) );   
}

XYZPoint StateTraj::position( const double& arclength ) const
{
  return m_pos + arclength * ( m_dir + 0.5 * arclength * curvature() );
}
    
XYZVector StateTraj::direction( const double& arclength ) const
{
  return (m_dir + arclength * curvature()).unit();
}

XYZVector StateTraj::curvature() const 
{
  return m_curv;
}

void StateTraj::expansion( const double& arclength,
                           XYZPoint& p,
                           XYZVector& dp,
                           XYZVector& ddp ) const 
{
  ddp = curvature();
  dp = direction(arclength);
  p = position(arclength);
}

const ROOT::Math::SMatrix<double, 3, 5> StateTraj::derivative( const double& arclength ) const 
{
//  double vx  = m_dir(0);
//  double vy  = m_dir(1);
//  double vz  = m_dir(2);
//  double svz = arclength * vz;
//  double arcvxyz  = arclength * vx * vy * vz;
//  double arcvx = 1.0 - vx * vx;
//  double arcvy = 1.0 - vy * vy;
//  double C     = 0.5 * arclength * arclength * kappa;
//  double cvz   = C * m_qOverP * vz;
//  HepVector3D crossB = m_dir.cross(m_bField);
//
//  SMatrix deriv<double, 3, 5>;
//
//  deriv(0,0) = 1.0;
//  deriv(0,2) = svz * arcvx - cvz * vx * crossB(0);
//  deriv(0,3) = - arcvxyz
//    + cvz * ( vy * vz * m_bField(1) + arcvy * m_bField(2) );
//  deriv(0,4) = C * crossB(0);
//  deriv(1,1) = 1.0;
//  deriv(1,2) = - arcvxyz
//    - cvz * ( vx * vz * m_bField(0) + arcvx * m_bField(2) );
//  deriv(1,3) = svz * arcvy - cvz * vy * crossB(1);
//  deriv(1,4) = C * crossB(1);
//  deriv(2,2) = - vx * vz * svz
//    + cvz * ( vx * vy * m_bField(0) + arcvx * m_bField(1) );
//  deriv(2,3) = - vy * vz * svz
//    - cvz * ( vx * vy * m_bField(1) + arcvy * m_bField(0) );
//  deriv(2,4) = C * crossB(2);
//
//  return deriv;        
ROOT::Math::SMatrix<double,3,5> mat =  ROOT::Math::SMatrix<double,3,5>();

return mat;
}

// Not yet implemented
double StateTraj::distTo1stError( double& , const double& , int ) const 
{
  return 10*km;  
}

// Not yet implemented
double StateTraj::distTo2ndError( double& , const double& , int ) const
{
  return 10*km;  
}

// Not yet implemented  
std::pair<double,double> StateTraj::range() const
{
  std::pair<double,double> infiniteRange;
  infiniteRange.first = -10*km;
  infiniteRange.second = 10*km;
}
