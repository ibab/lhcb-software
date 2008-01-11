// $Id: 3DTransformationFunctions.cpp,v 1.2 2008-01-11 12:39:28 jpalac Exp $
// Include files 



// local
#include "DetDesc/3DTransformationFunctions.h"
#include "RotationZYX.h"
#include "3DConversions.h"

namespace DetDesc {

const Gaudi::Transform3D localToGlobalTransformation(const std::vector<double>& translationParams,
                                                     const std::vector<double>& rotationParams,
                                                     const std::vector<double>& pivotParams) 
{
  const ROOT::Math::Translation3D pivot    = ROOT::Math::Translation3D( pivotParams.begin(),
                                                                 pivotParams.end()   );
  const Gaudi::Transform3D rotation = DetDesc::ZYXRotation(rotationParams);
  const ROOT::Math::Translation3D translation = 
    ROOT::Math::Translation3D( translationParams.begin(),
                               translationParams.end()    );

  return translation*pivot*rotation*(pivot.Inverse());
  
}

const Gaudi::Transform3D ZYXRotation(const std::vector<double>& params)
{
  if (params.size() != 3) return Gaudi::Transform3D();
  Gaudi::RotationZYX rotZYX(params[2], params[1], params[0]);
  Gaudi::Rotation3D tmp;
  Gaudi::Math::convert(rotZYX, tmp);

  return Gaudi::Transform3D(tmp);
}

void getZYXTransformParameters(const Gaudi::Transform3D& CDM,
                               std::vector<double>& translationParams,
                               std::vector<double>& rotationParams,
                               const std::vector<double>& pivotParams) 
{

  Gaudi::Rotation3D newRot;
  Gaudi::TranslationXYZ newTrans;
  CDM.GetDecomposition(newRot, newTrans);
  const ROOT::Math::Translation3D pivotTrans = ROOT::Math::Translation3D( pivotParams.begin(),
                                                                          pivotParams.end()    );
  
  // Take the pivot out of the rotation.
  const Gaudi::Transform3D newRotPart = 
    pivotTrans.Inverse() * Gaudi::Transform3D(newRot) * pivotTrans;

  Gaudi::TranslationXYZ tmp;
  newRotPart.GetDecomposition(newRot, tmp);

  getZYXRotationParameters(newRot, rotationParams);

  getTranslationParameters(newTrans, translationParams);
  
}

void getZYXRotationParameters(const Gaudi::Rotation3D& rot,
                              std::vector<double>& rotParams)
{

  const Gaudi::RotationZYX rotation(rot);

  rotation.GetComponents(rotParams[2], rotParams[1], rotParams[0]);

  return;
}


void getTranslationParameters(const Gaudi::TranslationXYZ& trans,
                              std::vector<double>& transParams)
{

  transParams[0]=trans.X();
  transParams[1]=trans.Y();
  transParams[2]=trans.Z();
  
  return;
}
  
} // namespace DetDesc
//=============================================================================

