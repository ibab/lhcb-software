// $Id: DirectionFitBase.h,v 1.2 2008-02-24 19:48:19 ibelyaev Exp $
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
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/VertexBase.h"
// ============================================================================
namespace LoKi
{
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
    /// auxillary enum which define the error codes 
    enum 
      {
        // no valid end-vertex is found for the particle 
        NoEndVertex            = 500  , ///< no valid end-vertex is found for the particle 4
        // no IPhysDesktop is available  (for IParticleReFitter interface)
        NoDesktop              = 501  , ///< no IPhysDesktop is available  
        // no valid primary vertex is found 
        NoPrimaryVertex        = 502  , ///< no valid primary vertex is found
        // matrix inversion failure 
        MatrixInversionFailure = 504  , ///< matrix inversion failure
        // no convergency 
        NoConvergency          = 503     ///< no convergency 
      } ;
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
     *  @param primary the primary vertex 
     *  @param particle the particle
     *  @param decay  the decay vertex 
     *  @return fast evaluation of ctau 
     */
    double ctau0 
    ( const LHCb::VertexBase* primary  , 
      const LHCb::Particle*   particle , 
      const LHCb::VertexBase* decay    ) const ;
    // ========================================================================
    /** make one iteration step
     * 
     *  @attention input particle&vertices are modified!
     *
     *  @param primary pointer to (non-const!) primary vertex (input/output) 
     *  @param particle pointer to (non-const!) particle      (input/output) 
     *  @param decay    pointer to (non-const!) decay vertex  (input/output) 
     *  @param ctau     the estiamte of the proper time (c*tau) (input/output)
     *  @param error    the estiamte of the error in the proper time (c*tau) (output)
     *  @param chi2     the estiamte of the chi2 (output)
     *  @return status code 
     */
    StatusCode step 
    ( LHCb::VertexBase* primary   , 
      LHCb::Particle*   particle  , 
      LHCb::VertexBase* decay     , 
      double&           ctau      , 
      double&           error     , 
      double&           chi2      ) const ;
    // ========================================================================
    /** make the real fit 
     *
     *  @attention input particle&vertices are modified!
     *
     *  @param primary pointer to (non-const!) primary vertex (input/output) 
     *  @param particle pointer to (non-const!) particle      (input/output) 
     *  @param decay    pointer to (non-const!) decay vertex  (input/output) 
     *  @param ctau     the estiamte of the proper time (c*tau) (input/output)
     *  @param error    the estiamte of the error in the proper time (c*tau) (output)
     *  @param chi2     the estiamte of the chi2 (output)
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
    /** calculate W-matrix: 
     *  \f$ \mathbf{W} = \frac{\partial}{\partial p^{\mu}}   \mathbf{H} \f$ 
     *
     *  It is the only non-trivial component of 
     *  the general matrix \f$\mathbf{D}\f$:
     *    \f$ \mathbf{D} = 
     *     \frac{\partial}
     *          {\partial\vec{\alpha}}\mathbf{H} = 
     *      \left(  
     *       \frac{\partial}{\partial p^{\mu}}   \mathbf{H},
     *       \frac{\partial}{\partial \vec{x}_d} \mathbf{H},
     *       \frac{\partial}{\partial \vec{x}_p} \mathbf{H} \right) = 
     *       \left( \mathbf{W} , -1 , 1 \right) \f$
     *
     *  @param v    the lorents-vector      (input) 
     *  @param ctau the evaluation of c*tau (input) 
     *  @param w    the w-matrix            (output) 
     */
    void wmatrix 
    ( const Gaudi::LorentzVector& v    , 
      const double                ctau , 
      Gaudi::Matrix3x4&           w    ) const ;
    // ========================================================================
  private:
    // ========================================================================
    // the default constructor is disabled
    DirectionFitBase() ; ///< the default constructor is disabled
    // the copy constructor is disabled 
    DirectionFitBase( const DirectionFitBase& ) ; ///< no copy constructor
    // assigmenent operator is disabled
    DirectionFitBase& operator= ( const DirectionFitBase& ) ; ///< no assignement
    // ========================================================================
  private:
    // ========================================================================
    /// the maximal number of iterations
    unsigned int m_max_iter ; // maximal number of iteration 
    /// the convergency parameter 
    double m_delta_chi2 ; // convergency parameter 
    /// the convergency parameter 
    double m_delta_ctau ; // convergency parameter 
    ///  \f$\mathbf{E}=\frac{\partial}{\partial c\tau}\mathbf{H}\f$
    mutable ROOT::Math::SVector<double,3> m_e   ; // E = d(H)/d(ctau)
    ///  \f$ \mathbf{V}_D = \left(\mathbf{D}^T\mathbf{V}\mathbf{D}\right)^{-1} \f$ 
    mutable Gaudi::SymMatrix3x3           m_vd  ; // V_D = (D^T*V*D)^{-1}
    /// \f$ \lambda_0  = \mathbf{V}_D\left(\mathbf{D}\vec{\alpha}+\mathbf{d}\right) \f$ 
    mutable ROOT::Math::SVector<double,3> m_l0  ; // lambda_0 = V_D*(D*alpha+d) 
    /// \f$ \lambda = \lambda_0 + \mathbf{V}_D\mathbf{E}\delta c\tau \f$ 
    mutable ROOT::Math::SVector<double,3> m_l   ; // lambda   = lambda_0 + V*D*E*dctau 
    /// \f$ \mathbf{d} = \mathbf{H}\left(\vec{\alpha}_0,c\tau_0\right) \f$ 
    mutable ROOT::Math::SVector<double,3> m_d   ; // d = H( alpha=alpha0, ctau=ctau0) 
    /// \f$ \mathbf{W} = \frac{\partial}{\partial p^{\mu}}   \mathbf{H} \f$ 
    mutable Gaudi::Matrix3x4              m_w   ; // W = d(H)/d(p_mu)
    /// \f$ \mathbf{D}_1^T = \mathbf{W}\mathbf{V_{p_{\mu}}} - \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}}^T \f$
    mutable Gaudi::Matrix4x3              m_d1  ; // D_1
    /// \f$ \mathbf{D}_2^T = \mathbf{W} \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}} -  \mathbf{V}_{\vec{x}_d} \f$ 
    mutable Gaudi::Matrix3x3              m_d2  ; // D_2
    /// \f$ -\mathbf{V}_D+\mathbf{V}_D\mathbf{E}^T\mathbf{V}_{c\tau}\mathbf{E}\mathbf{V}_D \f$   
    mutable Gaudi::SymMatrix3x3           m_vd1 ; //  -V_D + V_D * E^T * V_ct * E * V_D  
    /// \f$ \delta\vec{p}^{\mu} \f$ 
    mutable Gaudi::Vector4                m_dp  ; // delta(p)
    /// \f$ \delta \vec{x}_d \f$ 
    mutable Gaudi::Vector3                m_dxd ; // delta(x_d) 
    /// \f$ \delta \vec{x}_p \f$ 
    mutable Gaudi::Vector3                m_dxp ; // delta(x_p)
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKIFITTERS_DIRECTIONFITBASE_H
// ============================================================================

