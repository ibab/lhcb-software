#include "TrackKernel/TrackVertexUtils.h"
#include <GaudiKernel/GenericVectorTypes.h>

namespace LHCb
{
  namespace TrackVertexUtils {

    inline Gaudi::Vector3 transform( const Gaudi::XYZVector& vec)
    {
      return Gaudi::Vector3(vec.X(),vec.Y(),vec.Z()) ;
    }

    int computeChiSquare(const Gaudi::XYZPoint&  pos,
			 const Gaudi::XYZVector& vec,
			 const Gaudi::SymMatrix6x6& cov6,
			 const Gaudi::XYZPoint& motherpos,
			 const Gaudi::SymMatrix3x3& mothercov,
			 double& chi2,
			 double& decaylength,
			 double& decaylengtherr) 
    {
      // pos:  decay vertex of particle
      // vec:  direction or momentum of particle (does not need to be normalized)
      // cov6: corresponding covariance matrix
      


      // This calculation is basically a 1-iteration beamspot fit. The
      // constraint is 
      //
      //    r = x - lambda p/|p| - xbs
      // 
      // where x and p are the position of the decay vertex of the
      // candidate and its momentum, lambda is the decaylength and xbs
      // the position of the beamspot. The covariance in the constraint
      // is
      //
      //    V = Vbs + Vxx - a * Vxp - a Vxp^T + a^2 * Vpp
      //
      // where a=lambda/|p|^2. It needs an initial estimate for the
      // flightlength, for which we simply take the projection of deltaX
      // on the direction. We now minimize  the chisquare contribution 
      // 
      //     chi^2 = r^T V^{-1} r
      //
      // for lambda.
      
      Gaudi::Vector3 dx = transform(pos -  motherpos) ;
      double p3mag = vec.R() ; 
      Gaudi::Vector3 dir = transform(vec.Unit()) ;
  
      Gaudi::SymMatrix3x3 W = mothercov ;

      double a = ROOT::Math::Dot(dir,dx)/p3mag ;
      for(size_t row=0; row<3; ++row)
	for(size_t col=0; col<=row; ++col)
	  W(row,col) += 
	    cov6(row,col)
	    + a*a * cov6(row+3,col+3)
	    - a * (cov6(row+3,col) + cov6(col+3,row)) ;
      
      int OK = W.Invert() ;
      
      double halfdChi2dLam2 = ROOT::Math::Similarity(W,dir) ;
      decaylength    = ROOT::Math::Dot(dir,W*dx)/halfdChi2dLam2 ;
      decaylengtherr = std::sqrt(1/halfdChi2dLam2) ;
      
      Gaudi::Vector3 res = dx - decaylength * dir ;
      
      chi2 = ROOT::Math::Similarity(W,res) ;

      return OK ;
    }
  } 
}
