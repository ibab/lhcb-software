// $Id: GlobalToLocalDelta.h,v 1.2 2007-06-01 15:29:32 jpalac Exp $
#ifndef DETDESC_GLOBALTOLOCALDELTA_H 
#define DETDESC_GLOBALTOLOCALDELTA_H 1

// Include files
#include "GaudiKernel/Transform3DTypes.h"

// forward declarations
class IDetectorElement;
class IGeometryInfo;

/** @namespace DetDesc  GlobalToLocalDelta.h DetDesc/GlobalToLocalDelta.h
 *  
 *  Free functions to go from a 3D global "delta" matrix to the
 *  corresponding matrix in the frame of the detector element's 
 *  parent.
 *
 *  @author Juan Palacios
 *  @date   2007-05-31
 *  @todo Put this and other tools in some other place?
 */
namespace DetDesc {


  /**
   * @param  DE detector element in question.
   * @param  globalDelta 3D transformation that takes 
   *         global nominal position matrix and applies misalignment:
   *         Misalinged position matrix = globalDelta*M_nominal_global
   * @return 3D transformation that applies misalignment from nominal
   *         position in the frame of the detector element's parent.
   * @date 2007-05-31
   * @author Juan Palacios juan.palacios@cern.ch
   *
  */
  const Gaudi::Transform3D localDeltaMatrix(const IDetectorElement* DE,
                                            const Gaudi::Transform3D& globalDelta);  
  
  /**
   * @param  GI IGeometryInfo from detector element in question.
   * @param  globalDelta 3D transformation that takes 
   *         global nominal position matrix and applies misalignment:
   *         Misalinged position matrix = globalDelta*M_nominal_global
   * @return 3D transformation that applies misalignment from nominal
   *         position in the frame of the detector element's parent.
   * @date 2007-05-31
   * @author Juan Palacios juan.palacios@cern.ch
   *
  */
  const Gaudi::Transform3D localDeltaMatrix(const IGeometryInfo* GI,
                                            const Gaudi::Transform3D& globalDelta);

  /**
   * Build the local to global transformation matrix given 
   *
   * @param pivotParams vector containing the X,Y,Z coordinates of a pivot point for the
   *        rotation part of the transformation.
   * @param translationParams vector containing the X,Y,Z coordinates of the translation
   *        part of the transformation
   * @param rotationParams vector containing the rotation angles about the X,Y,Z axes. 
   *        The rotation is applied in the following order: Z-Y'-X"
   * @return The full 3D transformation object. 
   * @author Juan Palacios juan.palacios@cern.ch
   * @todo Move this function outside of DetDesc once the ROOT::Math::RotationZYX class
   *       is fully available in MathCore.
   */
  const Gaudi::Transform3D localToGlobalTransformation(std::vector<double>& pivotParams,
                                                       std::vector<double>& translationParams,
                                                       std::vector<double>& rotationParams);
  /**
   * Make a 3D transformation object from a simple 3D translation
   * 
   * @author Juan Palacios juan.palacios@cern.ch
   *
   * @param params vector containing the X,Y,Z coordinates of the translation.
   * @return 3D transformation object representing the translation.
   * @todo Move this function outside of DetDesc once the ROOT::Math::RotationZYX class              
   *       is fully available in MathCore. 
   */
  const Gaudi::Transform3D XYZTranslation(const std::vector<double>& params);
  /**
   * Build a 3D rotation from three angles.
   *
   * @author Juan Palacios juan.palacios@cern.ch
   *
   * @param params vector containing the rotations about the X,Y,Z axes. The rotations are applied
   *        in the following order: Z-Y'-X"
   * @return 3D transformation object describing the rotation
   * @todo Move this function outside of DetDesc once the ROOT::Math::RotationZYX class              
   *       is fully available in MathCore.  
   *
   */
  const Gaudi::Transform3D ZYXRotation(const std::vector<double>& params);
  
}
#endif // DETDESC_GLOBALTOLOCALDELTA_H
