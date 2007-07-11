// $Id: GlobalToLocalDelta.cpp,v 1.13 2007-07-11 16:36:35 jpalac Exp $
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

  const Gaudi::Transform3D d_0 = GI->localDeltaMatrix().Inverse();
  const Gaudi::Transform3D T   = GI->matrixInv();
  return d_0 * T.Inverse() * toMisaligned_global * T;

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

const Gaudi::Transform3D localDeltaFromGlobalTransform(const IDetectorElement* DE,
                                                       const Gaudi::Transform3D& globalTransform) {
  return localDeltaFromGlobalTransform(DE->geometry(), globalTransform);
}

const Gaudi::Transform3D localDeltaFromGlobalTransform(const IGeometryInfo* GI,
                                                       const Gaudi::Transform3D& globalTransform) {
  const Gaudi::Transform3D d_0 = GI->localDeltaMatrix().Inverse();
  const Gaudi::Transform3D T_inv   = GI->matrix();
  return d_0 * T_inv * globalTransform;
}

const Gaudi::Transform3D localDeltaFromParentTransform(const IDetectorElement* parentDE,
                                                       const IDetectorElement* DE,
                                                       const Gaudi::Transform3D& parentTransform) {
  return localDeltaFromParentTransform(parentDE->geometry(),
                                       DE->geometry(), 
                                       parentTransform);
}

const Gaudi::Transform3D localDeltaFromParentTransform(const IGeometryInfo* parentGI,
                                                       const IGeometryInfo* GI,
                                                       const Gaudi::Transform3D& parentTransform) {
  const Gaudi::Transform3D d_0 = GI->localDeltaMatrix().Inverse();
  const Gaudi::Transform3D T_parent = parentGI->matrixInv();
  const Gaudi::Transform3D T_k2n_inv   = GI->matrix() * T_parent;
  return d_0 * T_k2n_inv * parentTransform;
}
  
} // namespace DetDesc
//=============================================================================
