// $Id: 3DTransformationFunctions.h,v 1.6 2009-11-13 10:07:31 wouter Exp $
#ifndef DETDESC_3DTRANSFORMATIONFUNCTIONS_H 
#define DETDESC_3DTRANSFORMATIONFUNCTIONS_H 1

// Include files
#include "GaudiKernel/Transform3DTypes.h"
#include "Math/Translation3D.h"
#include "Math/RotationZYX.h"
#include "Math/SMatrix.h"
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
   */
  const Gaudi::Transform3D localToGlobalTransformation(const std::vector<double>& translationParams,
                                                       const std::vector<double>& rotationParams,
                                                       const std::vector<double>& pivotParams);


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
   * Starting from any MathCore supported 3D rotation, obtain the equivalent
   * Euler 321 rotation parameters.
   * Euler 321: Rotation about Z, Y', X" (co-moving axes)
   *
   * @param rotation Any Mathcore-supported 3D rotation
   * @param rotX rotation angle about X" axis
   * @param rotY rotation angle about Y' axis
   * @param rotZ rotation angle about Z axis
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 2008-01-14
   * 
   */

  template <class ROT>
  void getZYXRotationParameters( const ROT& rotation,
                                 double& rotX, 
                                 double& rotY, 
                                 double& rotZ           ) 
  {
    ROOT::Math::RotationZYX euler321 = ROOT::Math::RotationZYX(rotation);
    euler321.GetComponents(rotZ, rotY, rotX); 
  }
  
   /**
    * Returns a Jacobian for a given transform. This you need to
    * transform alignment derivatives from one frame to
    * another. Suppose f( x ) is the transform corresponding to 6
    * alignment parameters (e.g. deltatranslations and rotations).
    * This routine returns the jacobian for a transformation of f: if
    * x' is the delta in frame 0 and x is the delta in frame 1 and A
    * is the transform from frame 0 to frame 1, then this returns the
    * matrix Mij = dx_i / dx'_j, by taking the derivative of
    *  
    *   f'(x') = A * f(x) * A^{-1}
    *
    * Currently the application is transforming the alignment
    * derivatives computed by TrackProjector. Suppose that T is the
    * local-global transform. Suppose the derivative in the globa
    * frame is D_global which is a 1x6 matrix. To get the derivative
    * in the local frame, you do
    *
    * Transform3D localToGlobal = elements->geometry()->toGlobalMatrix()
    * Matri6x6    jacobian      = getTransformJacobian( localToGlobal ) ;
    * Matrix1x6   D_local       = D_global * jacobian
    * 
    * This is how this is used in the alignment and monitoring.
    *
    * @author Wouter Hulsbergen @nikhef.nl
    * 
    * @param CDM 3D transformation 
    * @return The 6x6 jacobian
    */ 
  ROOT::Math::SMatrix<double,6,6> getTransformJacobian( const Gaudi::Transform3D& transform ) ;
}

#endif // DETDESC_3DTRANSFORMATIONFUNCTIONS_H
