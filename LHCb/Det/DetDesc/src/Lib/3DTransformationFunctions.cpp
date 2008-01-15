// $Id: 3DTransformationFunctions.cpp,v 1.5 2008-01-15 14:45:25 jpalac Exp $
// Include files 



// local
#include "DetDesc/3DTransformationFunctions.h"

namespace DetDesc {

const Gaudi::Transform3D localToGlobalTransformation(const std::vector<double>& translationParams,
                                                     const std::vector<double>& rotationParams,
                                                     const std::vector<double>& pivotParams) 
{
  const ROOT::Math::Translation3D pivot = ROOT::Math::Translation3D( pivotParams.begin(),
                                                                     pivotParams.end()   );

  const ROOT::Math::RotationZYX rotation = ROOT::Math::RotationZYX( rotationParams[2],
                                                                    rotationParams[1],
                                                                    rotationParams[0]  );
  const ROOT::Math::Translation3D translation = 
    ROOT::Math::Translation3D( translationParams.begin(),
                               translationParams.end()    );

  return Gaudi::Transform3D( translation*pivot*rotation*(pivot.Inverse()) );
  
}

void getZYXTransformParameters(const Gaudi::Transform3D& CDM,
                               std::vector<double>& translationParams,
                               std::vector<double>& rotationParams,
                               const std::vector<double>& pivotParams) 
{

  ROOT::Math::RotationZYX newRot = CDM.Rotation<ROOT::Math::RotationZYX>();

  const ROOT::Math::Translation3D newTrans = CDM.Translation();

  const ROOT::Math::Translation3D pivotTrans = 
    ROOT::Math::Translation3D( pivotParams.begin(), pivotParams.end()    );
  

  const Gaudi::Transform3D newRotPart = 
    pivotTrans.Inverse() * newRot * pivotTrans;

  newRot = newRotPart.Rotation<ROOT::Math::RotationZYX>();

  newRot.GetComponents(rotationParams[2], rotationParams[1], rotationParams[0]);

  newTrans.GetComponents( translationParams.begin(), translationParams.end() );
  
}
  
} // namespace DetDesc
//=============================================================================

