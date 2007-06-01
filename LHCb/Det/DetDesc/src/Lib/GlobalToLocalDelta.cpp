// $Id: GlobalToLocalDelta.cpp,v 1.3 2007-06-01 15:29:32 jpalac Exp $
// Include files 
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// local
#include "DetDesc/GlobalToLocalDelta.h"
#include "RotationZYX.h"
#include "3DConversions.h"
namespace DetDesc {

const Gaudi::Transform3D localDeltaMatrix(const IDetectorElement* DE,
                                          const Gaudi::Transform3D& globalDelta) {
  return localDeltaMatrix(DE->geometry(), globalDelta);
}

const Gaudi::Transform3D localDeltaMatrix(const IGeometryInfo* GI,
                                          const Gaudi::Transform3D& toMisaligned_global) {

  const Gaudi::Transform3D toLocal_total       = GI->matrix();
  const Gaudi::Transform3D toGlobal_ideal       = GI->idealMatrixInv();
  const Gaudi::Transform3D toMisalignedOld_local = GI->localDeltaMatrix().Inverse();
  return toMisalignedOld_local*toLocal_total*toGlobal_ideal*toMisaligned_global;
  
}
// @todo move these to a general Transform3D tools suite.  
const Gaudi::Transform3D localToGlobalTransformation(std::vector<double>& pivotParams,
                                                     std::vector<double>& translationParams,
                                                     std::vector<double>& rotationParams) 
{
  const Gaudi::Transform3D pivot    = DetDesc::XYZTranslation(pivotParams);
  const Gaudi::Transform3D rotation = DetDesc::ZYXRotation(rotationParams);
  const Gaudi::Transform3D translation = DetDesc::XYZTranslation(translationParams);

  return translation*pivot.Inverse()*rotation*pivot;

}

const Gaudi::Transform3D XYZTranslation(const std::vector<double>& params) 
{
  return (params.size()==3) ? Gaudi::Transform3D( Gaudi::TranslationXYZ(params[0], 
                                                                        params[1], 
                                                                        params[2]) )
    : Gaudi::Transform3D();
  
}

const Gaudi::Transform3D ZYXRotation(const std::vector<double>& params)
{
  if (params.size() != 3) return Gaudi::Transform3D();
  
  Gaudi::RotationZYX rotZYX(params[2], params[1], params[0]);
  Gaudi::Rotation3D tmp;
  Gaudi::Math::convert(rotZYX, tmp);

  return Gaudi::Transform3D(tmp);
}
  
} // namespace DetDesc
//=============================================================================
