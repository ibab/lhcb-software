// $Id: StateTraj.cpp,v 1.4 2006-02-10 12:31:04 graven Exp $
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
};

StateTraj::StateTraj( const TrackVector& stateVector,
                      double z,
                      const XYZVector& bField )
{
  XYZVector slopes = XYZVector( stateVector(2), stateVector(3), 1. );
  m_pos = XYZPoint( stateVector(0), stateVector(1), z );
  // True when approximating the trajectory as a straight line
  m_dir    = slopes.unit();
  m_qOverP = stateVector(4);
  m_bField = bField;
  m_curv   = kappa * m_qOverP * ( m_dir.Cross(m_bField) );   
};

XYZPoint StateTraj::position( double arclength ) const
{
  return m_pos + arclength * ( m_dir + 0.5 * arclength * curvature(arclength) );
};
    
XYZVector StateTraj::direction( double arclength ) const
{
  return (m_dir + arclength * curvature(arclength)).unit();
};

XYZVector StateTraj::curvature(double /*arclength*/) const 
{
  return m_curv;
};

void StateTraj::expansion( double arclength,
                           XYZPoint& p,
                           XYZVector& dp,
                           XYZVector& ddp ) const 
{
  p   = position(arclength);
  dp  = direction(arclength);
  ddp = curvature(arclength);
};

ROOT::Math::SMatrix<double,3,LHCb::StateTraj::kSize>
StateTraj::derivative( double arclength ) const
{
  ROOT::Math::SMatrix<double,3,LHCb::StateTraj::kSize> deriv =
    ROOT::Math::SMatrix<double,3,LHCb::StateTraj::kSize>();

  double vx  = m_dir.x();
  double vy  = m_dir.y();
  double vz  = m_dir.z();
  double svz = arclength * vz;
  double arcvxyz  = arclength * vx * vy * vz;
  double arcvx = 1.0 - vx * vx;
  double arcvy = 1.0 - vy * vy;
  double C     = 0.5 * arclength * arclength * kappa;
  double cvz   = C * m_qOverP * vz;
  XYZVector crossB = m_dir.Cross(m_bField);

  deriv(0,0) = 1.0;
  deriv(0,2) = svz * arcvx - cvz * vx * crossB.x();
  deriv(0,3) = - arcvxyz
               + cvz * ( vy * vz * m_bField.y() + arcvy * m_bField.z() );
  deriv(0,4) = C * crossB.x();
  deriv(1,1) = 1.0;
  deriv(1,2) = - arcvxyz
               - cvz * ( vx * vz * m_bField.x() + arcvx * m_bField.z() );
  deriv(1,3) = svz * arcvy - cvz * vy * crossB.y();
  deriv(1,4) = C * crossB.y();
  deriv(2,2) = - vx * vz * svz
               + cvz * ( vx * vy * m_bField.x() + arcvx * m_bField.y() );
  deriv(2,3) = - vy * vz * svz
               - cvz * ( vx * vy * m_bField.y() + arcvy * m_bField.x() );
  deriv(2,4) = C * crossB.z();

  return deriv;        
};

// Not yet implemented
double StateTraj::distTo1stError( double , double , int ) const 
{
  return 10*km;  
};

// Not yet implemented
double StateTraj::distTo2ndError( double , double , int ) const
{
  return 10*km;  
};

// Not yet implemented  
Trajectory::Range StateTraj::range() const
{
        //FIXME: worry about this later...
  return Trajectory::Range(-10*km,10*km);
};

double StateTraj::length() const
{
  return range().second-range().first;
};
