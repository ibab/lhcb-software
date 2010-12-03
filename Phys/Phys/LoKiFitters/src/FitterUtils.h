// $Id: FitterUtils.h,v 1.3 2008-03-10 18:24:43 ibelyaev Exp $
// ============================================================================
#ifndef LOKIFITTERS_FITTERUTILS_H 
#define LOKIFITTERS_FITTERUTILS_H 1
// ============================================================================
// Include files
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb 
{
  class VertexBase ;
  class Particle   ;  
}
// ============================================================================
/** @file
 *  The collection of the useful utilities for imeplmentation of 
 *  various fitters 
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
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-07
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Fitters 
  {
    // ========================================================================
    /// error codes from the various routines 
    enum {
      /// Error in Matrix Inversion (I)
      ErrorInMatrixInversion1      = 403 ,  // Error in Matrix Inversion
      /// Error in Matrix Inversion (II)
      ErrorInMatrixInversion2      = 404 ,  // Error in Matrix Inversion
      /// Negative Error2 
      NegativeSigma2               = 407 ,  // Negative Error
      /// Error from Particle Transporter 
      ErrorFromParticleTransporter = 410    // Error form particle transporter 
    } ;    
    // ========================================================================
    /// the actual type of the function which calculates W=d(H)/d(p_mu) 
    typedef void (*FunW) ( const Gaudi::LorentzVector& ,
                           const double                , 
                           Gaudi::Matrix3x4&           ) ;
    // ========================================================================
    /// the actual type of the function which calculates E=d(H)/d(var)
    typedef void (*FunE) ( const Gaudi::LorentzVector& , 
                           Gaudi::Vector3&             ) ;
    // ========================================================================
    /* @class Fitter1
     * The helper class to store all nesessary matrices and vectors 
     * @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     * @date 2008-03-07
     */
    class Fitter1
    {
    public:
      /// the scalar variable 
      double m_var     ; // the scalar variable 
      /// the covarinace for the scalar varaible 
      double m_Vvar    ; // the covarinace for the scalar variable 
      /// chi2 of the fit procedure 
      double m_chi2  ; // chi2 of the fit procedure 
      /// the vector E = dH(
      ///  \f$\mathbf{E}=\frac{\partial}{\partial var }\mathbf{H}\f$
      Gaudi::Vector3       m_E   ; // E = d(H)/d(var)
      ///  \f$ \mathbf{V}_D = \left(\mathbf{D}^T\mathbf{V}\mathbf{D}\right)^{-1} \f$ 
      Gaudi::SymMatrix3x3  m_VD  ; // V_D = (D^T*V*D)^{-1}
      /// \f$\mathbf{D}\delta\vec{\alpha}_0=\mathbf{D}\left( \vec{\alpha}_0 - \vec{\alpha}_A\right)\f$ 
      Gaudi::Vector3       m_Da0 ; // D*delta(alpha_0) 
      /// \f$ \lambda_0  = \mathbf{V}_D\left(\mathbf{D}\delta\vec{\alpha}_0+\mathbf{d}\right) \f$ 
      Gaudi::Vector3       m_l0  ; // lambda_0 = V_D*(D*delta(alpha)+d) 
      /// \f$ \lambda = \lambda_0 + \mathbf{V}_D\mathbf{E}\delta(var)  \f$ 
      Gaudi::Vector3       m_l   ; // lambda   = lambda_0 + V*D*E*delta(var) 
      /// \f$ \mathbf{d} = \mathbf{H}\left(\vec{\alpha}_0,s_0\right) \f$ 
      Gaudi::Vector3       m_d   ; // d = H( alpha=alpha0, var=var0) 
      /// \f$ \mathbf{W} = \frac{\partial}{\partial p^{\mu}}   \mathbf{H} \f$ 
      Gaudi::Matrix3x4     m_W   ; // W = d(H)/d(p_mu)
      /// \f$ \mathbf{D}_1^T = \mathbf{W}\mathbf{V_{p_{\mu}}} - \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}}^T \f$
      Gaudi::Matrix4x3     m_D1  ; // D_1
      /// \f$ \mathbf{D}_2^T = \mathbf{W} \mathbf{V}_{ \left\{ p_{\mu},\vec{x}_d\right\}} -  \mathbf{V}_{\vec{x}_d} \f$ 
      Gaudi::Matrix3x3     m_D2  ; // D_2
    } ;
    // ========================================================================
    /** the simple function which evaluates E=d(H)/d(var) in the case var=c*tau:
     *  
     *  \f[  \mathbf{E} = 
     *       \frac{\partial}{\partial c\tau}\mathbf{H} = 
     *       \frac{\vec{\mathbf{p}}}{m} 
     *   \f]
     *
     *  @param p (input)  the particle momentum
     *  @param E (output) the vector E 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-07
     */
    void e_ctau ( const Gaudi::LorentzVector& p , Gaudi::Vector3& E ) ;
    // ========================================================================
    /** the simple function which evaluates E=d(H)/d(var) in the case var=path:
     *  
     *  \f[  \mathbf{E} = 
     *       \frac{\partial}{\partial path}\mathbf{H} = 
     *       \frac{\vec{\mathbf{p}}}{\left|\vec{\mathbf{p}}\right|} 
     *   \f]
     *
     *  @param p (input)  the particle momentum
     *  @param E (output) the vector E 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-07
     */
    void e_path ( const Gaudi::LorentzVector& p , Gaudi::Vector3& E ) ;
    // ========================================================================
    /** the simple function which evaluates the first&fast evaluatuon of 
     *  the constrained scalar variable \f$v\f$using the decay vertex, 
     *  primary vertex and the momentum.
     *  The algorithm is applicable for any constraints of the type:
     *  \f[ \mathbf{H} 
     *        = \vec{\mathbf{x}}_{p} 
     *        - \vec{\mathbf{x}}_{d}
     *        + \vec{\mathbf{p}}f\left(p^{\mu}\right)v,\f]
     *  where \f$ \vec{\mathbf{x}}_{p}\f$ is a production vertex, 
     *  \f$ \vec{\mathbf{x}}_{d}\f$ is a deay vertex, 
     *  \f$ \vec{\mathbf{p}}\f$ is partile momentum and 
     *   \f$f\left(\vec{\mathbf{p}},E\right)\f$ is the arbitrary scalar 
     *  function of the particle 4-momentum.
     *
     *  @param primary  (input)  the primary vertex 
     *  @param decay    (input)  the decay vertex
     *  @param momentum (input)  the particle 4-momentum
     *  @param var      (output) the estimator for the variable 
     *  @param funE     (input)  the function, which estimates d(H)/d(var) 
     */
    StatusCode var0 
    ( const LHCb::VertexBase&     primary  ,   // (input)  the primary vertex 
      const LHCb::VertexBase&     decay    ,   // (input)  the decay vertex 
      const Gaudi::LorentzVector& momentum ,   // (input)  4-momentum of the particle
      double&                     var      ,   // (output) the variable 
      LoKi::Fitters::FunE         funE     ) ; // (input) the evaluator of E=d(H)/d(var)
    // ========================================================================
    /** the simple function which evaluates the first&fast evaluatuon of 
     *  the constrained scalar variable \f$v\f$using the decay vertex, 
     *  primary vertex and the momentum.
     *  The algorithm is applicable for any constraints of the type:
     *  \f[ \mathbf{H} 
     *        = \vec{\mathbf{x}}_{p} 
     *        - \vec{\mathbf{x}}_{d}
     *        + \vec{\mathbf{p}}f\left(p^{\mu}\right)v,\f]
     *  where \f$ \vec{\mathbf{x}}_{p}\f$ is a production vertex, 
     *  \f$ \vec{\mathbf{x}}_{d}\f$ is a deay vertex, 
     *  \f$ \vec{\mathbf{p}}\f$ is partile momentum and 
     *   \f$f\left(\vec{\mathbf{p}},E\right)\f$ is the arbitrary scalar 
     *  function of the particle 4-momentum.
     *
     *  @param primary  (input)  the primary vertex 
     *  @param particle (input)  the particle 
     *  @param decay    (input)  the decay vertex
     *  @param var      (output) the estimator for the variable 
     *  @param funE     (input)  the function, which estimates d(H)/d(var) 
     */
    inline StatusCode var0 
    ( const LHCb::VertexBase& primary  , // (input)  the primary vertex 
      const LHCb::Particle&   particle , // (input)  the particle 
      const LHCb::VertexBase& decay    , // (input)  the decay vertex 
      double&                 var      , // (output) the variable 
      LoKi::Fitters::FunE     funE     ) //(input) evaluator of E=d(H)/d(var)
    {
      return var0 ( primary , decay   , particle.momentum() , var , funE ) ;
    }
    // ========================================================================
    /** make the first(fast) evaluation of the proper lifetime of the 
     *  particle \f$c\tau\f$, using the decay vertex, primary vertex 
     *  and the momometum:
     *
     *   \f[  \vec{\mathbf{v}}_{decay} = \vec{\mathbf{v}}_{production} + 
     *           \frac{\vec{\mathbf{p}}}{m}c\tau \f]
     *
     *  @attention essentially it correspond to the first iteration 
     *             of "lifetime" fit neglecting the errors in the 
     *             particle moomentum.
     *  @param primary  (input) the primary vertex 
     *  @param particle (input) the particle itself
     *  @param decay    (input) the decay vertex 
     *  @param ctau     (output) the estimate for c*tau (in units of length, of course)
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-07
     */
    inline StatusCode ctau0 
    ( const LHCb::VertexBase& primary  , // the primary vertex 
      const LHCb::Particle&   particle , // the particle 
      const LHCb::VertexBase& decay    , // the decay vertex 
      double&                 ctau     ) // (output) the c*tau estimate
    {
      return var0 ( primary , particle , decay , ctau , &e_ctau ) ;
    }
    // ========================================================================
    /** make the first(fast) evaluation of the path-distance of  
     *  particle, using the decay vertex, primary vertex and the momometum:
     *
     *   \f[  \vec{\mathbf{v}}_{decay} = \vec{\mathbf{v}}_{production} + 
     *           \frac{\vec{\mathbf{p}}}{\left|\vec{\mathbf{p}}\right|}s \f]
     *
     *  @param primary  (input) the primary vertex 
     *  @param particle (input) the particle itself
     *  @param decay    (input) the decay vertex 
     *  @param path     (output) the path estimate 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-07
     */
    inline StatusCode path0 
    ( const LHCb::VertexBase& primary  , // the primary vertex 
      const LHCb::Particle&   particle , // the particle 
      const LHCb::VertexBase& decay    , // the decay vertez 
      double&                 path     ) // (output) the path
    {
      return var0 ( primary , particle , decay , path , &e_path ) ;
    }
    // ========================================================================
    /** evaluate the matrix W, which is 
     *  
     *   \f[ \mathbf{W} = \frac{\partial}{\partial p^{\mu}}\mathbf{H},\f]
     *
     *   where \f$\mathbf{H}\f$ is a vector of constrains for \f$c\tau\f$-fit:
     *
     *  The direction constraint equations are:
     *   \f[ \mathbf{H} = 
     *        \vec{\mathbf{x}}_{p} -\vec{\mathbf{x}}_{d} 
     *         + \frac{\vec{\mathbf{p}}}{m}c\tau = 
     *        \begin{pmatrix}
     *         x_p - x_d + \frac{p_x}{m}c\tau \\
     *         y_p - y_d + \frac{p_y}{m}c\tau \\
     *         z_p - z_d + \frac{p_z}{m}c\tau 
     *        \end{pmatrix} = 0. \f]
     *
     *  Therefore one gets:
     *
     *  \f[  \mathbf{W} = 
     *       \frac{\partial}{\partial p^{\mu}}\mathbf{H} = 
     *        \frac{c\tau}{m} 
     *       \begin{pmatrix}
     *  1 + \frac{p_xp_x}{m^2} & \frac{p_xp_y}{m2} & \frac{p_xp_y}{m^2} & -\frac{p_xE}{m^2} \\
     *  \frac{p_yp_x}{m^2} & 1 + \frac{p_yp_y}{m2} & \frac{p_yp_y}{m^2} & -\frac{p_yE}{m^2} \\
     *  \frac{p_zp_x}{m^2} & \frac{p_yp_y}{m2} & 1 + \frac{p_zp_y}{m^2} & -\frac{p_zE}{m^2} 
     *       \end  {pmatrix}
     *   \f]
     *
     *  @param p    (input) 4-momentum of the particle 
     *  @param ctau (input) the current estimate for c*tau 
     *  @param W    (output) the matrix W 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-07
     */
    void w_ctau 
    ( const Gaudi::LorentzVector&  p    , 
      const double                 ctau , 
      Gaudi::Matrix3x4&            W    ) ;
    // ========================================================================
    /** evaluate the matrix W, which is 
     *  
     *   \f[ \mathbf{W} = \frac{\partial}{\partial p^{\mu}}\mathbf{H},\f]
     *
     *   where \f$\mathbf{H}\f$ is a vector of constrains for \f$s\f$-fit:
     *
     *  The direction constraint equations are:
     *   \f[ \mathbf{H} = 
     *        \vec{\mathbf{x}}_{p} -\vec{\mathbf{x}}_{d} 
     *         + \frac{\vec{\mathbf{p}}}
     *         {\left|\vec{\mathbf{p}}\right|}c\tau = 
     *         \begin{pmatrix}
     *         x_p - x_d + \frac{p_x}{\left|\vec{\mathbf{p}}\right|}s \\
     *         y_p - y_d + \frac{p_y}{\left|\vec{\mathbf{p}}\right|}s \\
     *         z_p - z_d + \frac{p_z}{\left|\vec{\mathbf{p}}\right|}s 
     *        \end{pmatrix} = 0. \f]
     * 
     *  Therefore one gets:
     *
     *  \f[  \mathbf{W} = 
     *       \frac{\partial}{\partial p^{\mu}}\mathbf{H} = 
     *        \frac{c\tau}{m} 
     *       \begin{pmatrix}
     *  1 - \frac{p_xp_x}{m^2} & - \frac{p_xp_y}{m2} & - \frac{p_xp_y}{m^2} & 0 \\
     *  - \frac{p_yp_x}{m^2} & 1 - \frac{p_yp_y}{m2} & - \frac{p_yp_y}{m^2} & 0 \\
     *  - \frac{p_zp_x}{m^2} & - \frac{p_yp_y}{m2} & 1 - \frac{p_zp_y}{m^2} & 0  
     *       \end  {pmatrix}
     *   \f]
     *
     *  @param p    (input) 4-momentum of the particle 
     *  @param path (input) the current estimate for s 
     *  @param W    (output) the matrix W
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-07
     */
    void w_path
    ( const Gaudi::LorentzVector&  p    , 
      const double                 path , 
      Gaudi::Matrix3x4&            W    ) ;
    // ========================================================================
    /** make one step of the "var-fit"
     *  The assumntions here are:
     *
     *  - the constrain equations \f$\mathbf{H}=\mathbf{0}\f$ have the form:
     *  \f$ \mathbf{H} 
     *        = \vec{\mathbf{x}}_{p} 
     *        - \vec{\mathbf{x}}_{d}
     *        + \vec{\mathbf{p}}f\left(p^{\mu}\right)v,\f$
     *  where \f$ \vec{\mathbf{x}}_{p}\f$ is a production vertex, 
     *  \f$ \vec{\mathbf{x}}_{d}\f$ is a deay vertex, 
     *  \f$ \vec{\mathbf{p}}\f$ is partile momentum and 
     *   \f$f\left(\vec{\mathbf{p}},E\right)\f$ is the arbitrary scalar 
     *  function of the particle 4-momentum.
     *  - the particle is <b>properly</b> propagated into 
     *    its "decay" vertex, therefore one can rely on the 
     *    following functions as parameters of "decay" vertex:
     *    -# <c>referencePoint() </c>
     *    -# <c>posCovMatrix()   </c>
     *    -# <c>posMomCovMatrix()</c>
     *
     *   @param primary    (input)  the primary vertex 
     *   @param particle   (input)  the properly propagated particle 
     *   @param momentum   (update) the expansion point/updated particle momentum 
     *   @param decvertex  (update) the expansion point/updated decay vertex position
     *   @param primvertex (update) the expansion point/updated primary vertex position
     *   @param fit        (update) the full results of the fit 
     *   @param funW       (input)  the function to evaluate d(H)/d(p^mu)
     *   @param funE       (input)  the function to evaluate d(H)/d(var)
     *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *   @date 2008-03-07
     */
    StatusCode var_step 
    ( const LHCb::VertexBase& primary    , // the primary vertex 
      const LHCb::Particle&   particle   , // the properly propagated particle 
      Gaudi::LorentzVector&   momentum   , // the expansion point/updated particle momentum 
      Gaudi::XYZPoint&        decvertex  , // the expansion point/updated decay vertex position 
      Gaudi::XYZPoint&        primvertex , // the expansion point/updated primary vertex position 
      LoKi::Fitters::Fitter1& fit        , // the full results of the fit 
      LoKi::Fitters::FunW     funW       , // the function to evaluate d(H)/d(p^mu)
      LoKi::Fitters::FunE     funE       ) ; // the function to evaluate d(H)/d(var)
    // ========================================================================
    /** make one step for \f$c\tau\f$-fit 
     *  @attention It is assumed that the particle is
     *             properly propagated into its decay vertex 
     *  @param primary    (input)  the primary vertex 
     *  @param particle   (input)  the properly propagated particle 
     *  @param momentum   (update) the expansion point/updated particle momentum 
     *  @param decvertex  (update) the expansion point/updated decay vertex position
     *  @param primvertex (update) the expansion point/updated primary vertex position
     *  @param fit        (update) the full results of the fit 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-07
     */
    inline StatusCode ctau_step 
    ( const LHCb::VertexBase& primary    , // the primary vertex 
      const LHCb::Particle&   particle   , // the properly propagated particle 
      Gaudi::LorentzVector&   momentum   , // the expansion point/updated particle momentum 
      Gaudi::XYZPoint&        decvertex  , // the expansion point/updated decay vertex position 
      Gaudi::XYZPoint&        primvertex , // the expansion point/updated primary vertex position 
      LoKi::Fitters::Fitter1& fit        ) // the full results of the fit 
    {
      return var_step 
        ( primary   , particle   , momentum , 
          decvertex , primvertex , fit      , 
          &w_ctau ,   // the actual function to evaluate d(H)/d(p^mu)
          &e_ctau ) ; // the actual function to evaluate d(H)/d(var)
    }
    // ========================================================================
    /** make one step for "path"-fit 
     *  @attention it is assumed that the particle is
     *             properly propagated into its decay vertex 
     *  @param primary    (input)  the primary vertex 
     *  @param particle   (input)  the properly propagated particle 
     *  @param momentum   (update) the expansion point/updated particle momentum 
     *  @param decvertex  (update) the expansion point/updated decay vertex position
     *  @param primvertex (update) the expansion point/updated primary vertex position
     *  @param fit        (update) the full results of the fit 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-07
     */
    inline StatusCode path_step 
    ( const LHCb::VertexBase& primary    , // the primary vertex 
      const LHCb::Particle&   particle   , // the properly propagated particle 
      Gaudi::LorentzVector&   momentum   , // the expansion point/updated particle momentum 
      Gaudi::XYZPoint&        decvertex  , // the expansion point/updated decay vertex position 
      Gaudi::XYZPoint&        primvertex , // the expansion point/updated primary vertex position 
      LoKi::Fitters::Fitter1& fit        ) // the full results of the fit 
    {
      return var_step 
        ( primary    , particle   , momentum   , 
          decvertex  , primvertex , fit        , 
          &w_path    ,   // the actual function to evaluate d(H)/d(p^mu)
          &e_path    ) ; // the actual function to evaluate d(H)/d(var)
    }
    // ========================================================================
  } //                                           end of namespace LoKi::Fitters 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKIFITTERS_FITTERUTILS_H
// ============================================================================
