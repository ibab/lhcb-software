// $Id: GlobalToLocalDelta.cpp,v 1.2 2007-05-31 16:53:30 jpalac Exp $
// Include files 
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// local
#include "DetDesc/GlobalToLocalDelta.h"
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
  

  
} // namespace DetDesc
//=============================================================================
