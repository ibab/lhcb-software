#include "TrackKernel/CubicStateInterpolationTraj.h"

namespace LHCb
{

  LHCb::State CubicStateInterpolationTraj::state( double z ) const
  {
    Gaudi::TrackVector vec(x(z),y(z),tx(z),ty(z),qop(z)) ;
    return LHCb::State(vec, covariance(z), z,  LHCb::State::LocationUnknown) ;
  }
  
  Gaudi::TrackSymMatrix CubicStateInterpolationTraj::covariance( double z ) const
  {
    static Gaudi::TrackMatrix Fb = Gaudi::TrackMatrix( ROOT::Math::SMatrixIdentity() );
    Fb(0,2) = (z - zbegin()) ;
    Fb(1,3) = (z - zbegin()) ;
    
    static Gaudi::TrackMatrix Fe = Gaudi::TrackMatrix( ROOT::Math::SMatrixIdentity() );
    Fe(0,2) = (z - zend()) ;
    Fe(1,3) = (z - zend()) ;
    
    static Gaudi::TrackSymMatrix cov ;
    if( z <= zbegin() ) {
      cov = ROOT::Math::Similarity(Fb, *m_covbegin) ;
    } else if (z>= zend()) {
      cov = ROOT::Math::Similarity(Fe, *m_covend) ;
    } else {
      // what is the right weight? FIXME!
      double zfrac = (z - zbegin())/(zend()-zbegin()) ;
      double weight = (1-zfrac) ; //linear
      //double weight = 0.5 - 4*std::pow(zfrac-0.5,3) ; // cubic
      cov =  weight     * ROOT::Math::Similarity(Fb, *m_covbegin) ;
      cov += (1-weight) * ROOT::Math::Similarity(Fe, *m_covend) ;
    }
    
    return cov ;
  }
}
