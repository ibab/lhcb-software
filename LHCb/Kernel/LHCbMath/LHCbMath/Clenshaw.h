// $Id:$ 
// ============================================================================
#ifndef LHCBMATH_CLENSHAW_H 
#define LHCBMATH_CLENSHAW_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <cmath>
#include <utility>
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Math
  {
    // ========================================================================
    /** Namespace with collection of Clenshaw summation algorithms 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2015-08-06
     *  Version           $Revision:$
     *  Last modification $Date:$
     *                 by $Author:$
     */
    namespace Clenshaw
    {
      // ======================================================================
      /** Clenshaw algorithm for summation of monomial series (aka "Horner's rule")
       *  \f$  f_1(x) = \sum_{i=0}^{n} a_i x^i     \f$ and 
       *  \f$  f_2(x) = \sum_{i=0}^{n} a_i x^{n-1} \f$
       *
       *  @see https://en.wikipedia.org/wiki/Clenshaw_algorithm
       *  @see https://en.wikipedia.org/wiki/Horner%27s_method
       *
       *  @code
       *  VECTOR_LIKE_TYPE a { { 1 , 2 , 3 } } ;
       *  double x     = 0 ;
       *  
       *  // f1 = a[0]*x*x +a[1]*x + a[2] 
       *  double f1 = monomial_sum ( a.begin()  , a.end()  , x ).first ;
       *  
       *  // f2 = a[0]+a[1]*x+a[2]*x*x 
       *  double f2 = monomial_sum ( a.rbegin() , a.rend() , x ).first ;
       *
       *  @endcode 
       *
       *  With this algorithm one also gets a first derivative:
       *
       *  @code
       *  VECTOR_LIKE_TYPE a { { 1 , 2 , 3 } } ;
       *  double x     = 0 ;
       *  
       *  // f1 = a[0]*x*x +a[1]*x + a[2] 
       *  std::pair<double,double> r  = monomial_sum ( a.begin()  , a.end()  , x ) ;
       *  double f1 = r.first  ; // polynomial
       *  double d1 = r.second ; // derivative 
       *
       *  @endcode        
       *  
       *  @param first iterator for start of the sequence 
       *  @param last  iterator for end   of the sequence 
       *  @param x    x-value 
       *  @return value of polynom and the first derivative at point x 
       *
       *  @see Gaudi::Math::clenshaw_polynom
       *  @see Gaudi::Math::horner_a0
       *  @see Gaudi::Math::horner_aN
       *
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2015-02-10
       */
      template <class ITERATOR>
      inline 
      std::pair<long double,long double>
      monomial_sum 
      ( ITERATOR          first  , 
        ITERATOR          last   , 
        const long double x      ) 
      {
        if ( first == last ) { return std::make_pair(0,0) ; }
        //
        long double p = *first ;
        long double q = 0      ;
        while ( ++first != last ) 
        {
          q = std::fma ( x , q ,  p     ) ; // x * q + p       ;
          p = std::fma ( x , p , *first ) ; // x * p + *first  ;
        }
        //
        return std::make_pair ( p , q ) ;
      }
      // ======================================================================
      /** Clenshaw algorithm for summation of Legendre series 
       *  \f$ f(x) = \sum_{i=0}^{n} a_i L_i(x) \f$
       *  @see https://en.wikipedia.org/wiki/Clenshaw_algorithm
       *  @see Gaudi::Math::LegendreSum 
       *  @see Gaudi::Math::clenshaw_legendre
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2015-02-10
       */
      template <class ITERATOR>
      inline 
      long double 
      legendre_sum 
      ( ITERATOR           first , 
        ITERATOR           last  ,
        const long double  x     ) 
      {
        // 
        if ( first == last ) { return 0 ; }
        //
        long double b2 = 0 ;
        long double b1 = 0 ;
        long double b0 = 0 ;
        while ( first != last ) 
        {
          --last ;
          const int j          =  last - first ;
          const long double pj = *last ;
          //
          b2 = b1 ;
          b1 = b0 ;
          b0 = pj + ( 2 * j + 1 ) * x * b1 / ( j + 1 ) - ( j + 1 ) *  b2 / ( j + 2 ) ;
        }
        //
        return b0 ;
      }
      // ======================================================================
      /** Clenshaw algorithm for summation of Chebyshev series 
       *  \f$ f(x) = \sum_{i=0}^{n} a_i C_i(x) \f$
       *  @see https://en.wikipedia.org/wiki/Clenshaw_algorithm
       *  @see Gaudi::Math::ChebyshevSum 
       *  @see Gaudi::Math::clenshaw_chebyshev
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2015-02-10
       */
      template <class ITERATOR>
      inline 
      long double
      chebyshev_sum 
      ( ITERATOR           first , 
        ITERATOR           last  ,
        const long double  x     ) 
      {
        if ( first == last ) { return 0 ; }
        //
        long double b2 = 0 ;
        long double b1 = 0 ;
        long double b0 = 0 ;
        while ( first != last ) 
        {
          --last  ;
          b2 = b1 ;
          b1 = b0 ;
          // b0 = (*ia) + 2 * x * b1 - b2 ;
          b0 = std::fma ( 2 * x , b1 , (*last) - b2 ) ;
        }
        //
        b0 += *first ;
        //
        return 0.5 * ( b0 - b2) ;
      }
      // ======================================================================      
    } //                             The end of namespace Gaudi::Math::Clenshaw     
    // ========================================================================
  } //                                         The end of namespace Gaudi::Math
  // ==========================================================================
} //                                                 The end of namespace Gaudi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LHCBMATH_CLENSHAW_H
// ============================================================================
