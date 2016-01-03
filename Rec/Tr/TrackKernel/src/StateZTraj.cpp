// $Id: StateZTraj.cpp,v 1.1 2009-07-08 13:33:45 wouter Exp $
// Include files

// local
#include "TrackKernel/StateZTraj.h"

namespace LHCb
{
  void StateZTraj::expansion( double z, Gaudi::XYZPoint& p, Gaudi::XYZVector& dp, Gaudi::XYZVector& ddp ) const 
  {
    p.SetXYZ(   x(z), y(z), z );
    dp.SetXYZ(  tx(z), ty(z), 1 ) ;
    ddp.SetXYZ( omegax(z), omegay(z), 0 );
  }

  double StateZTraj::muEstimate( const Gaudi::XYZPoint& p ) const 
  {
    Gaudi::XYZVector dir = direction(p.z()) ;
    Gaudi::XYZVector dx  = p - position(p.z()) ;
    double dir2 = dir.mag2() ;
    double det  = dir2 - curvature(p.z()).Dot(dx) ;
    if(det<=0) det = dir2 ;
    return p.z() + dx.Dot(dir)/det ;
  }
  
  StateZTraj::Parameters StateZTraj::parameters() const
  {
    return { m_cx[0], m_cy[0], m_cx[1], m_cy[1], m_qOverP };
  }

  LHCb::StateVector StateZTraj::stateVector( double z ) const
  {
    Gaudi::TrackVector parameters(x(z),y(z),tx(z),ty(z),m_qOverP) ;
    return { parameters, z };
  }

  StateZTraj& StateZTraj::operator+=(const Parameters& /*delta*/)
  {
    // to implement this we need the full b-field. 
    std::cout << "StateZTraj::operator+= not yet implemented." << std::endl ;
    return *this ;
  }
  

  StateZTraj::Derivative StateZTraj::derivative( double z ) const
  {
    Derivative deriv;
    double dz = z - m_z ;
    deriv(0,0) = deriv(1,1) = 1;
    deriv(0,2) = deriv(1,3) = dz ;
    
    // to speed this up, we only calculate the rest if the trajectory is indeed curved.
    bool isnonlinear = fabs(m_cx[2]) > 1e-10 || fabs(m_cy[2]) > 1e-10 ;
    if( isnonlinear ) {
      double tx = m_cx[1] ;
      double ty = m_cy[1] ;
      double omegax = m_cx[2] ;
      double omegay = m_cy[2] ;
      double n  = std::sqrt(1 + tx*tx + ty*ty ) ;
      double dndtx = tx/n ;
      double dndty = ty/n ;

      double half_dz_sqr = 0.5 * dz * dz;
      
      deriv(0,2) += half_dz_sqr *  omegax/n * dndtx ;
      deriv(0,3) += half_dz_sqr * (omegax/n * dndty + n * m_qOverP * Gaudi::Units::c_light * m_Bz ) ;
      deriv(0,4) += half_dz_sqr *  omegax/m_qOverP ;
      
      deriv(1,2) += half_dz_sqr * (omegax/n * dndtx - n * m_qOverP * Gaudi::Units::c_light * m_Bz ) ;
      deriv(1,3) += half_dz_sqr *  omegax/n * dndty ;
      deriv(1,4) += half_dz_sqr *  omegay/m_qOverP ;
    }
    
    return deriv;        
  }
}
