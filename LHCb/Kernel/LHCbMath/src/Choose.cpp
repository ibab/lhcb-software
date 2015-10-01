// $Id$ 
// ============================================================================
// Include files
// ============================================================================
// STD/STL 
// ============================================================================
#include <cmath>
#include <climits>
#include <algorithm>
// ============================================================================
// local
// ============================================================================
#include "LHCbMath/Choose.h"
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Power.h"
// ============================================================================
/** @file 
 *  Cacualte binbomial coefficinects 
 *  @author Vanya BELYAEV Ivan.Belyaev@irep.ru
 *  @date 2015-03-08
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  // calculate the binomial coefficient C(k,n) = n!/((n-k)!*k!)
  inline unsigned long long  
  _choose_ ( unsigned short n , unsigned short k ) 
  {
    //
    if      ( k > n            ) { return 0 ; }
    else if ( 0 == k || n == k ) { return 1 ; }
    //
    k = std::min ( k , (unsigned short) ( n - k ) ) ;
    unsigned long long r = 1  ;
    for ( unsigned short d = 1 ; d <= k  ; ++d ) 
    {
      r *= n-- ;
      r /= d   ; 
    }
    return r ;
  }
  // ==========================================================================
  typedef std::numeric_limits<unsigned long long> ULLTYPE ;
  static_assert( ULLTYPE::is_specialized , "Unsigned long long is not specialized!" ) ;
  const unsigned long long s_ullmax = ULLTYPE::max    ()          ;
  const long double        s_emax   = std::log( 0.2 * s_ullmax )  ;
  const unsigned short     s_digits = ULLTYPE::digits - 2         ;
  // ==========================================================================
  /// zero for doubles  
  LHCb::Math::Zero<double>     s_zero  ;       // zero for doubles
  // ==========================================================================
}
// ============================================================================
/* calculate the binomial coefficient C(k,n) = n!/((n-k)!*k!)
 * the result is exact for n,k<63 
 * @author Vanya BELYAEV Ivan.Belyaev@irep.ru
 * @date 2015-03-08
 */
// ============================================================================
unsigned long long  
Gaudi::Math::choose 
( const unsigned short n , 
  const unsigned short k ) 
{ return _choose_ ( n , k ) ; }
// ============================================================================
/** calculate the binomial coefficient C(k,n) = n!/((n-k)!*k!)
 *  @author Vanya BELYAEV Ivan.Belyaev@irep.ru
 *  @date 2015-03-08
 */
// ============================================================================
double Gaudi::Math::choose_double 
( const unsigned short n ,
  const unsigned short k ) 
{
  //
  if      ( k > n            ) { return 0 ; }
  else if ( 0 == k || n == k ) { return 1 ; }
  else if ( n < s_digits     ) { return _choose_ ( n , k ) ; }
  //
  long double a = std::lgamma ( (long double)     n + 1 )  ;
  if ( a < s_emax ) { return _choose_ ( n , k ) ; }
  a            -= std::lgamma ( (long double) n - k + 1 ) ;
  if ( a < s_emax ) { return _choose_ ( n , k ) ; }
  a            -= std::lgamma ( (long double)     k + 1 ) ;
  if ( a < s_emax ) { return _choose_ ( n , k ) ; }
  //
  return std::exp ( a ) ;
}
// ============================================================================
/*  calculate the generalized binomial coefficient C(a,n) 
 *  \f$C(\alpha,k) = \frac{\alpha}{k}\frac{\alpha-1}{k-1}...\f$
 *  @author Vanya BELYAEV Ivan.Belyaev@irep.ru
 *  @date 2015-03-08
 */
// ============================================================================
double Gaudi::Math::gen_choose 
( const double         a ,
  const unsigned short k ) 
{
  //
  if      ( 0 == k        ) { return 1 ; } 
  else if ( 1 == k        ) { return a ; }
  else if ( s_zero ( a )  ) { return 0 ; }
  //
  double r = 1 ;
  double b = a ;
  for ( unsigned short d = k ; 0 < d  ; --d ) 
  {
    // if ( s_zero( b ) ) { return 0 ; }  // RETURN  
    r *= b ;
    r /= d ; 
    b -= 1 ;
  }
  return r ;
}
// ============================================================================
/* calculate the generalized binomial coefficient C(n/2,k) 
 *  \f$C(n,k) = \frac{n/2}{k}\frac{n/2-1}{k-1}...\f$
 *  @author Vanya BELYAEV Ivan.Belyaev@irep.ru
 *  @date 2015-03-08
 */
// ============================================================================
double Gaudi::Math::choose_half 
( const int            n ,
  const unsigned short k ) 
{
  if      ( 0 == k              ) { return 1                    ; }
  else if ( 0 < n && 0 == n % 2 ) { return choose ( n * 2 , k ) ; }
  else if ( 1 == k              ) { return 0.5 * n              ; }  // attention! 
  else if ( 0 == n              ) { return 0                    ; } 
  //
  double r = 1 ;
  int    N = n ;
  for ( unsigned short d = k ; 0 < d  ; --d ) 
  {
    r *= N ;
    r /= d ; 
    N -= 2 ;  // ATTENTION 
  }
  return r / Gaudi::Math::pow ( 2 , k ) ;
}
// ============================================================================
 

// ============================================================================
// The END 
// ============================================================================
