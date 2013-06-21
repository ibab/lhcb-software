#include "GaudiKernel/Point3DTypes.h"
#include "Event/State.h"

namespace LHCb
{

  namespace TrackVertexUtils {

    ///////////////////////////////////////////////////////////////////////////
    /// Return the chi2 of a track state with respect to a
    /// vertex. This is also known as the 'IPCHI2'.
    ///////////////////////////////////////////////////////////////////////////
    double vertexChi2(const LHCb::State& state, 
		      const Gaudi::XYZPoint& vertexpos,
		      const Gaudi::SymMatrix3x3& vertexcov) ;
    
    ///////////////////////////////////////////////////////////////////////////
    /// Return the chi2 of the vertex of two track states
    ///////////////////////////////////////////////////////////////////////////
    double vertexChi2( const LHCb::State& stateA, const LHCb::State& stateB ) ;

    
    ///////////////////////////////////////////////////////////////////////////
    /// Return the doca between two track states
    ///////////////////////////////////////////////////////////////////////////
    double doca( const LHCb::State& stateA, const LHCb::State& stateB ) ;
    
    /////////////////////////////////////////////////////////////////////////
    /// Compute the chi2 and decaylength of a 'particle' with respect
    /// to a vertex. This should probably go into LHCb math.
    /////////////////////////////////////////////////////////////////////////
    int computeChiSquare(const Gaudi::XYZPoint&  pos,
			 const Gaudi::XYZVector& mom,
			 const Gaudi::SymMatrix6x6& cov6,
			 const Gaudi::XYZPoint& motherpos,
			 const Gaudi::SymMatrix3x3& mothercov,
			 double& chi2,
			 double& decaylength,
			 double& decaylengtherr) ;
  }
}
		       
