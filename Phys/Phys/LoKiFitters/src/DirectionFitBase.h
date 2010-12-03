// $Id$
// ============================================================================
#ifndef LOKIFITTERS_DIRECTIONFITBASE_H 
#define LOKIFITTERS_DIRECTIONFITBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h" 
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleTransporter.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/VertexBase.h"
// ============================================================================
// local
// ============================================================================
#include "FitterUtils.h"
// ============================================================================
/** @file 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2008-02-17
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace LoKi
{
  // =========================================================================
  /** @class DirectionFitBase DirectionFitBase.h
   *
   *  The useful base class for implementationof two importantdf fitters
   *    - Lifetime fitter 
   *    - Direction fitter 
   * 
   *  The implementation follows the note by Paul AVERY 
   *    "Directly Determining Lifetime Using a 3-D Fit"
   *
   *  Lets \f$\vec{\alpha}=\left( p^{\mu} , \vec{x}_d , \vec{x}_p \right)\f$
   *  be a 10-vector of particle & primary vertex parameters:
   *   - \f$  p^{\mu}   = \left( p_x , p_y , p_z , E \right) \f$ 
   *   - \f$ \vec{x}_d = \left( x_d , y_d , z_d \right) \f$ 
   *   - \f$ \vec{x}_p = \left( x_p , y_p , z_p \right) \f$ 
   *
   *  The corresponding \f$10\times10\f$-covarinace matrix could 
   *  be decomposed as:
   *  \f[ \mathbf{V_{\alpha}} =
   *       \begin{pmatrix}
   *         \mathbf{V}_{p_{\mu}} & \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}} & 0 \\
   *         \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}}^T &  \mathbf{V}_{\vec{x}_d} & 0  \\
   *         0 & 0 &   \mathbf{V}_{\vec{x}_{pv}}
   *       \end{pmatrix}. \f]
   *  
   *  The direction constraint equations are:
   *   \f[ \mathbf{H} = \begin{pmatrix}
   *         x_p - x_d + \frac{p_x}{m}c\tau \\
   *         y_p - y_d + \frac{p_y}{m}c\tau \\
   *         z_p - z_d + \frac{p_z}{m}c\tau 
   *        \end{pmatrix} = 0. \f]
   * These constraints could be represented as:
   * \f$ \mathbf{H}\left( \mathbf{\alpha} , c\tau \right) = 
   * \mathbf{D} \left( \mathbf{\alpha} - \mathbf{\alpha}_0 \right) + 
   * \mathbf{E} \left( c\tau - c\tau_0                     \right)  +\mathbf{d}
   * = 
   * \mathbf{D} \delta \mathbf{\alpha} + 
   * \mathbf{E} \delta c\tau  +\mathbf{d}, \f$
   * where
   *  - \f$ \mathbf{E} = \left. \frac{\partial \mathbf{H}}{\partial c\tau} \right|_{\alpha=\alpha_o,c\tau=c\tau_0} 
   *   = \begin{pmatrix}
   *             p_x/m \\
   *             p_y/m \\
   *             p_z/m 
   *      \end{pmatrix} \f$ 
   *  - \f$ \mathbf{D} = \left. \frac{\partial \mathbf{H}}{\partial \alpha} \right|_{\alpha=\alpha_o, c\tau=c\tau_0} = 
   * \begin{pmatrix} 
   *  \frac{\partial \mathbf{H}}{\partial p_{\mu} } & 
   *  \frac{\partial \mathbf{H}}{\partial \vec{x}_d} & 
   * \frac{\partial \mathbf{H}}{\partial \vec{x}_{pv}} 
   *  \end{pmatrix}  = 
   *  \begin{pmatrix} \mathbf{W} & -1 & 1 
   * \end{pmatrix}. \f$ 
   *
   The matrix \f$\mathbf{W}\f$ is easy to calculate
   * \f[ \mathbf{W} = \left. 
   *   \frac{\partial \mathbf{H}}{\partial p_{\mu}} \right|_{\alpha=\alpha_o,c\tau=c\tau_0} = 
   * c\tau_0 \frac{ \partial \vec{p}/m }{\partial p_{\mu}} = \frac{c\tau_0}{m}
   * \begin{pmatrix} 
   * 1+\frac{p_x^2}{m^2} & \frac{p_xp_y}{m^2}    &  \frac{p_xp_z}{m^2} & - \frac{p_xE}{m^2} \\
   *  \frac{p_yp_x}{m^2} & 1 + \frac{p_y^2}{m^2} &  \frac{p_yp_z}{m^2} & - \frac{p_yE}{m^2} \\
   *  \frac{p_zp_x}{m^2} & \frac{p_zp_y}{m^2}    & 1 + \frac{p_z^2}{m^2} & - \frac{p_zE}{m^2} 
   * \end{pmatrix} \f] 
   *
   * The auxillary \f$3\times3\f$-matrix 
   *  \f$\mathbf{V_D}\f$ (the covariance matrix of constraints) is equal to:
   * \f[
   * \mathbf{V_D} = \left( \mathbf{D}\mathbf{V_{\alpha}}\mathbf{D}^T\right)^{-1} 
   * = \left( \mathbf{W}\mathbf{V}_{p_{\mu}}\mathbf{W}^T 
   * - \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}}^T\mathbf{W}^T 
   * -  \mathbf{W}\mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}}
   * +  \mathbf{V}_{\vec{x}_d}
   * +  \mathbf{V}_{\vec{x}_{pv}}\right)^{-1}
   * \f]
   *
   *  - \f$ \mathbf{\lambda}_0 = 
   *    \mathbf{V_D} \left( \mathbf{D}\delta\mathbf{\alpha} +\mathbf{d} \right)  \f$ 
   *  - \f$ V_{c\tau} = \left( \mathbf{E}^T \mathbf{V_D} \mathbf{E} \right)^{-1} \f$ 
   *  - \f$ \delta c\tau = V_{c\tau} \mathbf{E}^T \mathbf{\lambda}_0 \f$ 
   *  - \f$ \lambda = \lambda_0 + \mathbf{V}_D\mathbf{E}\delta c\tau \f$ 
   *  - \f$ \vec{\alpha} = \vec{\alpha}_0 - \mathbf{V}_{\vec{\alpha}0}\mathbf{D}^T\lambda \f$ 
   *
   * The covariance matrices are:
   *
   * - \f$ \mathbf{D}\mathbf{V_{\alpha}} = 
   * \left( \mathbf{D}_1^T , \mathbf{D}_2^T , \mathbf{D_3} \right) = 
   * \left( 
   * \mathbf{W}\mathbf{V_{p_{\mu}}} - \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}}^T , 
   * \mathbf{W} \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}} -  \mathbf{V}_{\vec{x}_d} ,
   * \mathbf{V}_{\vec{x}_{pv}}
   * \right) \f$ 
   * - \f$ \mathbf{V_{\alpha}^\prime} = 
   * \mathbf{V_{\alpha}} - 
   * \mathbf{V_{\alpha}}\mathbf{D}^T \mathbf{V_D} \mathbf{D}\mathbf{V_{\alpha}}  + 
   * \mathbf{V_{\alpha}}\mathbf{D}^T 
   * \mathbf{V_D} 
   * \mathbf{E} 
   * \mathbf{V_{c\tau}} 
   * \mathbf{E}^T 
   * \mathbf{V_D} 
   * \mathbf{D}\mathbf{V_{\alpha}} \f$
   * - \f$ \mathbf{V_{p_{\mu}}^\prime} = 
   *       \mathbf{V_{p_{\mu}}} - 
   *       \mathbf{D}_1
   *       \mathbf{V_{D}} 
   *       \mathbf{D}_1^T +
   *       \mathbf{D}_1
   *       \left( \mathbf{V_D} 
   *              \mathbf{E} 
   *              \mathbf{V_{c\tau}} 
   *              \mathbf{E}^T 
   *              \mathbf{V_D} \right) \mathbf{D}_1^T \f$ 
   * - \f$ \mathbf{V^\prime}_{ \left\{ p_{\mu},\vec{x}_d\right\}} =  
   *       \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}} - 
   *       \mathbf{D}_2
   *       \mathbf{V_D}
   *       \mathbf{D}_1^T +
   *       \mathbf{D}_2^T
   *       \left( \mathbf{V_D} 
   *              \mathbf{E} 
   *              \mathbf{V_{c\tau}} 
   *              \mathbf{E}^T 
   *              \mathbf{V_D} \right) \mathbf{D}_1^T \f$ 
   * - \f$ \mathbf{V^\prime}_{\vec{x}_d} = 
   *       \mathbf{V}_{\vec{x}_d} - 
   *       \mathbf{D}_2
   *       \mathbf{V_D}
   *       \mathbf{D}_2^T +
   *       \mathbf{D}_2
   *       \left( \mathbf{V_D} 
   *              \mathbf{E} 
   *              \mathbf{V_{c\tau}} 
   *              \mathbf{E}^T 
   *              \mathbf{V_D} \right) \mathbf{D}_2^T \f$
   * - \f$ \mathbf{V^\prime}_{\vec{x}_{pv}} = 
   *       \mathbf{V}_{\vec{x}_{pv}}  - 
   *       \mathbf{D}_3
   *       \mathbf{V_D}
   *       \mathbf{D}_3^T+
   *       \mathbf{D}_3
   *       \left( \mathbf{V_D} 
   *              \mathbf{E} 
   *              \mathbf{V_{c\tau}} 
   *              \mathbf{E}^T 
   *             \mathbf{V_D} \right) \mathbf{D}_3^T \f$ 
   *
   *  The clear symmetries of these expressions allows to perforem really efficient
   *  calculation. Please note that one requires only the inversion of 
   *  one symmetrix \f$3\times3\f$-matrix. 
   *
   *  Also note that taking \f$c\tau_0=0\f$ and \f$\mathbf{W}=0\f$ one is able to 
   *  find the first and very efficient approximation to \f$c\tau\f$, 
   *  neglecting the uncertanty in the particle momenta. 
   *  Using this approximation and the starting point fo riteration 
   *  and applying the full matrix \f$\mathbf{W}\f$ one achieves
   *  very rapid convergency.
   *
   *  @see IDirectionFit
   *  @see ILifetimeFitter
   *  @see IParticleReFitter
   *  @see LoKi::LifetimeFitter
   *  @see LoKi::DirectionFitter
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2008-02-17
   */
  class DirectionFitBase : public GaudiTool
  {
  public:
    // ========================================================================
    /// auxillary enum which define the error codes 
    enum {
      /// no valid end-vertex is found for the particle 
      NoEndVertex            = 500  , // no valid end-vertex is found for the particle 4
      /// no IPhysDesktop is available  (for IParticleReFitter interface)
      NoDesktop              = 501  , // no IPhysDesktop is available  
      /// no valid primary vertex is found 
      NoPrimaryVertex        = 502  , // no valid primary vertex is found
      /// matrix inversion failure 
      MatrixInversionFailure = 504  , // matrix inversion failure
      /// no convergency 
      NoConvergency          = 503     // no convergency 
    } ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization of the tool 
    virtual StatusCode initialize() 
    {
      StatusCode sc = GaudiTool::initialize() ;
      if ( sc.isFailure() ) { return sc ; }
      svc<IService>( "LoKiSvc" , true ) ;
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /** constructor 
     *  @param Type the actual tool type (??)
     *  @param Name the tool instance name 
     *  @param parent the parent of the tool
     */
    DirectionFitBase
    ( const std::string& Type   , 
      const std::string& Name   , 
      const IInterface*  parent ) ;
    // virtual & protected destructor 
    virtual ~DirectionFitBase () ; ///< virtual & protected destructor 
    // ========================================================================
  protected:
    // ========================================================================
    /** get the fast evaluation of ctau, neglecting the 
     *  uncertainties in momentum, and taking into account only
     *  the uincertanties in primary and secondary vertices.
     *
     *  Essentially it corresponds to the first iteration with 
     *  \f$ \mathbf{W}=\frac{\partial}{\partial p^{\mu}}\mathbf{H} = 0  \f$ 
     *   and \f$ c\tau_0 = 0 \f$. 
     *
     *  @param primary  (input) the primary vertex 
     *  @param particle (input) the particle
     *  @param decay    (input) the decay vertex 
     *  @return fast evaluation of ctau 
     */
    double ctau0 
    ( const LHCb::VertexBase& primary  , 
      const LHCb::Particle&   particle , 
      const LHCb::VertexBase& decay    ) const ;
    // ========================================================================
    /** make few iteration steps 
     * 
     *  @attention input particle&vertices are modified!
     *
     *  @param primary    pointer to primary vertex (input) 
     *  @param particle   pointer to particle       (input) 
     *  @param decay      pointer to decay vertex   (input) 
     *  @param momentum   the expansion point for particle momentum (input/output) 
     *  @param decvertex  the expansion point for decay   vertex      (input/output) 
     *  @param primvertex the expansion point for primary vertex      (input/output) 
     *  @param ctau       the estimate of the proper time (c*tau) (input/output)
     *  @param error      the estimate of the error in the proper time (c*tau) (output)
     *  @param chi2       the estimate of the chi2 (output)
     *  @return status code 
     */
    StatusCode iterate 
    ( const LHCb::VertexBase* primary    , 
      const LHCb::Particle*   particle   , 
      const LHCb::VertexBase* decay      , 
      Gaudi::LorentzVector&   momentum   ,
      Gaudi::XYZPoint&        decvertex  , 
      Gaudi::XYZPoint&        primvertex ,
      double&                 ctau       , 
      double&                 error      , 
      double&                 chi2       ) const ;
    // ========================================================================
    /** make the real fit 
     *
     *  @attention the input particle&vertices are modified!
     *
     *  @param primary  pointer to (non-const!) primary vertex (input/output) 
     *  @param particle pointer to (non-const!) particle      (input/output) 
     *  @param decay    pointer to (non-const!) decay vertex  (input/output) 
     *  @param ctau     the estimate of the proper time (c*tau) (input/output)
     *  @param error    the estimate of the error in the proper time (c*tau) (output)
     *  @param chi2     the estimate of the chi2 (output)
     *  @return status code 
     */
    StatusCode fit_ 
    ( LHCb::VertexBase* primary   , 
      LHCb::Particle*   particle  , 
      LHCb::VertexBase* decay     , 
      double&           ctau      , 
      double&           error     , 
      double&           chi2      ) const ;
    // ========================================================================
    /** make the real fit 
     *
     *  @attention the input particle&vertices are *NOT* modified!
     *
     *  @param primary  pointer to primary vertex (input) 
     *  @param particle pointer to particle       (input) 
     *  @param decay    pointer to decay vertex   (input) 
     *  @param ctau     the estiamte of the proper time (c*tau) (input/output)
     *  @param error    the estiamte of the error in the proper time (c*tau) (output)
     *  @param chi2     the estiamte of the chi2 (output)
     *  @return status code 
     */
    StatusCode fitConst_ 
    ( const LHCb::VertexBase* primary   , 
      const LHCb::Particle*   particle  , 
      const LHCb::VertexBase* decay     , 
      double&                 ctau      , 
      double&                 error     , 
      double&                 chi2      ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    DirectionFitBase() ;                 // the default constructor is disabled
    /// the copy constructor is disabled 
    DirectionFitBase( const DirectionFitBase& ) ;        // no copy constructor
    /// assigmenent operator is disabled
    DirectionFitBase& operator= ( const DirectionFitBase& ) ; // no assignement
    // ========================================================================
  private:
    // ========================================================================
    /// get the tranporter tool:
    IParticleTransporter* transporter() const 
    {
      if ( 0 == m_transporter ) 
      { m_transporter = tool<IParticleTransporter> ( m_transporterName ) ; }
      return m_transporter ;
    }
    // ========================================================================
    /** transport the particle into new position 
     *  @attention it call transport <b>AND</b> project
     *  @param p1 (INPUT)  particle to be tranported 
     *  @param x  (INPUT)  z-ccordiate of tranportation 
     *  @param p2 (OUTPUT) the tarnsported particle
     */
    inline StatusCode transport
    ( const LHCb::Particle* p1 , 
      const double          z  , 
      LHCb::Particle&       p2 ) const
    {
      if ( 0 == m_transporter ) 
      { m_transporter = tool<IParticleTransporter> ( m_transporterName ) ; }
      // return m_transporter -> transportAndProject ( p1 , z , p2 ) ;
      return m_transporter -> transport  ( p1 , z , p2 ) ;
    }
    // ========================================================================
    /** transport the particle into new position 
     *  @attention it call transport <b>AND</b> project
     *  @param p1 (INPUT)  particle to be tranported 
     *  @param x  (INPUT)  the point of tranportation 
     *  @param p2 (OUTPUT) the tarnsported particle
     */
    inline StatusCode transport 
    ( const LHCb::Particle*  p1 , 
      const Gaudi::XYZPoint& z  , 
      LHCb::Particle&        p2 ) const 
    { return transport ( p1 , z.Z() , p2 ) ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the maximal number of iterations
    unsigned int m_max_iter   ; // maximal number of iteration 
    /// the convergency parameter 
    double       m_delta_chi2 ; // convergency parameter 
    /// the convergency parameter 
    double       m_delta_ctau ; // convergency parameter 
    /// The name of particle transpoter tool 
    std::string  m_transporterName ; // The name of particle transpoter tool    
    /// The particle transporter itself 
    mutable IParticleTransporter* m_transporter ; // The transporter itself
    /// the actual fitter object
    mutable LoKi::Fitters::Fitter1 m_fitter ; // the actual fitter object
    /// the particle (needed for propagation) 
    mutable LHCb::Particle         m_particle ; // the particle for propagation
    /// helper matrix for evalaution of other covarinace matrices:
    mutable Gaudi::SymMatrix3x3    m_vd1 ; // helper matrix
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKIFITTERS_DIRECTIONFITBASE_H
// ============================================================================

