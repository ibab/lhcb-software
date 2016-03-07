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

  // This is the definition of the transform: (See the routine above).
  //
  //   A_tot    =   A_trans  * A_pivot  * A_rot * A_pivot^-1
  //
  // Note that 'A_trans' and 'A_pivot' are transforms that contain
  // only a translation, while A_rot applies exclusively a
  // rotation. We now need to compute A_trans and A_rot for given
  // A_tot and A_pivot.

  // Extracting the rotation is simple: Since there is only a single
  // rotation and translations do not change rotations, it must be
  // equal to the rotation part of the total transform:
  const ROOT::Math::RotationZYX newRot = CDM.Rotation<ROOT::Math::RotationZYX>();

  // To compute A_trans we now first create a pivot point transform
  const ROOT::Math::Translation3D pivotTrans = 
    ROOT::Math::Translation3D( pivotParams.begin(), pivotParams.end()    );
  
  // we then create ' A_pivot * A_rot * A_pivot^-1 ', so the term on
  // the right hand side in the equation above. (If you are worried
  // about precision, it would be better to compute directly its
  // inverse, because that is what we need below.)
  const ROOT::Math::Transform3D pivotRot = pivotTrans * newRot * pivotTrans.Inverse() ;

  // we then construct A_trans by multiplying A_tot on the right side
  // with the inverse of this thing. note that the result should not
  // have a rotation part anymore: it should be exclusively translation.
  const ROOT::Math::Translation3D newTrans = (CDM * pivotRot.Inverse()).Translation() ;
  
  // finally we extract the 6 parameters
  newRot.GetComponents(rotationParams[2], rotationParams[1], rotationParams[0]);
  newTrans.GetComponents( translationParams.begin(), translationParams.end() );
  
}

ROOT::Math::SMatrix<double,6,6> getTransformJacobian( const Gaudi::Transform3D& transform ) 
{

  // Suppose f( x ) is the transform corresponding to parameters x
  // (see routine AlParameters::transform).  This routine returns the
  // jacobian for a transformation of f: if x' is the delta in frame 0
  // and x is the delta in frame 1 and A is the transform from frame 0
  // to frame 1, then this returns the matrix Mij = dx_i / dx'_j, by taking the
  // derivative of
  //   f'(x')   =    A * f(x) * A^{-1}
  //

  typedef ROOT::Math::SMatrix<double, 3, 3> Matrix3x3 ;
  typedef ROOT::Math::SMatrix<double, 6, 6> Matrix6x6 ;
  typedef ROOT::Math::SVector<double, 3>    Vector3;

  // first decompose the transform. cannot do this quicker because of old ROOT version.
  Matrix3x3 R ;
  Vector3 T ;
  transform.GetComponents(R(0,0),R(0,1),R(0,2),T(0),
                          R(1,0),R(1,1),R(1,2),T(1),
                          R(2,0),R(2,1),R(2,2),T(2)) ;

  // We also need the inverse of R, which is of course just its transpose:
  Matrix3x3 Rinv = ROOT::Math::Transpose(R) ;
  
  // const matrices for derivative of rotations to rotation pars
  Matrix3x3 drot[3] ;
  drot[0](2,1) = drot[1](0,2) = drot[2](1,0) = 1 ;
  drot[0](1,2) = drot[1](2,0) = drot[2](0,1) = -1 ;
  
  // R ( deps ) R^{-1} for each three components
  Matrix3x3 RdrotR[3] ;
  //for(int i=0; i<3; ++i) RdrotR[i] = R * (drot[i] * Rinv) ;
  // this breaks in optimization on SLC5. the solution was to create a
  // temporary in the loop. (Thanks Lorenzo)
  for(int i=0; i<3; ++i) {
    Matrix3x3 tmp = drot[i] * Rinv ;
    RdrotR[i] = R * tmp ;
  }

  // now construct all components of the jacobian
  Matrix6x6 jacobian ;
  
  // the derivative of the rotation to the translation (deps/deta') is zero.

  // the derivative of the translation to the translation (deta/deta')
  jacobian.Place_at( R, 0, 0) ;
  
  // the derivatives to the rotation are a bit more complicated. let's
  // first do the rotation to the rotation:
  for(int i=0; i<3; ++i) {
    jacobian(3,3+i) = RdrotR[i](2,1) ;
    jacobian(4,3+i) = RdrotR[i](0,2) ;
    jacobian(5,3+i) = RdrotR[i](1,0) ;
  }

  // SO, if you just think about it for a minute, you realize that
  // also the delta-rotation should transform with the rotation:
  // //jacobian.Place_at( R, 3, 3) ;

  // the only thing left is the derivative of the translation to the
  // rotation. it's the only part that involves T:
  for(int i=0; i<3; ++i) {
    Vector3 tmp = RdrotR[i] * T ;
    for(int j=0; j<3; ++j) jacobian(j,i+3) = - tmp(j) ;
  }
  
  // Het staat er nu zo kort, dat je geen idee meer hebt hoeveel werk
  // de algebra was ...
  return jacobian ;
}

  
} // namespace DetDesc
//=============================================================================
