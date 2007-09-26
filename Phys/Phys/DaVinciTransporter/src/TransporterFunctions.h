// $Id: TransporterFunctions.h,v 1.7 2007-09-26 13:48:27 jpalac Exp $
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
   *  x_1  = x_0 + dZ*pX_0/pZ_0
   *  y_1  = y_0 + dZ*pY_0/pZ_0
   *  z_1  = z_0 + dZ
   *  pX_1 = pX_0
   *  pY_1 = pY_0
   *  pZ_1 = pZ_0
   *  E_1  = E_old
   *  @endcode
   *
   *  The transport matrix D is [dv_1/dv_0], or
   *  @code  
   *   1  0  0 dz/Pz   0    -dz*Px/Pz/Pz  0
   *   0  1  0   0   dz/Pz  -dz*Py/Pz/Pz  0
   *   0  0  1   0     0          0       0
   *   0  0  0   1     0          0       0
   *   0  0  0   0     1          0       0
   *   0  0  0   0     0          1       0
   *   0  0  0   0     0          0       1
   *  @endcode
   *
   *  Or, in block structure,
   *  @code
   *   I(3x3)   Dxp(3x4)
   *   0(4x3)    I(4x4)
   *  @endcode
   *
   *  Where Dxp is
   *   
   *   @code      
   *         /  1    0 -Px/Pz  0  \
   *  1/Pz * |  0    1 -Py/Pz  0  |
   *         \  0    0    0    0  /
   *  @endcode
   * The transformation on the Particle's 7x7 covariance matrix C_0 is given by
   *
   *   C_1 = D*C_0*D^T
   * 
   * or, in block form,
   *  @code
   *  /  I  Dxp  \   /  Cx_0  Cpx_0^T  \   /  I      0  \    
   *  |          | * |                 | * |            |
   *  \  0   I   /   \  Cpx_0   Cp_0   /   \  Dxp^T  I  /
   *  @endcode
   * 
   *  resulting in the expressions
   *  @code
   *  Cx_1  = Cx_0 + Cpx_0^T*Dxp^T + Dxp*Cpx_0 + Dxp*Cp_0*Dxp^T
   *  Cpx_1 = Cpx_0 + Cp*Dxp^T
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
    const double Pz = particle->momentum().Z();
    const double coef = 1./Pz;

    // Transport matrix position-momentum block
    Matrix3x4 Dxp;
    Dxp(0,0) = coef;
    Dxp(0,2) = -1*coef*particle->momentum().X()/Pz;
    Dxp(1,1) = coef;
    Dxp(1,2) = -1*coef*particle->momentum().Y()/Pz;

    const Matrix4x3 DxpT = ROOT::Math::Transpose(Dxp);

    // Old covariance matrix blocks
    const SymMatrix3x3  Cx_0  = particle->posCovMatrix();
    const Matrix4x3     Cpx_0 = particle->posMomCovMatrix();
    const SymMatrix4x4  Cp_0  = particle->momCovMatrix();
    const Matrix3x4     Cxp_0 = ROOT::Math::Transpose(Cpx_0);

    // New covariance matrix blocks
    const Matrix3x3 Cx_1  = 
      Cx_0 + Cxp_0*DxpT +
      Matrix3x3(Dxp*Cpx_0) + 
      Matrix3x3( Dxp*Cp_0*DxpT );

    const Matrix4x3 Cpx_1 = Cpx_0 + Cp_0*DxpT;

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
