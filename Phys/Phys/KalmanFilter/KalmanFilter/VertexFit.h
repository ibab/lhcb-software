// $Id:$ 
// ============================================================================
#ifndef LOKIFITTERS_KALMANFILTER_H 
#define LOKIFITTERS_KALMANFILTER_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKenrel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Point4DTypes.h"
// ============================================================================
// Event 
// ============================================================================
#include  "Event/Particle.h"
#include  "Event/Vertex.h"
// ============================================================================
// KalmanFilter
// ============================================================================
#include "KalmanFilter/ParticleTypes.h"
// ============================================================================
// forward declarations 
// ============================================================================
class IParticleTransporter ;
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-04-12
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ===========================================================================
  /** @namespace LoKi::KalmanFilter KalmanFilter.h
   *
   *  Helper namespace with various utility classes, 
   *  useful for the Kalman-filter-based implementation
   *  of vertex fit
   *
   *  <b>"Vertex Fitter as Kalman Filter"</b>
   *
   *  Let  \f$\vec{p}_k\f$ be a vector containing the parameters of the 
   *  \f$k\f$-track. Let  \f$V_k\f$ be the covariance matrix of these parameters, 
   *  and  \f$G_k\f$ is <b>the formal</b> inverse matrix: \f$G_k = V_k^{-1}\f$.
   *  For the trivial case of \f$dim(\vec{p}_k)=5\f$ both matrices 
   *  \f$V_k\f$ and \f$G_k\f$ could be defined properly and are non-singular.
   *
   *  Here we'll try to extend the standard Kalman Filter formalism 
   *  for <b>almost</b> linear model, by embedding the 
   *  minimal \f$dim=5\f$-model into more dimensions. 
   *  In particular we choose the \f$dim(\vec{p}_k)=7\f$:
   *  \f[ \vec{p} = p^T = 
   *      \left( \vec{x} , \vec{q} \right) = 
   *      \left( x , y , z , p_x , p_y , p_z , E \right)  
   *  \f] 
   *
   * The vertex fit determines the new parameters 
   * \f$\vec{x}\f$- position of the common vertex and
   * \f$\vec{q}\f$- 4-momentum of the track, 
   * constrained to originate in \f$\vec{x}\f$.
   *
   * 
   * The covariance matrix of fit parameters could be split into sub-matrices:
   *
   *  - the vertex part \f$ 3\times3\f$:       \f$C = cov\left( \vec{x} \right)\f$  
   *  - particle momentum part \f$4\times4\f$: \f$D_k = cov\left( \vec{q}_k \right)\f$ 
   *  - correlation between the vertex position and the particle 
   *    momentum part \f$3\times4\f$: \f$E_k = cor\left( \vec{x},\vec{q}_k \right)\f$ 
   *
   *  - correlation between the momenta of different particles \f$4\times4\f$:
   *    \f$S_{k,l} = cor\left( \vec{q}_l,\vec{q}_k \right)\f$  
   *
   *  Such choice of \f$\vec{p}\f$ and \f$\vec{q}\f$ 
   *  allows to obtain almost trivial projection matrices \f$A\f$ and 
   *  \f$B\f$:
   *  \f[ \vec{p}_k \left( \vec{x}_k , \vec{q}_k \right) = 
   *      c_k^0 +  A_k \vec{x} + B_k \vec{q},  \f]
   *  where 
   *  \f$ A_k = \left[ \frac{\partial \vec{p} }{\partial \vec{x}} \right] \f$ and 
   *  \f$ B_k  = \left[ \frac{\partial \vec{p} }{\partial \vec{q}} \right] \f$.
   *  Clearly with the the chosen  parameterization of \f$\vec{p}\f$ one gets:
   *  \f$A_k = A = \bigl( \begin{smallmatrix}1 \\ 0\end{smallmatrix}\bigr)\f$, 
   *  \f$B_k = B = \bigl( \begin{smallmatrix}0 \\ 1\end{smallmatrix}\bigr)\f$. 
   *
   *
   *  The Kalman filter works as:
   * 
   *  - \f$ C^{-1}_k = C^{-1}_{k-1} + A^T_{k}G_{k}^{B}A_{k} \f$ 
   *  - \f$ E_k      = -F_{k}C_{k}  \f$ 
   *  - \f$ D_{k}    = W_{k} - E_{k}F^{T}_{k} \f$
   *  
   *  where:
   *  
   *  - \f$ W_{k}     = \left( B^T_{k}G_{k}B_{k}\right)^{-1}  \f$ 
   *  - \f$ G^{B}_{k} = G_{k} - G_{k}B_{k}W_{k}B^T_{k}G_{k} \f$ 
   *  - \f$ F_{k}     = W_{k}B^T_{k}G_{k}A_{k} \f$ 
   *
   *  New fit values are:
   *
   * - \f$ \vec{x}_{k} = C_{k}\left(C^{-1}_{k-1}\vec{x}{k-1}
   *           + A^T_{k}G^B_{k}\left(\vec{p}_{k}-c^0_{k}\right)\right) \f$
   * - \f$ \vec{q}_{k} = W_{k}B^T_{k}G_{k}\left(\vec{p}_{k}
   *           - c^0_k-A_k\vec{x}_k\right) \f$ 
   * 
   *  New \f$\chi^2\f$ is :
   *   \f$ \chi^2_{k} = \chi^2_{k-1} + \chi_1 +\chi_2\f$, where 
   *  - \f$ \chi_1 = \vec{\delta_1}^{T} G_k \vec{\delta_1} \f$ 
   *  - \f$ \chi_2 = \vec{\delta_2}^{T} C^{-1}_{k-1} \vec{\delta_2} \f$ 
   *  - \f$ \vec{\delta}_1 = \vec{p}-c^0_{k}-A_{k}\vec{x}-B_{k}\vec{q}_{k}\f$ 
   *  - \f$ \vec{\delta}_2 = \vec{x}_{k}-\vec{x}_{k-1} \f$ 
   * 
   *  The Kalman smoothing step is fairly trivial:
   *  
   *  - \f$ \vec{x}^{n}_{k} = \vec{x}_{n} \f$
   *  - \f$ \vec{q}^{n}_{k} = W_{k}B^T_{k}G_{k}\left[\vec{p}-A_{k}\vec{x}_{n}\right] \f$
   *  
   *  After the iteration the covariance matrices are calculated as:
   *  
   *  - \f$ C^{n}_{k} = C_{n} \f$ 
   *  - \f$ E^{n}_{k} = -F_{k}C_{n} \f$ 
   *  - \f$ D^{n}_{k} = W_k - E^{n}_{k}F^T_{k} \f$ 
   *  - \f$ cov(\vec{q}^{n}_{k},\vec{q}^{n}_{l}) = F_{k}C_{n}F^T_{l} \f$ 
   *
   *  Since for our parameterization 
   *  \f$A_k = A = \bigl( \begin{smallmatrix}1 \\ 0\end{smallmatrix}\bigr)\f$, 
   *  \f$B_k = B = \bigl( \begin{smallmatrix}0 \\ 1\end{smallmatrix}\bigr)\f$, 
   *  all formulare become relaively simle:
   *  
   *  Assuming that 
   *  \f$ G_{k} = \bigl( \begin{smallmatrix} 
   *       G_x  & G_{xp} \\ G^T_{xp} & G_{p} \end{smallmatrix}\bigr), \f$
   *  one has
   *  
   *  - \f$ W_k = G_p^{-1} \f$ 
   *  - \f$ G^B_k = \bigl( \begin{smallmatrix}
   *   G_x - G_{xp}G_{p}^{-1}G^{T}_{xp} & 0 \\ 0 & 0 \end{smallmatrix} \bigr) \f$ 
   *  - \f$ F_k       = G_p^{-1} G^{T}_{xp} \f$ 
   *  - \f$ C_k^{-1}  = C_{k-1}^{-1} + \left( G_x - G_{xp}G_p^{-1}G_{xp}^T\right) \f$ 
   *  - \f$ D_k       = G_p^{-1}+ G_p^{-1}G^T_{xp}C_kG_{xp}G_p^{-1} \f$ 
   *  - \f$ \vec{x}_x = C_k\left( C^{-1}_{k-1}\vec{x}_{k-1}+
   *            \left( G_x - G_{xp}G^{-1}_{p}G^{T}_{xp} , 0 \right) 
   *            \left( \vec{p}_k - c^0_k \right) \right) \f$ 
   *  - \f$ \chi^2_{k} = \chi^2_{k-1} + \delta_{\vec{x}}^T 
   *           \left[ G_x - G_{xp}G_{p}^{-1}G^T_{xp} \right] \delta_{\vec{x}} 
   *           + ... \f$ 
   *
   *
   *  In an obvious way:
   *
   *  - \f$ G_x - G_{xp}G_{p}^{-1}G_{xp} = V^{-1}_{x}     \f$ 
   *  - \f$ G_{p}^{-1} = V_{p} - V^T_{xp}V^{-1}_{x}V_{xp} \f$ 
   *  - \f$ G^{-1}G^T_{xp} = -V^T_{xp}V^{-1}_{x}          \f$ 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-04-12
   *
   *  For details see:
   *  R.K.Bock et al., "Data analysis techniques for high-energy physics experiemnts", 
   *                   Cambridge Univ.Press (1990), p319
   *
   *                    $Revision: 114335 $
   *  Last modification $Date: 2010-12-03 10:13:51 +0100 (Fri, 03 Dec 2010) $
   *                 by $Author: ibelyaev $
   *    
   */
  namespace KalmanFilter
  {
    // ========================================================================
    /** @class Entry
     *  Helper class useful for kinematical fits.
     *  It stores in the useful format the most important 
     *  values needed for the vertex Kalman fit procedure 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-04-12
     */
    class GAUDI_API Entry 
    {
    public:
      // ======================================================================
      ParticleType          m_type ; // the particle type 
      /// pointer to the original particle 
      const LHCb::Particle* m_p0   ; // pointer to the original particle 
      /// the properly extrapolated (local) particle 
      LHCb::Particle        m_p    ; // properly extrapolated (local) particle 
      // "Measurements": The parameter vector alpha=(x,y,z,px,py,pz,e)
      /// \f$\vec{x}=(x,y,z)\f$ 
      Gaudi::Vector3        m_parx ; // x=(x,y,z)  
      /// \f$\vec{q}=(p_x,p_y,p_z,E)\f$ 
      Gaudi::Vector4        m_parq ; // q=(p_x,p_y,p_z,E)
      // inverse of the (modified?) covariance submatrix for (x,y,z)
      /// \f$C_{\vec{x}}^{-1}\f$
      Gaudi::SymMatrix3x3   m_vxi  ;  // C(x,x)^{-1}
      //      Projectors are trivial and omitted 
      //      Kalman filter values:
      /// \f$\chi^2\f$
      double                            m_chi2 ; // chi2 of kalman step/fit 
      /// \f$\vec{x}=(x,y,z)\f$ 
      Gaudi::Vector3                    m_x    ; // vertex position  (x,y,z)
      /// \f$C_{\vec{x}}=cov(\vec{x})\f$
      Gaudi::SymMatrix3x3               m_c    ; // position errors  
      /// \f$C_{\vec{x}}^{-1}\f$
      Gaudi::SymMatrix3x3               m_ci   ; // position errors(inverse), gain matrix 
      /// \f$\vec{q}=(p_x,p_y,p_z,E)\f$ 
      Gaudi::Vector4                    m_q    ; // parameter vector (px,py,pz,e)
      /// \f$C_q=cov(\vec{q})\f$
      Gaudi::SymMatrix4x4               m_d    ; // covariance matrix for q-parameters
      /// \f$E=cov(\vec{x},vec{q})\f$
      Gaudi::Matrix4x3                  m_e    ; // covariance matrix for (x,q)
      /// \f$F=WB^{T}GA\f$
      Gaudi::Matrix4x3                  m_f    ; // auxillary matrix F 
      // ======================================================================
    public:
      // ======================================================================
      /// is it the liong lived entry ? 
      bool isLong  () const { return LongLivedParticle  == m_type ; }
      /// is it the liong lived entry ? 
      bool isShort () const { return ShortLivedParticle == m_type ; }
      /// is it the regular entry ? 
      bool regular () const { return !special() ; }
      /// is it the special entry ? 
      bool special () const 
      {
        return
          GammaLikeParticle   == m_type || 
          DiGammaLikeParticle == m_type ;
      }
      /// is it the liong lived entry ? 
      bool isType  ( ParticleType t ) const { return t == m_type ; }
      // ======================================================================
    } ;
    // ========================================================================
    /// the actual type of the container of entries 
    typedef std::vector<Entry>                    Entries ;
    // ========================================================================
    /** Load the particle into "entry" representation"
     *
     *  Essentially it includes also the proper tarnsformation of the 
     *  covariance matrix of the particle 
     *
     *  @param  particle (input)  the particle to be added 
     *  @param  entry    (output) the resulting entry 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode load 
    ( const LHCb::Particle&      particle , 
      LoKi::KalmanFilter::Entry& entry    ) ;
    // ========================================================================
    /** Load the particle into "entry" representation"
     *
     *  Essentially it includes also the proper tarnsformation of the 
     *  covariance matrix of the particle 
     *
     *  @param  particle (input)  the particle to be added 
     *  @param  ptype    (input)  the particle type 
     *  @param  entry    (output) the resulting entry 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode load 
    ( const LHCb::Particle&                  particle , 
      const LoKi::KalmanFilter::ParticleType ptype    ,
      LoKi::KalmanFilter::Entry&             entry    ) ;
    // ========================================================================
    /** Load the particle into "entry" representation"
     *
     *  Essentially it includes also the proper transformation 
     *  of the covariance matrix of the particle 
     *
     *  @param  particle (input)  the particle to be added 
     *  @param  entry    (output) the resulting entry 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode loadAsFlying
    ( const LHCb::Particle&      particle , 
      LoKi::KalmanFilter::Entry& entry    ) ;
    // ========================================================================
    /** Load the particle into "entry" representation"
     *
     *  Essentially it includes also the proper tarnsformation 
     *  of the covariance matrix of the particle 
     *
     *  @param  particle (input)  the particle to be added 
     *  @param  entry    (output) the resulting entry 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-01-04
     */
    GAUDI_API 
    StatusCode loadAsGamma
    ( const LHCb::Particle&      particle , 
      LoKi::KalmanFilter::Entry& entry    ) ;
    // ========================================================================
    /** Load the particle into "entry" representation"
     *
     *  Essentially it includes also the proper tarnsformation 
     *  of the covariance matrix of the particle 
     *
     *  @param  particle (input)  the particle to be added 
     *  @param  entry    (output) the resulting entry 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-11-11
     */
    GAUDI_API 
    StatusCode loadAsDiGamma
    ( const LHCb::Particle&      particle , 
      LoKi::KalmanFilter::Entry& entry    ) ;
    // ========================================================================
    /** Load the particle into "entry" representation"
     *
     *  Essentially it includes also the proper tarnsformation 
     *  of the covariance matrix of the particle 
     *
     *  @attention It is not implemented properly yet! 
     *
     *  @param  particle (input)  the particle to be added 
     *  @param  entry    (output) the resulting entry 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-01-04
     */
    GAUDI_API 
    StatusCode loadAsShortLived
    ( const LHCb::Particle&      particle , 
      LoKi::KalmanFilter::Entry& entry    ) ;
    // ========================================================================
    /** transport the regular entry into new Z 
     *  @param entry (UPATE) the entry to be transported 
     *  @param newZ  (INPUT) new Z-position 
     *  @param tool  (INPUT) the particle transporter tool
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode transport 
    ( LoKi::KalmanFilter::Entry& entry    , 
      const double               newZ     , 
      IParticleTransporter*      tool     ) ;
    // ========================================================================
    /** transport the entry into new point 
     *  @param entry     (UPDATE) the entry to be transported 
     *  @param point     (INPUT)  new position
     *  @param tool      (INPUT)  the particle transporter tool
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode transport 
    ( LoKi::KalmanFilter::Entry& entry    , 
      const Gaudi::Vector3&      point    , 
      IParticleTransporter*      tool     ) ;
    // ========================================================================
    /** transport the entry into new point 
     *  @param entry     (UPDATE) the entry to be transported 
     *  @param point     (INPUT)  new position 
     *  @param tool      (INPUT)  the particle transporter tool
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode transport 
    ( LoKi::KalmanFilter::Entry& entry    , 
      const Gaudi::XYZPoint&     point    , 
      IParticleTransporter*      tool     ) ;
    // ========================================================================
    /** transport gamma-like and digamma-like particles into new point 
     *  @param entry     (UPDATE) the entry to be transported
     *  @param point     (INPUT)  new position 
     *  @param pointCov2 (INPUT)  covariance matrix for new point 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode transportGamma  
    ( LoKi::KalmanFilter::Entry& entry         , 
      const Gaudi::XYZPoint&     point         , 
      const Gaudi::SymMatrix3x3* pointCov2 = 0 ) ;
    // ========================================================================
    /** transport gamma-like and digamma-like particles into new point 
     *  @param entry     (UPDATE) the entry to be transported
     *  @param point     (INPUT)  new position 
     *  @param pointCov2 (INPUT)  covariance matrix for new point 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode transportGamma  
    ( LoKi::KalmanFilter::Entry& entry         , 
      const Gaudi::Vector3&      point         , 
      const Gaudi::SymMatrix3x3* pointCov2 = 0 ) ;
    // ========================================================================
    /** make one step of Kalman filter 
     *  @param entry (update)       measurement to be updated 
     *  @param x     (input)        the initial position of the vertex 
     *  @param ci    (input)        its gain matrix 
     *  @param chi2  (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode step 
    ( LoKi::KalmanFilter::Entry&  entry , 
      const Gaudi::Vector3&       x     , 
      const Gaudi::SymMatrix3x3&  ci    , 
      const double                chi2  ) ;
    // ========================================================================    
    /** make the special step of Kalman filter from two entries 
     *  @param entry1 (update)       measurements to be updated 
     *  @param entry2 (update)       measurements to be updated 
     *  @param chi2   (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode step 
    ( LoKi::KalmanFilter::Entry&  entry1 ,
      LoKi::KalmanFilter::Entry&  entry2 , 
      const double                chi2   ) ;
    // ========================================================================    
    /** make the special step of Kalman filter from three entries 
     *  @param entry1 (update)       measurements to be updated 
     *  @param entry2 (update)       measurements to be updated 
     *  @param entry3 (update)       measurements to be updated 
     *  @param chi2   (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode step 
    ( LoKi::KalmanFilter::Entry&  entry1 ,
      LoKi::KalmanFilter::Entry&  entry2 , 
      LoKi::KalmanFilter::Entry&  entry3 , 
      const double                chi2   ) ;
    // ========================================================================    
    /** make the special step of Kalman filter from four entries 
     *  @param entry1 (update)       measurements to be updated 
     *  @param entry2 (update)       measurements to be updated 
     *  @param entry3 (update)       measurements to be updated 
     *  @param entry4 (update)       measurements to be updated 
     *  @param chi2   (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode step 
    ( LoKi::KalmanFilter::Entry&  entry1 ,
      LoKi::KalmanFilter::Entry&  entry2 , 
      LoKi::KalmanFilter::Entry&  entry3 , 
      LoKi::KalmanFilter::Entry&  entry4 , 
      const double                chi2   ) ;
    // ========================================================================    
    /** make the special step of Kalman filter for rho+-ilke combination 
     *  @param entries (update)  the entries 
     *  @param chi2    (input)   the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode stepRho
    ( LoKi::KalmanFilter::Entries& entries , 
      const double                 chi2    ) ;
    // ========================================================================    
    /** make one step of Kalman filter 
     *  @param entry (update)       measurement to be updated 
     *  @param x     (input)        the initial position of the vertex 
     *  @param ci    (input)        its gain matrix 
     *  @param chi2  (input)        the initial chi2 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode step 
    ( LoKi::KalmanFilter::Entry&  entry , 
      const Gaudi::XYZPoint&      x     , 
      const Gaudi::SymMatrix3x3&  ci    , 
      const double                chi2  ) ;
    // ========================================================================    
    /** run Kalman smoothing 
     *  @param entries (update) the entries to be smoothed 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode smooth ( LoKi::KalmanFilter::Entries&  entries ) ;
    // ========================================================================
    /** evaluate all covariances 
     *  @param  entries (update) the entries to be updated 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode evalCov ( LoKi::KalmanFilter::Entries&  entries ) ;
    // ========================================================================
    /** construct the seed from the entries 
     *  @param entries (input)  the list of entries 
     *  @param x       (output) the position of "seed"
     *  @param ci      (output) the gain matrix for the seed 
     *  @param scale   (input)  the scale factor for gain matrix
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode seed 
    ( const LoKi::KalmanFilter::Entries& entries        ,
      Gaudi::Vector3&                    x              , 
      Gaudi::SymMatrix3x3&               ci             , 
      const double                       scale = 1.e-4  ) ;
    // ========================================================================
    /** construct the seed from the entries 
     *  @param entries (input)  the list of entries 
     *  @param x       (output) the position of "seed"
     *  @param ci      (output) the gain matrix for the seed 
     *  @param scale   (input)  the scale factor for gain matrix
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-06
     */
    GAUDI_API 
    StatusCode seed 
    ( const LoKi::KalmanFilter::Entries& entries        ,
      Gaudi::XYZPoint&                   x              , 
      Gaudi::SymMatrix3x3&               ci             , 
      const double                       scale = 1.e-4  ) ;
    // ========================================================================      
    /** get number of 'good' entries for verticing 
     *  @param entries   (input) the vector of entries 
     *  @return number of good entries for verticing
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-24
     */   
    GAUDI_API 
    unsigned short nGood ( const Entries& entries ) ;
    // ========================================================================      
    /** check if the collection of entries is OK for vertex:
     *   - either at least one short-lived particle 
     *   - or at least two long-lived particles 
     *  @return true of collection of entries is OK 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-24
     */   
    GAUDI_API 
    bool okForVertex ( const Entries& entries ) ;
    // ========================================================================
    /** check if the collection of entries is ``rho+''-like 
     *   - exactly one long-lived particle (track) 
     *   - and the cetrain amount of ``gamma/di-gamma''-like particles 
     *  @return true of collection of entries is OK 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-24
     */   
    GAUDI_API 
    bool rhoPlusLike ( const Entries& entries ) ;
    // ========================================================================
    /** check if the collection of entries is ``two-prong''-particle 
     *  @return true of collection of entries is OK 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-24
     */   
    GAUDI_API 
    bool twoProngs ( const Entries& entries ) ;
    // ========================================================================
    /** check if the collection of entries is ``three-prong''-particle 
     *  @return true of collection of entries is OK 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-24
     */   
    GAUDI_API 
    bool threeProngs  ( const Entries& entries ) ;
    // ========================================================================
    /** check if the collection of entries is ``four-prong''-particle 
     *  @return true of collection of entries is OK 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-24
     */   
    GAUDI_API 
    bool fourProngs  ( const Entries& entries ) ;
    // ========================================================================
    /** calculate number degrees of freedom for vertex fit 
     *  @thanks Yuehong Xie 
     *  @thanks Wouter Hulsbergen
     *  @thanks Fred Blanc 
     *  @param entries (input) vector of entries 
     *  @return number of degrees of freedom 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-09-26
     */
    GAUDI_API 
    int nDoF ( const Entries& entries ) ;
    // ========================================================================
    /** calculate the chi2 
     *  @param entries (input) vector of entries 
     *  @return chi2 of vertex 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-07-19
     */
    GAUDI_API 
    double chi2 ( const Entries& entries ) ;
    // ========================================================================
  } //                                      end of namespace LoKi::KalmanFilter
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKIFITTERS_KALMANFILTER_H
// ============================================================================
