// $Id: Kinematics.h,v 1.2 2008-12-02 15:54:30 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_KINEMATICS_H 
#define LHCBMATH_KINEMATICS_H 1
// ============================================================================
// Include files
// ============================================================================
// ROOT
// ============================================================================
#include "Math/SMatrix.h"
#include "Math/SVector.h"
#include "Math/Vector4D.h"
// ============================================================================
/** @file 
 *
 *  Collection of useful mathematical utilities related to the kinematics
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2008-01-15
 */
namespace Gaudi
{
  // ==========================================================================
  namespace Math 
  {   
    // ========================================================================
    /** evaluate the dispersion of M^2 from the particle 4-vector and 
     *  the covarinace matrix
     *
     *  @code
     *
     *   const LHCb::Particle* p = ... ;
     *   double s2m2 = sigma2mass2 ( p -> momentum() , p -> momCovMatrix() ) ; 
     *
     *  @endcode
     *  
     *  @param momentum   (in) the particle momentum
     *  @param covariance (in) 4x4 covarinnce matrix
     *  @return the estimate for dispersion of M^2
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-15
     */
    template <class C, class T>
    inline double
    sigma2mass2 
    ( const ROOT::Math::LorentzVector<C>&                           momentum   , 
      const ROOT::Math::SMatrix<T,4,4,ROOT::Math::MatRepSym<T,4> >& covariance ) 
    {
      // get the vector d(M2)/dp_i :
      ROOT::Math::SVector<T,4> dM2dp;
      dM2dp [0] = -2 * momentum.Px () ;
      dM2dp [1] = -2 * momentum.Py () ;
      dM2dp [2] = -2 * momentum.Pz () ;
      dM2dp [3] =  2 * momentum.E  () ;
      //
      return ROOT::Math::Similarity ( covariance , dM2dp ) ;
    }
    // ========================================================================
    /** evaluate the dispersion of M from the particle 4-vector and 
     *  the covarinace matrix
     *
     *  @code
     *
     *   const LHCb::Particle* p = ... ;
     *   double s2m = sigma2mass ( p -> momentum() , p -> momCovMatrix() ) ; 
     *
     *  @endcode
     *  
     *  @attention the correct result is returned only for time-like vectors! 
     * 
     *  @param momentum   (in) the particle momentum
     *  @param covariance (in) 4x4 covarinnce matrix
     *  @return the estimate for dispersion of M
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-15
     */
    template <class C, class T>
    inline double
    sigma2mass 
    ( const ROOT::Math::LorentzVector<C>&                           momentum   , 
      const ROOT::Math::SMatrix<T,4,4,ROOT::Math::MatRepSym<T,4> >& covariance ) 
    {
      const double s2m2 = sigma2mass2( momentum , covariance ) ;
      const double m2   = momentum.M2 () ;
      if ( 0 < m2 ) { return 0.25 * s2m2 / m2 ; }
      return -1000000 ;                                          // RETURN 
    }
    // ========================================================================
    /** evaluate sigma(M) from the particle 4-vector and 
     *  the covarinace matrix
     *
     *  @code
     *
     *   const LHCb::Particle* p = ... ;
     *   double sigma = sigmamass ( p -> momentum() , p -> momCovMatrix() ) ; 
     *
     *  @endcode
     *  
     *  @attention the correct result is returned only for time-like vectors!
     * 
     *  @param momentum   (in) the particle momentum
     *  @param covariance (in) 4x4 covarinnce matrix
     *  @return the estimate for dispersion of M
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-15
     */
    template <class C, class T>
    inline double
    sigmamass 
    ( const ROOT::Math::LorentzVector<C>&                           momentum   , 
      const ROOT::Math::SMatrix<T,4,4,ROOT::Math::MatRepSym<T,4> >& covariance ) 
    {
      const double s2m = sigma2mass ( momentum , covariance ) ;
      if ( 0 < s2m ) { return ::sqrt ( s2m ) ; }
      return s2m ;
    }
    // ========================================================================
    /** evaluate the chi2 of the mass 
     *
     *  @code
     *
     *   const LHCb::Particle* B = ... ;
     *
     *   const double chi2 = 
     *       chi2mass ( 5.279 * Gaudi::Units::GeV , 
     *                  B -> momentum()           , 
     *                  B -> momCovMatrix()       ) ; 
     *
     *  @endcode
     *  
     *  @param mass       (in) nominal mass
     *  @param momentum   (in) 4-momentum of the particle
     *  @param covariance (in) 4x4-covariance matrix 
     *  @return chi2 of the delta mass 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-15
     */
    template <class C, class T>
    inline double chi2mass 
    ( const double                                                  mass       , 
      const ROOT::Math::LorentzVector<C>&                           momentum   , 
      const ROOT::Math::SMatrix<T,4,4,ROOT::Math::MatRepSym<T,4> >& covariance ) 
    {
      // sigma^2(M^2):
      const double s2 = 1.0 / Gaudi::Math::sigma2mass2 ( momentum , covariance ) ;
      // delta(M^2)
      const double dm2 = momentum.M2() - mass * mass ;
      //  (delta^2(M^2))/(sigma^2(M^2))
      return ( dm2 * dm2 ) * s2 ;
    }
    // ========================================================================
  } // end of namespace Gaudi::Math
  // ==========================================================================
} // end of namespace Gaudi
// ============================================================================
// The END
// ============================================================================
#endif // LHCBMATH_KINEMATICS_H
// ============================================================================
