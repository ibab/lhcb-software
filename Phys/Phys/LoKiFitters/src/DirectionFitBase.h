// $Id: DirectionFitBase.h,v 1.1.1.1 2008-02-20 15:48:44 ibelyaev Exp $
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
   *  @see IDirectionFit
   *  @see ILifetimeFitter
   *  @see IParticleReFitter
   *  @see LoKi::LifetimeFitter
   *  @see LoKi::DirectionFitter
   *
   *  @author Vanya BELYAEV
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
     *  Essentially it corresponds to the first iteration with 
     *   W=d(H)/d(m_mu)=0 
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
    /** calculate w-matrix: W = d(H)/d(p_mu)
     *
     *  It is the only non-trivial component of the general matrix D:
     *   D = d(H)/d(alpha) = ( W , -1 , 1 ) 
     *
     *  @param v    the lorents-vector     (input) 
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
    // maximal number of iteration 
    unsigned int m_max_iter ;
    // convergency parameter 
    double m_delta_chi2 ; ///< convergency parameter 
    // convergency parameter 
    double m_delta_ctau ; ///< convergency parameter 
    //
    mutable ROOT::Math::SVector<double,3> m_e   ; ///< d(H)/d(ctau)
    mutable Gaudi::SymMatrix3x3           m_vd  ; ///<  (D^T*V*D)^{-1}
    mutable ROOT::Math::SVector<double,3> m_l0  ; ///< V_D(D*alpha+d) 
    mutable ROOT::Math::SVector<double,3> m_l   ; ///< Lam0 + V_D*E*dctau
    mutable ROOT::Math::SVector<double,3> m_d   ; ///< H(0,0) 
    mutable Gaudi::Matrix3x4              m_w   ; ///< dH/dp_mu 
    mutable Gaudi::Matrix3x4              m_d1  ; ///< DV (1)
    mutable Gaudi::Matrix3x3              m_d2  ; ///< DV (2)
    mutable Gaudi::SymMatrix3x3           m_vd1 ; ///<  -V_D + V_D * E^T * V_ct * E * V_D  
    //
    mutable Gaudi::Vector4                m_dp  ; ///< delta(p)
    mutable Gaudi::Vector4                m_dxd ; ///< delta(x_d)
    mutable Gaudi::Vector4                m_dxp ; ///< delta(x_p)
    // ========================================================================    
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKIFITTERS_DIRECTIONFITBASE_H
// ============================================================================

