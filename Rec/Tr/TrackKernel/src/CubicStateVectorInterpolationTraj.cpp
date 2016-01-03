#include "TrackKernel/CubicStateVectorInterpolationTraj.h"

namespace LHCb
{
  
  LHCb::StateVector CubicStateVectorInterpolationTraj::stateVector( double z ) const
  {
    return { { x(z),y(z),tx(z),ty(z),qop(z)}, z } ;
  }

  double CubicStateVectorInterpolationTraj::arclength(double /*z1*/, double /*z2*/) const 
  {
    std::cout << "CubicStateVectorInterpolationTraj::arclength not yet implemented" << std::endl ;
    return 0 ;
  }  

  double  CubicStateVectorInterpolationTraj::muEstimate(const Gaudi::XYZPoint& p) const {
    Gaudi::XYZVector dir = direction(p.z()) ;
    Gaudi::XYZVector dx  = p - position(p.z()) ;
    double dir2 = dir.mag2() ;
    double det  = dir2 - curvature(p.z()).Dot(dx) ;
    if(det<=0) det = dir2 ;
    return p.z() + dx.Dot(dir)/det ;
  }
}
