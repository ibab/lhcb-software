// $Id: GlobalToLocalDelta.cpp,v 1.1 2007-05-31 14:46:55 jpalac Exp $
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
                                          const Gaudi::Transform3D& globalDelta) {
    
  const Gaudi::Transform3D totalMatrix       = GI->matrixInv();
  const Gaudi::Transform3D idealMatrix       = GI->idealMatrixInv();
  const Gaudi::Transform3D localToNominalOld = GI->localDeltaMatrix();
  return totalMatrix*localToNominalOld*idealMatrix*globalDelta;
    
}
  

  
} // namespace DetDesc
//=============================================================================
