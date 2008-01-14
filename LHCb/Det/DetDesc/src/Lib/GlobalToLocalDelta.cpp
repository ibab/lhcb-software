// $Id: GlobalToLocalDelta.cpp,v 1.17 2008-01-14 13:23:01 jpalac Exp $
// Include files 
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// local
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/3DTransformationFunctions.h"
namespace DetDesc {

const Gaudi::Transform3D localDeltaMatrix(const IDetectorElement* DE,
                                          const Gaudi::Transform3D& globalDelta) {
  return localDeltaMatrix(DE->geometry(), globalDelta);
}

const Gaudi::Transform3D localDeltaMatrix(const IGeometryInfo* GI,
                                          const Gaudi::Transform3D& toMisaligned_global) {

  const Gaudi::Transform3D d_0 = GI->ownToOffNominalMatrix();
  const Gaudi::Transform3D T   = GI->toGlobalMatrix();
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
  const Gaudi::Transform3D d_0 = GI->ownToOffNominalMatrix();
  const Gaudi::Transform3D T_inv   = GI->toLocalMatrix();
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
  const Gaudi::Transform3D d_0 = GI->ownToOffNominalMatrix();
  const Gaudi::Transform3D T_parent = parentGI->toGlobalMatrix();
  const Gaudi::Transform3D T_k2n_inv   = GI->toLocalMatrix() * T_parent;
  return d_0 * T_k2n_inv * parentTransform;
}

const Gaudi::Transform3D localDeltaFromAnyFrame(const Gaudi::Transform3D& frame,
                                                const IDetectorElement* DE,
                                                const Gaudi::Transform3D& toGlobalInFrame) {

  return localDeltaFromAnyFrame(frame, DE->geometry(), toGlobalInFrame);
  
}
  
const Gaudi::Transform3D localDeltaFromAnyFrame(const Gaudi::Transform3D& frame,
                                                const IGeometryInfo* GI,
                                                const Gaudi::Transform3D& toGlobalInFrame) {

  const Gaudi::Transform3D d_0 = GI->ownToOffNominalMatrix();
  const Gaudi::Transform3D T_k2n_inv   = GI->toLocalMatrix() * frame;
  return d_0 * T_k2n_inv * toGlobalInFrame;  
  
}

const Gaudi::Transform3D localDeltaFromAnyFrameDelta(const Gaudi::Transform3D& frame,
                                                     const IDetectorElement* DE,
                                                     const Gaudi::Transform3D& deltaInFrame) {

  return localDeltaFromAnyFrame(frame, DE->geometry(), deltaInFrame);
  
}
  
const Gaudi::Transform3D localDeltaFromAnyFrameDelta(const Gaudi::Transform3D& frame,
                                                     const IGeometryInfo* GI,
                                                     const Gaudi::Transform3D& deltaInFrame) {

  const Gaudi::Transform3D d_0 = GI->ownToOffNominalMatrix();
  const Gaudi::Transform3D T_k2n_inv   = GI->toLocalMatrix() * frame;
  return d_0 * frame.Inverse() * deltaInFrame * frame;  
  
}

const Gaudi::Transform3D globalDeltaFromLocalDeltaPrime(const IDetectorElement*   DE,
                                                        const Gaudi::Transform3D& deltaPrime)
{
  return  globalDeltaFromLocalDeltaPrime(DE->geometry(), deltaPrime);
}

const Gaudi::Transform3D globalDeltaFromLocalDeltaPrime(const IGeometryInfo*      GI,
                                                        const Gaudi::Transform3D& deltaPrime)
{
  return  GI->toGlobalMatrix()*deltaPrime*GI->toLocalMatrix();
}

const Gaudi::Transform3D globalDelta(const IDetectorElement* DE)
{
  return  globalDelta( DE->geometry() );
}

const Gaudi::Transform3D globalDelta(const IGeometryInfo* GI)
{
  return  GI->toGlobalMatrix()*GI->ownToOffNominalMatrix()*GI->toLocalMatrix();
}

const Gaudi::Transform3D relativeTransformation(const IDetectorElement* referenceDE,
                                                const IDetectorElement* DE)
{
  return referenceDE->geometry()->toLocalMatrix()*DE->geometry()->toGlobalMatrix();
}
  
} // namespace DetDesc
//=============================================================================
