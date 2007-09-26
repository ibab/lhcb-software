// $Id: TransporterFunctions.h,v 1.8 2007-09-26 16:25:06 jpalac Exp $
#ifndef TRANSPORTERFUNCTIONS_H 
#define TRANSPORTERFUNCTIONS_H 1

// Include files
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

/** @namespace DaVinciTransporter TransporterFunctions.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-09-22
 */

namespace DaVinciTransporter {

  /**
   *  Transport a composite Particle to specified z position, using linear
   *  extrapolation.
   *  The transformation on the state vector, v_0 -> v_1, is
   *  @code
   *  x_1  = x_0 + (z_1-z_0)*pX_0/pZ_0
   *  y_1  = y_0 + (z_1-z_0)*pY_0/pZ_0
   *  z_1  = z_1
   *  pX_1 = pX_0
   *  pY_1 = pY_0
   *  pZ_1 = pZ_0
   *  E_1  = E_0
   *  @endcode
   *
   *  The transport matrix D is [dv_1/dv_0]. Here dz = ( z_1-z_0 )
   *  @code  
   *   1  0 -Px/Pz dz/Pz   0    -dz*Px/Pz/Pz  0
   *   0  1 -Py/Pz   0   dz/Pz  -dz*Py/Pz/Pz  0
   *   0  0    0     0     0          0       0
   *   0  0    0     1     0          0       0
   *   0  0    0     0     1          0       0
   *   0  0    0     0     0          1       0
   *   0  0    0     0     0          0       1
   *  @endcode
   *
   *  Or, in block structure,
   *  @code
   *   A(3x3)  Dxp(3x4)  
   *   0(4x3)    I(4x4)
   *  @endcode
   *
   *  Where Dxp is
   *   
   *   @code      
   *
   *   dz/Pz * /  1  0  -dz*Px/Pz  0  \
   *           |  0  1  -dz*Py/Pz  0  |
   *           \  0  0       0     0  /
   *
   *  @endcode
   *  A is
   *  @code
   *  / 1 0 -Px/Pz \
   *  | 0 1 -Py/Pz |
   *  \ 0 0    0   /
   *  @endcode
   * The transformation on the Particle's 7x7 covariance matrix C_0 is given by
   *
   *   C_1 = D*C_0*D^T
   * 
   * or, in block form,
   *  @code
   *  /  A  Dxp  \   /  Cx_0  Cpx_0^T  \   /  A^T    0  \    
   *  |          | * |                 | * |            |
   *  \  0   I   /   \  Cpx_0   Cp_0   /   \  Dxp^T  I  /
   *  @endcode
   * 
   *  This results in the expressions
   *  @code
   *  Cx_1  = A*Cx_0*A^T + A*Cpx_0^T*Dxp^T + Dxp*Cpx_0*A^T + Dxp*Cp_0*Dxp^T
   *  Cpx_1 = Cpx_0*A^T + Cp_0*Dxp^T
   *  Cp_1  = Cp_0 
   *  @endcode
   *
   *  @author Juan PALACIOS
   *  @date   2006-09-22
   *
   *
   */
  StatusCode transportComposite(const LHCb::Particle* particle, 
                                const double zNew,
                                LHCb::Particle& transParticle) 
  {
    using namespace Gaudi;
    
    const double dZ = zNew-particle->referencePoint().Z();
    const double Px = particle->momentum().X();
    const double Py = particle->momentum().Y();
    const double Pz = particle->momentum().Z();
    const double coef = dZ/Pz;

    // Transport matrix position-momentum block
    Matrix3x4 Dxp;
    Dxp(0,0) = coef;
    Dxp(0,2) = -1*coef*Px/Pz;
    Dxp(1,1) = coef;
    Dxp(1,2) = -1*coef*Py/Pz;

    const Matrix4x3 DxpT = ROOT::Math::Transpose(Dxp);

    // Transport matrix position block
    Matrix3x3 A;
    A(0,0) = 1.;
    A(1,1) = 1.;
    A(0,2) = -1.*Px/Pz;
    A(1,2) = -1.*Py/Pz;
    
    const Matrix3x3 AT = ROOT::Math::Transpose(A);

    // Old covariance matrix blocks
    const SymMatrix3x3  Cx_0  = particle->posCovMatrix();
    const Matrix4x3     Cpx_0 = particle->posMomCovMatrix();
    const SymMatrix4x4  Cp_0  = particle->momCovMatrix();
    const Matrix3x4     Cxp_0 = ROOT::Math::Transpose(Cpx_0);

    // New covariance matrix blocks
    const Matrix3x3 Cx_1  = 
      A*Cx_0*AT + Matrix3x3(A*Cxp_0*DxpT) +
      Matrix3x3(Dxp*Cpx_0*AT) + 
      Matrix3x3( Dxp*Cp_0*DxpT );

    const Matrix4x3 Cpx_1 = Cpx_0*AT + Matrix4x3(Cp_0*DxpT);

    // Update covariance matrices in particle
    transParticle.setPosCovMatrix( Cx_1.LowerBlock() );
    transParticle.setPosMomCovMatrix ( Cpx_1 );

    // calculate reference point
    XYZPoint refPoint(particle->referencePoint());
    refPoint += dZ*particle->slopes();

    // update reference point
    transParticle.setReferencePoint(refPoint);
   
    return StatusCode::SUCCESS;
 
  }

  StatusCode transportNeutralBasic(const LHCb::Particle* particle, 
                                   const double zNew,
                                   LHCb::Particle& transParticle) 
  {
    return transportComposite(particle, zNew, transParticle);
  }
  
}
#endif // TRANSPORTERFUNCTIONS_H
