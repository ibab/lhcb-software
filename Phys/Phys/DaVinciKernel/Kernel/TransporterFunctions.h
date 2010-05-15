// $Id: TransporterFunctions.h,v 1.2 2010-05-15 14:46:17 ibelyaev Exp $
// ============================================================================
#ifndef TRANSPORTERFUNCTIONS_H 
#define TRANSPORTERFUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// GauduKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
/** @namespace DaVinci::Transporter Kernel/TransporterFunctions.h
 *
 *  @author Juan PALACIOS
 *  @date   2006-09-22
 */
namespace DaVinci 
{
  // ==========================================================================
  namespace Transporter
  {
    // ========================================================================
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
     *   dz/Pz * /  1  0  -Px/Pz  0             \
     *           |  0  1  -Py/Pz  0  |
     *           \  0  0    0     0  /
     *
     *  @endcode
     *  A is
     *  @code
     *  / 1 0 -Px/Pz                            \
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
    ( const LHCb::Particle* particle      , 
      const double          zNew          ,
      LHCb::Particle&       transParticle ) ;
    // ========================================================================
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
     *     p_{x0} \                             \
     *     p_{y0} \                             \
     *     p_{z0} \                             \
     *     e_{0} 
     *   \end{pmatrix} =
     *   \begin{pmatrix}
     *     x_0    \\   
     *     y_0    \\   
     *     z_0    \\   
     *     p_{x0} \                             \
     *     p_{y0} \                             \
     *     p_{z0} \                             \
     *     e_{0} 
     *   \end{pmatrix} + \delta z 
     *   \begin{pmatrix}
     *     \frac{p_x}{p_z} \\   
     *     \frac{p_x}{p_z} \\   
     *     1 \\   
     *     0 \                                  \
     *     0 \                                  \
     *     0 \                                  \
     *     0 
     *   \end{pmatrix},  \f]
     *  where \f$\delta z = z_1 - z_0\f$.
     *
     *  The transport matrix \f$\mathbf{D}\f$ is 
     *  defined as \f$\mathbf{D}=\frac{\vec{\mathbf{v}}_1}{\vec{\mathbf{v}}_0}\f$:
     *  \f[
     *  \mathbf{D} = \frac{\vec{\mathbf{v}}_1}{\vec{\mathbf{v}}_0} = 
     *  \begin{pmatrix}
     *   1  & 0 & 0  & \frac{\delta z}{p_z} & 0                    & -\frac{\delta z p_x}{p^2_z} & 0 \ \
     *   0  & 1 & 0  & 0                    & \frac{\delta z}{p_z} & -\frac{\delta z p_y}{p^2_z} & 0 \ \
     *   0  & 0 & 1  & 0 & 0 & 0 & 0 \                                  \
     *   0  & 0 & 0  & 1 & 0 & 0 & 0 \\ 
     *   0  & 0 & 0  & 0 & 1 & 0 & 0 \\ 
     *   0  & 0 & 0  & 0 & 0 & 1 & 0 \\ 
     *   0  & 0 & 0  & 0 & 0 & 0 & 1 
     *  \end{pmatrix} =
     *  \mathbf{1} + \frac{\delta z}{p_z} 
     *  \begin{pmatrix} 
     *   0  & 0 & 0  & 1 & 0 & -\frac{p_x}{p_z} & 0 \ \
     *   0  & 0 & 0  & 0 & 1 & -\frac{p_y}{p_z} & 0 \ \
     *   0  & 0 & 0  & 0 & 0 & 0                & 0 \ \
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
     *    1  & 0 & -\frac{p_x}{p_z} & 0 \       \
     *    0  & 1 & -\frac{p_x}{p_z} & 0 \       \
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
      LHCb::Particle&       transported  ) ;
    // ========================================================================
    inline StatusCode transportAndProjectNeutralBasic
    ( const LHCb::Particle* particle       , 
      const double          zNew           ,
      LHCb::Particle&       transParticle  ) 
    {
      return transportAndProjectComposite ( particle, zNew, transParticle ) ;
    }
    // ========================================================================
    inline StatusCode transportNeutralBasic
    ( const LHCb::Particle* particle      , 
      const double          zNew          ,
      LHCb::Particle&       transParticle ) 
    {
      return transportComposite ( particle, zNew, transParticle ) ;
    }
    // ========================================================================
  } //                                                 end of namespace DaVinci
  // ==========================================================================
} //                                               end of namespace Transporter
// ============================================================================
// The END 
// ============================================================================
#endif // TRANSPORTERFUNCTIONS_H
// ============================================================================
