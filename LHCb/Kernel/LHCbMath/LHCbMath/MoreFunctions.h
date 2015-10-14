// $Id$ 
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
     *  Abramowitz & Stegun, 6.5.11
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
     *  Abramowitz & Stegun, 4.2.41
     *  @param x  INPUT the argument 
     *  @param N  INPUT N-terms to be used 
     *  @return the value of "reduced exponent"
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-03-26
     */
    GAUDI_API double exp_rel_N  ( const double x , const unsigned short N ) ;
    // ========================================================================
    /** regularized incomplete gamma function 
     *  \f$ \gamma^{\ast}(a,x) = \frac{x^{-a}\gamma(a,x) {\Gamma(a)}\f$, 
     *  where \f$ \gamma(a,x) = \int_0^x e^{-t}t^{a-1}dt\f$, 
     *  Abramowitz & Stegun, 6.5.4
     *  @param a INPUT a-parameter 
     *  @param x INPUT x-argument 
     *  @return the value of regularized incomplete gamma function 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-03-27
     */
    GAUDI_API double gamma_star ( const double a , const double x ) ;
    // ========================================================================
    /** regularized incomplete gamma function 
     *  \f$ \gamma^{\ast}(n,x) = \frac{x^{-n}}{\Gamma(n)\gamma(n,x) }\f$, 
     *  where 
     *  \f$ \gamma(n,x) = \Gamma(n) - \Gamma(n,x) = \int_0^x e^{-t}t^{n-1}dt\f$, 
     *  Abramowitz & Stegun, 6.5.4
     *  @param n INPUT n-parameter 
     *  @param x INPUT x-argument 
     *  @return the value of regularized incomplete gamma function 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-03-27
     */
    GAUDI_API double gamma_star ( const int n , const double x ) ;
    // ========================================================================
    /** alpha_n 
     *  \f$\alpha_n(x) = \int_1^\inf t^n e^{-tx}dt \f$ for $x>0$
     *  Abramowitz & Stegun, 5.1.5
     *  @param n INPUT n-parameter 
     *  @param x INPUT x-argument 
     *  @return the function value 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-03-27
     */
    GAUDI_API double alpha_N ( const unsigned short n , const double x ) ;
    // ========================================================================
    /** complementary function to alpha_n 
     *  \f$\alpha^{\prime}_n(x) = \int_0^1 t^n e^{-tx}dt \f$
     *  @param n INPUT n-parameter 
     *  @param x INPUT x-argument 
     *  @return the function value 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-03-27
     */
    GAUDI_API double alpha_prime_N ( const unsigned short n , const double x ) ;
    // ========================================================================
    /** beta_n 
     *  \f$\beta_n(x) = \int_{-1}^{+1} t^n e^{-tx}dt \f$
     *  Abramowitz & Stegun, 5.1.6
     *  @param n INPUT n-parameter 
     *  @param x INPUT x-argument 
     *  @return the function value 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-03-27
     */
    GAUDI_API double beta_N ( const unsigned short n , const double x ) ;
    // ========================================================================
    /** confluent hypergeometrical function  1F1 aka Kummer's function
     *  \f$ f(a,b,x) = \sum_i  \frac{(a,i)}{((b,i)}\frac{x^i}{i!}\$f 
     *  @param a INPUT a-parameter 
     *  @param b INPUT b-argument  (b>0)
     *  @param x argument
     *  @return value of Kummer function
     */
    GAUDI_API double kummer 
    ( const unsigned short a ,
      const unsigned short b , 
      const double         x ) ;
    // ========================================================================
    /** get quantile function for standard normal distribution
     *  @see http://en.wikipedia.org./wiki/Probit
     *  @param alpha argument    \f$  0<\alpha<1 \f$  
     *  @return quantile value 
     */
    GAUDI_API double  probit ( const double alpha  ) ;
    // ========================================================================
    /** scaled complementary error function 
     *  \f$ 1 -  erf (x) = e^{-x^2} erfcx(x)  \f$ 
     *  @param x  the argument 
     *  @return the value of the scaled complementary error function 
     *  @attention  overflow happens for x<-26.6
     *  The actual implementation is copied from http://ab-initio.mit.edu/Faddeeva
     *  @see http://ab-initio.mit.edu/Faddeeva
     *  @see https://en.wikipedia.org/wiki/Error_function
     *  @see https://en.wikipedia.org/wiki/Faddeeva_function
     */
    GAUDI_API double  erfcx ( const double x ) ;
    // ========================================================================
    /** get the gaussian integral
     *  \f[ f = \int_a^b \exp { -\alpha^2 x^2 + \beta x } \mathrm{d}x \f]
     *  @param alpha the alpha parameter
     *  @param beta  the beta  parameter
     *  @param low   the low  integration limit
     *  @param high  the high integration limit
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-05-23
     */
    GAUDI_API double gaussian_integral
    ( const double alpha ,
      const double beta  ,
      const double low   ,
      const double high  ) ;
    // ========================================================================
      
    // ========================================================================
    // clenshaw summation algorithms 
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
     *  \f$ f(x) = \sum_i a_i x^{n-i} \f$, such as \f$f(0)= a_n\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double horner_aN 
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // ========================================================================
    /** Clenshaw algorithm for summation of cosine-series 
     *  \f$ f(x) = \frac{a_0}{2} + \sum_{i=k}^{n} a_k \cos( k x) \f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double clenshaw_cosine
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // ========================================================================
    /** Clenshaw algorithm for summation of sine-series 
     *  \f$ f(x) = \sum_{i=k}^{n} a_k \sin( k x) \f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double clenshaw_sine
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // ========================================================================
    /** Clenshaw algorithm for summation of Fourier-series 
     *  \f$ f(x) = \frac{a_0}{2} + \sum_{i=k}^{n} a_{2k-1}\sin(kx)+a_{2k}\cos(kx) \f$
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double clenshaw_fourier 
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // ========================================================================
    /** Clenshaw algorithm for summation of Hermite polynomials 
     *  \f$ f(x) = \sum_i p_i He_i(x) \f$
     *  @attention here we consider "probabilistic" polynomials
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2015-02-10
     */
    GAUDI_API double clenshaw_hermite
    ( const std::vector<double>& pars , 
      const double               x    ) ;
    // =========================================================================

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
     *  \f$f(x) = [b_0+]  \frac{a_1}{ b_1 + \frac{a_2}{ b_2 + ...}} \f$
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
