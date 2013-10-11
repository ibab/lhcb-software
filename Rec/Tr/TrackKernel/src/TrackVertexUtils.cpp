#include "TrackKernel/TrackVertexUtils.h"

#ifdef __INTEL_COMPILER         // Disable ICC remark from ROOT GenVector classes
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
#endif

#include <GaudiKernel/GenericVectorTypes.h>

namespace LHCb
{
  namespace TrackVertexUtils {

    ///////////////////////////////////////////////////////////////////////////
    /// Return the chi2 of a track state with respect to a
    /// vertex. This is also known as the 'IPCHI2'.
    ///////////////////////////////////////////////////////////////////////////
    double vertexChi2(const LHCb::State& state, 
		      const Gaudi::XYZPoint& vertexpos,
		      const Gaudi::SymMatrix3x3& vertexcov)
    {  
      double tx = state.tx() ;
      double ty = state.ty() ;
      double dz = vertexpos.z() - state.z() ;
      double dx = state.x() + dz * tx - vertexpos.x() ;
      double dy = state.y() + dz * ty - vertexpos.y() ;
      const Gaudi::SymMatrix5x5& trkcov = state.covariance() ;
      
      // compute the covariance matrix. first only the trivial parts:
      double cov00 = vertexcov(0,0) + trkcov(0,0) ;
      double cov10 = vertexcov(1,0) + trkcov(1,0) ;
      double cov11 = vertexcov(1,1) + trkcov(1,1) ;
      
      // add the contribution from the extrapolation
      cov00 += dz*dz*trkcov(2,2) + 2*dz*trkcov(2,0) ;
      cov10 += dz*dz*trkcov(3,2) + dz*(trkcov(3,0)+trkcov(2,1)) ;
      cov11 += dz*dz*trkcov(3,3) + 2*dz*trkcov(3,1) ;
      
      // add the contribution from pv Z
      cov00 += tx*tx*vertexcov(2,2)  -  2*tx*vertexcov(2,0) ;
      cov10 += tx*ty*vertexcov(2,2)  -  ty*vertexcov(2,0) - tx*vertexcov(2,1) ;
      cov11 += ty*ty*vertexcov(2,2)  -  2*ty*vertexcov(2,1) ;
      
      // invert the covariance matrix
      double D = cov00*cov11 - cov10*cov10 ;
      double invcov00 = cov11 / D ;
      double invcov10 = -cov10 / D ;
      double invcov11 = cov00 / D ;
      
      return dx*dx * invcov00 + 2*dx*dy * invcov10 + dy*dy * invcov11 ;
    }
 
    ///////////////////////////////////////////////////////////////////////////
    /// Return the chi2 of the vertex of two track states
    ///////////////////////////////////////////////////////////////////////////
    double vertexChi2( const LHCb::State& stateA, const LHCb::State& stateB )
    {
      // first compute the cross product of the directions. we'll need this in any case
      const double txA = stateA.tx() ;
      const double tyA = stateA.ty() ;
      const double txB = stateB.tx() ;
      const double tyB = stateB.ty() ;
      double nx = tyA - tyB ; //   y1 * z2 - y2 * z1
      double ny = txB - txA ; // - x1 * z2 + x2 * z1
      double nz = txA * tyB - tyA * txB ; //   x1 * y2 - x2 * y1
      // double n2 = nx*nx + ny*ny + nz*nz ;
      
      // compute doca. we don't divide by the normalization to save time. we call it 'ndoca'
      double dx = stateA.x() - stateB.x() ;
      double dy = stateA.y() - stateB.y() ;
      double dz = stateA.z() - stateB.z() ;
      double ndoca = dx*nx + dy*ny + dz*nz;
      
      // the hard part: compute the jacobians :-)
      Gaudi::Vector4 jacA, jacB ;
      jacA(0) = nx ;
      jacA(1) = ny ;
      jacA(2) = -dy + dz*tyB ;
      jacA(3) = +dx - dz*txB ;
      jacB(0) = -nx ;
      jacB(1) = -ny ;
      jacB(2) = +dy - dz*tyA ;
      jacB(3) = -dx + dz*txA ;
      
      // compute the variance on ndoca
      const Gaudi::TrackSymMatrix& covA = stateA.covariance() ;
      const Gaudi::TrackSymMatrix& covB = stateB.covariance() ;    
      double varndoca = 
	ROOT::Math::Similarity( jacA, covA.Sub<Gaudi::SymMatrix4x4>(0,0) ) +
	ROOT::Math::Similarity( jacB, covB.Sub<Gaudi::SymMatrix4x4>(0,0) ) ;
      
      // return the chi2
      return ndoca*ndoca / varndoca ;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Return the doca between two track states
    ///////////////////////////////////////////////////////////////////////////
    double doca( const LHCb::State& stateA, const LHCb::State& stateB ) 
    {
      // first compute the cross product of the directions.
      const double txA = stateA.tx() ;
      const double tyA = stateA.ty() ;
      const double txB = stateB.tx() ;
      const double tyB = stateB.ty() ;
      double nx = tyA - tyB ; //   y1 * z2 - y2 * z1
      double ny = txB - txA ; // - x1 * z2 + x2 * z1
      double nz = txA * tyB - tyA * txB ; //   x1 * y2 - x2 * y1
      double n  = std::sqrt(nx*nx + ny*ny + nz*nz) ;
      // compute the doca
      double dx = stateA.x() - stateB.x() ;
      double dy = stateA.y() - stateB.y() ;
      double dz = stateA.z() - stateB.z() ;
      double ndoca = dx*nx + dy*ny + dz*nz;
      return ndoca / n ;
    }   

    /////////////////////////////////////////////////////////////////////////
    /// Compute the chi2 and decaylength of a 'particle' with respect
    /// to a vertex. This should probably go into LHCb math.
    /////////////////////////////////////////////////////////////////////////
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
    
    /////////////////////////////////////////////////////////////////////////
    /// Compute the point of the doca of two track states.  Return 1 if successful.
    /////////////////////////////////////////////////////////////////////////
    int poca( const LHCb::State& stateA, const LHCb::State& stateB,
	      Gaudi::XYZPoint& vertex)
    {
      // find the z-positions of the doca. then take the average of the two points.
      // see also Gaudi::Math::closestPointParams.
      const double zA = stateA.z() ;
      const double xA = stateA.x() ;
      const double yA = stateA.y() ;
      const double txA = stateA.tx() ;
      const double tyA = stateA.ty() ;
      const double zB = stateB.z() ;
      const double xB = stateB.x() ;
      const double yB = stateB.y() ;
      const double txB = stateB.tx() ;
      const double tyB = stateB.ty() ;
      
      // define d^2 = ( xA + muA*txA - xB - muB*txB)^2 +  ( yA + muA*tyA - xB - muB*tyB)^2 + (zA + muA - zB - muB)^2
      // compute (half) 2nd derivative to muA and muB in point muA=muB=0
      double secondAA = txA*txA + tyA*tyA + 1;
      double secondBB = txB*txB + tyB*tyB + 1;
      double secondAB = -txA*txB - tyA*tyB - 1;
      // compute inverse matrix, but stop if determinant not positive
      double det = secondAA*secondBB - secondAB*secondAB ;
      int OK = 1 ;
      if ( std::abs(det) > 0 ) {
	double secondinvAA = secondBB / det ;
	double secondinvBB = secondAA / det ;
	double secondinvAB = -secondAB / det ;
	// compute (half) first derivative
	double firstA =  txA * ( xA - xB ) + tyA * (yA - yB ) + (zA - zB) ;
	double firstB = -txB * ( xA - xB ) - tyB * (yA - yB ) - (zA - zB) ;
	// compute muA and muB with delta-mu = - seconderivative^-1 * firstderivative
	double muA = - (secondinvAA * firstA + secondinvAB * firstB ) ;
	double muB = - (secondinvBB * firstB + secondinvAB * firstA ) ;
	// return the average point
	vertex.SetX( 0.5*(xA + muA*txA + xB + muB*txB) ) ;
	vertex.SetY( 0.5*(yA + muA*tyA + yB + muB*tyB) ) ;
	vertex.SetZ( 0.5*(zA + muA     + zB + muB    ) ) ;
      } else {
	OK = 0 ; // parallel tracks
      }
      return OK ;
    }
  } 
}
