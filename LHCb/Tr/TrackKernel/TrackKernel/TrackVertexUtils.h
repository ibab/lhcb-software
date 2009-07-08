// this really belongs somewhere else, for example in LHCbMath
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

namespace LHCb
{

  namespace TrackVertexUtils {
    
    
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
		       
