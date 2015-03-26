// $Id:$ 
// ============================================================================
#ifndef LHCBMATH_MOREFUNCTIONS_H 
#define LHCBMATH_MOREFUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
/** @file LHCbMath/MoreFunctions.h
 *  collection of various helper math functions  
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-03-26
 */  
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Math 
  {
    // ========================================================================
    // some special functions 
    // ========================================================================
    
    // ========================================================================
    /** sum of N-terms in the exponential expansion 
     *  \f$ f (x) = \sum_{n=0}^{N} \frac{x^k}{k!}\f$
     *  @param x  INPUT the argument 
     *  @param N  INPUT N-terms to be used 
     *  @return partial exponential sum 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-03-26
     */
    GAUDI_API double exp_N ( const double x , const unsigned short N ) ;
    // ========================================================================
    /** "relative or reduced exponent"
     *  \f$f(x) = N! ( e^{x} - \sum_{k=0}^{N-1} \frac{x^k}{k!})/x^{N} \f$ 
     *  it also can be written as :
     *  \f$ f(x) =  =  \frac{e^x n!}{x^n} (1 - \Gamma(n,x)/\Gamma(n)) \f$ 
     *  @param x  INPUT the argument 
     *  @param N  INPUT N-terms to be used 
     *  @return the value of "reduced exponent"
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-03-26
     */
    GAUDI_API double exp_rel_N ( const double x , const unsigned short N ) ;
    // ========================================================================
    
    // ========================================================================
    // summation algorithms 
    // ========================================================================
    
    // ========================================================================
    /** Clenshaw algorithm for summation of Chebyshev polynomials 
     *  \f$ f(x) = \sum_i p_i T_i(x)\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double clenshaw_chebyshev 
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // =========================================================================
    /** Clenshaw algorithm for summation of Legendre polynomials 
     *  \f$ f(x) = \sum_i p_i P_i(x) \f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double clenshaw_legendre
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // ========================================================================
    /** Clenshaw algorithm for summation of monomial series 
     *  (aka Horner rule) 
     *  \f$ f(x) = \sum_i a_i x^i \f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double clenshaw_polynom
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // ========================================================================
    /** Clenshaw algorithm for summation of monomial series (aka Horner rule) 
     *  \f$ f(x) = \sum_i a_i x^i \f$, such as \f$f(0)= a_0\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double horner_a0
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // ========================================================================
    /** Clenshaw algorithm for summation of monomial series (aka Horner rule) 
     *  \f$ f(x) = \sum_i a_i x^i \f$, such as \f$f(0)= a_n\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double horner_aN 
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // ========================================================================
    
    // ========================================================================
    // continued fractions 
    // ========================================================================
    
    // ========================================================================
    /** evaluate "simple" continued fraction 
     *  \f$f(x) = a_0 + \frac{1}{ a_1 + \frac{1}{ a_2 + ...} } \f$
     *  @param a  INPUT  coefficients  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double continued_fraction_simple 
    ( const std::vector<double>& a ) ;
    // ========================================================================
    /** evaluate "simple" continued fraction 
     *  \f$f(x) = \frac{b_0}{ 1 + \frac{b_1}{ 1 + ...}} \f$
     *  @param b  INPUT  coefficients  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double continued_fraction_b
    ( const std::vector<double>& b ) ;
    // ========================================================================
    /** evaluate the continued fraction 
     *  \f$f(x) = b_0 + \frac{a_1}{ b_1 + \frac{a_2}{ b_2 + ...}} \f$
     *  @param a  INPUT  coefficients, (length = N  )
     *  @param b  INPUT  coefficients, (length = N or N+1)
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double continued_fraction
    ( const std::vector<double>& a , 
      const std::vector<double>& b ) ;
    // ========================================================================
  } //                                                    end of namespace Math 
  // ==========================================================================
} //                                                     end of namespace Gaudi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LHCBMATH_MOREFUNCTIONS_H
// ============================================================================
