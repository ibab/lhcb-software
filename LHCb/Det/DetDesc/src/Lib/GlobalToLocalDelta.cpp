// $Id: GlobalToLocalDelta.cpp,v 1.9 2007-06-22 12:22:36 jpalac Exp $
// Include files 
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// local
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "RotationZYX.h"
#include "3DConversions.h"
namespace DetDesc {

const Gaudi::Transform3D localDeltaMatrix(const IDetectorElement* DE,
                                          const Gaudi::Transform3D& globalDelta) {
  return localDeltaMatrix(DE->geometry(), globalDelta);
}

const Gaudi::Transform3D localDeltaMatrix(const IGeometryInfo* GI,
                                          const Gaudi::Transform3D& toMisaligned_global) {

  const Gaudi::Transform3D T       = GI->matrixInv();
  const Gaudi::Transform3D T_inv   = T.Inverse();
  const Gaudi::Transform3D N_0_inv = GI->localMatrix();
  const Gaudi::Transform3D d_0_inv = GI->localDeltaMatrix();
  const Gaudi::Transform3D N_0     = N_0_inv.Inverse();
  const Gaudi::Transform3D d_0     = d_0_inv.Inverse();

  return d_0 * N_0 * T_inv * toMisaligned_global * T * N_0_inv;
  
}

void localDeltaParameters(const IDetectorElement* DE,
                          std::vector<double>& trans,
                          std::vector<double>& rot,
                          std::vector<double>& pivot) {

  DetDesc::localDeltaParameters(DE->geometry(), trans, rot, pivot);

}

void localDeltaParameters(const IGeometryInfo* GI,
                          std::vector<double>& trans,
                          std::vector<double>& rot,
                          std::vector<double>& pivot) {
  
  const Gaudi::Transform3D globalDelta = DetDesc::localToGlobalTransformation(trans, rot, pivot);
  const Gaudi::Transform3D localDelta  = DetDesc::localDeltaMatrix(GI, globalDelta);
  // get the pivot from the alignment condition
  // @todo find a way not to require this awful hard-wired string.
  const std::string pivotString("pivotXYZ");

  std::vector<double> tmp = (GI->alignmentCondition()->exists(pivotString) ) ? 
    GI->alignmentCondition()->paramAsDoubleVect(pivotString) : std::vector<double>(3, 0);

  std::copy( tmp.begin(), tmp.end(), pivot.begin() );

  DetDesc::getZYXTransformParameters(localDelta, trans, rot, pivot);

}
  
} // namespace DetDesc
//=============================================================================
