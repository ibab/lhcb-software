// $Id: TransporterFunctions.h,v 1.1 2009-07-27 16:24:05 jpalac Exp $
// ============================================================================
#ifndef TRANSPORTERFUNCTIONS_H 
#define TRANSPORTERFUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// GauduKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// ============================================================================
// LHcbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// Event
#include "Event/Particle.h"
/** @namespace DaVinci::Transporter Kernel/TransporterFunctions.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-09-22
 */
namespace DaVinci 
{
namespace Transporter
{
  // ==========================================================================
  /** Transport a composite Particle to specified z position, using linear
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
   *   dz/Pz * /  1  0  -Px/Pz  0  \
   *           |  0  1  -Py/Pz  0  |
   *           \  0  0    0     0  /
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
  StatusCode transportAndProjectComposite 
  ( const LHCb::Particle* particle, 
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
    Gaudi::XYZPoint refPoint(particle->referencePoint());
    refPoint += dZ*particle->slopes();

    // update reference point
    transParticle.setReferencePoint(refPoint);

    return StatusCode::SUCCESS;
 
  }
  // ==========================================================================
  /** Transport a composite Particle to specified z position, 
   *   using the linear extrapolation
   *  
   *  The transformation on the state vector, 
   *   \f$\vec{\mathbf{v}}_0\to\vec{\mathbf{v}}_1\f$, is
   *  \f[
   *   \begin{pmatrix}
   *      x_1 \\ y_1 \\ z_1 \\ p_{x1} \\ p_{y1} \\ p_{z1} \\ e_1 
   *   \end{pmatrix} = 
   *   \begin{pmatrix}
   *     x_0 + p_{x0}\frac{\delta z}{p_{z0}} \\   
   *     y_0 + p_{y0}\frac{\delta z}{p_{z0}} \\   
   *     z_0 +\delta z \\   
   *     p_{x0} \\
   *     p_{y0} \\
   *     p_{z0} \\
   *     e_{0} 
   *   \end{pmatrix} =
   *   \begin{pmatrix}
   *     x_0    \\   
   *     y_0    \\   
   *     z_0    \\   
   *     p_{x0} \\
   *     p_{y0} \\
   *     p_{z0} \\
   *     e_{0} 
   *   \end{pmatrix} + \delta z 
   *   \begin{pmatrix}
   *     \frac{p_x}{p_z} \\   
   *     \frac{p_x}{p_z} \\   
   *     1 \\   
   *     0 \\
   *     0 \\
   *     0 \\
   *     0 
   *   \end{pmatrix},  \f]
   *  where \f$\delta z = z_1 - z_0\f$.
   *
   *  The transport matrix \f$\mathbf{D}\f$ is 
   *  defined as \f$\mathbf{D}=\frac{\vec{\mathbf{v}}_1}{\vec{\mathbf{v}}_0}\f$:
   *  \f[
   *  \mathbf{D} = \frac{\vec{\mathbf{v}}_1}{\vec{\mathbf{v}}_0} = 
   *  \begin{pmatrix}
   *   1  & 0 & 0  & \frac{\delta z}{p_z} & 0                    & -\frac{\delta z p_x}{p^2_z} & 0 \\
   *   0  & 1 & 0  & 0                    & \frac{\delta z}{p_z} & -\frac{\delta z p_y}{p^2_z} & 0 \\
   *   0  & 0 & 1  & 0 & 0 & 0 & 0 \\
   *   0  & 0 & 0  & 1 & 0 & 0 & 0 \\ 
   *   0  & 0 & 0  & 0 & 1 & 0 & 0 \\ 
   *   0  & 0 & 0  & 0 & 0 & 1 & 0 \\ 
   *   0  & 0 & 0  & 0 & 0 & 0 & 1 
   *  \end{pmatrix} =
   *  \mathbf{1} + \frac{\delta z}{p_z} 
   *  \begin{pmatrix} 
   *   0  & 0 & 0  & 1 & 0 & -\frac{p_x}{p_z} & 0 \\
   *   0  & 0 & 0  & 0 & 1 & -\frac{p_y}{p_z} & 0 \\
   *   0  & 0 & 0  & 0 & 0 & 0                & 0 \\
   *   0  & 0 & 0  & 0 & 0 & 0                & 0 \\ 
   *   0  & 0 & 0  & 0 & 0 & 0                & 0 \\ 
   *   0  & 0 & 0  & 0 & 0 & 0                & 0 \\ 
   *   0  & 0 & 0  & 0 & 0 & 0                & 0 
   *  \end{pmatrix}   =  
   *  \begin{pmatrix} 
   *     \frac{\partial\vec{\mathbf{x}}_1}{\partial\vec{\mathbf{x}}_0} & 
   *     \frac{\partial\vec{\mathbf{x}}_1}{\partial         p^{\mu}_0} \\ 
   *     \frac{\partial         p^{\mu}_1}{\partial\vec{\mathbf{x}}_0} & 
   *     \frac{\partial         p^{\mu}_1}{\partial         p^{\mu}_0} 
   *  \end{pmatrix} = 
   *  \begin{pmatrix}
   *      \mathbf{1}_{3\times3}  & \mathbf{F}_{3\times4} \\ 
   *      \mathbf{0}_{4\times3}  & \mathbf{1}_{4\times4} 
   *  \end{pmatrix},
   *  \f]
   *  where the 3x4-matrix \f$\mathbf{F}\f$ is defined as 
   *  \f$    
   *  \mathbf{F} = \frac{\partial\vec{\mathbf{x}}_1}{\partial p^{\mu}_0} = 
   *  \frac{\delta z}{p_z}
   *  \begin{pmatrix}
   *    1  & 0 & -\frac{p_x}{p_z} & 0 \\
   *    0  & 1 & -\frac{p_x}{p_z} & 0 \\
   *    0  & 0 & 0                & 0 
   *  \end{pmatrix}. \f$
   *
   * The transformation on the particle's 7x7 covariance matrix 
   * \f$\mathbf{C}_0\f$ is given by
   * \f$\mathbf{C}_1 = \mathbf{D} \mathbf{C}_0 \mathbf{D}^T \f$,
   *  or, in block form,
   *  \f[ 
   *    \mathbf{C}_1 = 
   *     \begin{pmatrix}
   *      \mathbf{C}_{x1}  & \mathbf{C}^T_{px1} \\ 
   *      \mathbf{C}_{px1} & \mathbf{C}_{p1}    
   *     \end{pmatrix} =
   *  \begin{pmatrix}
   *      \mathbf{1}  & \mathbf{F} \\ 
   *      \mathbf{0}  & \mathbf{1}  
   *  \end{pmatrix} 
   *     \begin{pmatrix}
   *      \mathbf{C}_{x0}  & \mathbf{C}^T_{px0} \\ 
   *      \mathbf{C}_{px0} & \mathbf{C}_{p0}    
   *     \end{pmatrix} 
   *  \begin{pmatrix}
   *      \mathbf{1}   & \mathbf{0} \\ 
   *      \mathbf{F}^T & \mathbf{1} 
   *  \end{pmatrix} \f]
   *  This results in the expressions:
   *   \f[    
   *    \mathbf{C}_1 = 
   *     \begin{pmatrix}
   *      \mathbf{C}_{x1}  & \mathbf{C}^T_{px1} \\ 
   *      \mathbf{C}_{px1} & \mathbf{C}_{p1}    
   *     \end{pmatrix} =
   *    \begin{pmatrix}
   *       \mathbf{C}_{x0} + \mathbf{F}\mathbf{C}_{p0}\mathbf{F}^T + 
   *        \left( \mathbf{F}\mathbf{C}_{px0} + \mathbf{C}^{T}_{px0}\mathbf{F}^{T}\right) & 
   *       \mathbf{C}^T+\mathbf{F}\mathbf{C}_{p0} \\ 
   *       \mathbf{C}_{px0} + \mathbf{C}_{p0}\mathbf{F}^T &  \mathbf{C}_{p0} 
   *    \end{pmatrix}   \f]
   *
   *  @author Juan  PALACIOS Juan.Palacios@nikhef.nl
   *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
   *  @date   2008-03-10
   */
  StatusCode transportComposite 
  ( const LHCb::Particle* particle     , 
    const double          zNew         ,
    LHCb::Particle&       transported  ) 
  {
    // check the argument:
    if ( 0 == particle ) { return StatusCode::FAILURE ; } // RETURN 
    
    const double zOld    = particle -> referencePoint().Z() ;
    // no need to transport:
    if ( zNew == zOld ) /// @toto add a real comparison of float values  
    {
      if ( &transported != particle ) { transported = *particle ; }
      return StatusCode::SUCCESS ;
    }
    
    const double deltaZ  = zNew - zOld ;


    const Gaudi::LorentzVector& v = particle->momentum () ;
    
    // the only non-trivial component of the Jacobian
    Gaudi::Matrix3x4 F ; 
    const double coeff = deltaZ   / v.Pz () ; // delta(z)/p_z 
    
    F ( 0 , 0 ) =  coeff                   ;  // d(x_1)/d(p_x0) 
    F ( 0 , 2 ) = -coeff * v.Px() / v.Pz() ;  // d(x_1)/d(p_z0)
    F ( 1 , 1 ) =  coeff                   ;  // d(y_1)/d(p_y0) 
    F ( 1 , 2 ) = -coeff * v.Py() / v.Pz() ;  // d(y_1)/d(p_z0)
    
    // Linear transformation:
    
    // redefine the quantities:
    transported.setMomentum       
      ( particle -> momentum()                                     ) ;
    transported.setReferencePoint 
      ( particle -> referencePoint() + deltaZ * particle -> slopes () ) ;
    
    // redefine the covarince matrices:
    
    // get C_0 matrix: 
    
    const Gaudi::SymMatrix4x4& C_p0  = particle -> momCovMatrix    () ;
    const Gaudi::Matrix4x3&    C_px0 = particle -> posMomCovMatrix () ;
    const Gaudi::SymMatrix3x3& C_x0  = particle -> posCovMatrix    () ;
    
    // we are modifying the same particle 
    if ( &transported == particle )  
    {
      
      //  C_x1              = C_x0 + ...      
      Gaudi::SymMatrix3x3& C_x1 = const_cast<Gaudi::SymMatrix3x3&> ( C_x0 ) ;
      //                    + F*C_p0*F^T + ...
      C_x1 += ROOT::Math::Similarity ( F , C_p0 ) ;
      //                    +  F*C_{px0} + C_{px0}^T*F^T 
      Gaudi::Math::update ( C_x1 , F * C_px0 ) ;
      
      // C_px1 = C_px0 + C_{p0}*F^T
      Gaudi::Matrix4x3& C_px1 = const_cast<Gaudi::Matrix4x3&>( C_px0 ) ;
      C_px1 += C_p0 * ROOT::Math::Transpose ( F ) ;
      
      // no need to change C_p1 
      
    }
    else  // we are dealing with different partcicles:
    {      
      //  C_x1              = C_x0 + ...      
      Gaudi::SymMatrix3x3 C_x1 = C_x0 ;
      //                    + F*C_p0*F^T + ...
      C_x1 += ROOT::Math::Similarity ( F , C_p0 ) ;
      //                    +  F*C_{px0} + C_{px0}^T*F^T 
      Gaudi::Math::update ( C_x1 , F * C_px0 ) ;
      
      transported.setPosCovMatrix    ( C_x1 ) ;
      
      // C_p1 = C_p0
      transported.setMomCovMatrix    ( C_p0 ) ;
      
      // C_px1 = C_px0 + C_{p0}*F^T
      transported.setPosMomCovMatrix ( C_px0 + C_p0 * ROOT::Math::Transpose ( F ) ) ;
    }
    // 
    return StatusCode::SUCCESS ;                                    // RETURN 
  }
  // ==========================================================================
  StatusCode transportAndProjectNeutralBasic(const LHCb::Particle* particle, 
                                             const double zNew,
                                             LHCb::Particle& transParticle) 
  {
    return transportAndProjectComposite(particle, zNew, transParticle);
  }
  // ==========================================================================
  StatusCode transportNeutralBasic(const LHCb::Particle* particle, 
                                   const double zNew,
                                   LHCb::Particle& transParticle) 
  {
    return transportComposite(particle, zNew, transParticle);
  }
  // ==========================================================================
} // end of namespace DaVinci
// ============================================================================
} // end of namespace Transporter
// ============================================================================
// The END 
// ============================================================================
#endif // TRANSPORTERFUNCTIONS_H
// ============================================================================
