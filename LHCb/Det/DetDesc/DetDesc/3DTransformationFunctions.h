// $Id: 3DTransformationFunctions.h,v 1.2 2007-07-02 09:40:12 cattanem Exp $
#ifndef DETDESC_3DTRANSFORMATIONFUNCTIONS_H 
#define DETDESC_3DTRANSFORMATIONFUNCTIONS_H 1

// Include files
#include "GaudiKernel/Transform3DTypes.h"
#include <vector>

namespace DetDesc {

  /**
   * Build the local to global transformation matrix given 
   *
   * @author Juan Palacios
   * @date   2007-06-13
   *
   * @param translationParams vector containing the X,Y,Z coordinates of the translation
   *        part of the transformation
   * @param rotationParams vector containing the rotation angles about the X,Y,Z axes. 
   *        The rotation is applied in the following order: Z-Y'-X"
   * @param pivotParams vector containing the X,Y,Z coordinates of a pivot point for the
   *        rotation part of the transformation.
   * @return The full 3D transformation object. 
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @todo Move this function outside of DetDesc once the ROOT::Math::RotationZYX class
   *       is fully available in MathCore.
   */
  const Gaudi::Transform3D localToGlobalTransformation(const std::vector<double>& translationParams,
                                                       const std::vector<double>& rotationParams,
                                                       const std::vector<double>& pivotParams);

 
  /**
   * Make a 3D transformation object from a simple 3D translation
   * 
   * @author Juan Palacios juan.palacios@nikhef.nl
   *
   * @param params vector containing the X,Y,Z coordinates of the translation.
   * @return 3D transformation object representing the translation.
   * @todo Move this function outside of DetDesc if and when a 
   * ROOT::Math::Translation3D class available in MathCore. 
   */
  const Gaudi::Transform3D XYZTranslation(const std::vector<double>& params);

  /**
   * Build a 3D rotation from three angles.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   *
   * @param params vector containing the rotations about the X,Y,Z axes. 
   *               The rotations are applied in the following order: Z-Y'-X"
   * @return 3D transformation object describing the rotation
   * @todo Move this function outside of DetDesc once the 
   *       ROOT::Math::RotationZYX class is fully available in MathCore.  
   *
   */
  const Gaudi::Transform3D ZYXRotation(const std::vector<double>& params);

  /**
   * Get the three rotation angles and three translation coordinates from a 3D transformation
   * about a pivot point.
   * 
   * @author Juan Palacios juan.palacios@nikhef.nl
   * 
   * @param CDM 3D transformation object AKA cosine direction matrix
   * @param translationParams vector containing X,Y and Z of translation transformation.
   * @param rotationParams vector containing the rotations about the X,Y,Z axes. 
   *                       The rotations are applied in the following order: Z-Y'-X"
   * @param pivotParams    pivot point about which the 3D transformation is to be decomposed.
   *
   */
  void getZYXTransformParameters(const Gaudi::Transform3D& CDM,
                                 std::vector<double>& translationParams,
                                 std::vector<double>& rotationParams,
                                 const std::vector<double>& pivotParams = std::vector<double>(3,0.));

  /**
   *
   * Wrapper function to get the Z-X'-Y" rotation angles for a 3D rotation.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @param rot CDM 3D rotation
   * @param rotParams vector containing the resulting rotation angles.
   * @todo either remove this or template it based on rotatin representation
   *       depending on future evolution of MathCore.
   *
   */
  void getZYXRotationParameters(const Gaudi::Rotation3D& rot,
                                std::vector<double>& rotParams);

  /**
   * Wrapper function to get the translation parameters of a 3D translation into a vector.
   * @author Juan Palacios juan.palaicos@nikhef.nl
   */
  void getTranslationParameters(const Gaudi::TranslationXYZ& trans,
                                std::vector<double>& transParams);
  
}

#endif // DETDESC_3DTRANSFORMATIONFUNCTIONS_H
