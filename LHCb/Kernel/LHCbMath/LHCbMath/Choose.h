// $Id$
// ============================================================================ 
#ifndef LHCBMATH_CHOOSE_H 
#define LHCBMATH_CHOOSE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
namespace Gaudi
{
  namespace Math 
  {    
    // ========================================================================
    /** calculate the binomial coefficient C(k,n) = n!/((n-k)!*k!)
     *  the result is exact for n,k<63 
     *  @author Vanya BELYAEV Ivan.Belyaev@irep.ru
     *  @date 2015-03-08
     */
    GAUDI_API 
    unsigned long long  choose ( const unsigned short n ,
                                 const unsigned short k ) ;
    // ========================================================================
    /** calculate the binomial coefficient C(k,n) = n!/((n-k)!*k!)
     *  @author Vanya BELYAEV Ivan.Belyaev@irep.ru
     *  @date 2015-03-08
     */
    GAUDI_API 
    double choose_double       ( const unsigned short n , 
                                 const unsigned short k ) ;
    // ========================================================================
    /** calculate the generalized binomial coefficient C(a,k) 
     *  \f$C(\alpha,k) = \frac{\alpha}{k}\frac{\alpha-1}{k-1}...\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@irep.ru
     *  @date 2015-03-08
     */
    GAUDI_API 
    double gen_choose ( const double         a ,
                        const unsigned short k ) ;
    // ========================================================================
    /** calculate the generalized binomial coefficient C(n/2,k) 
     *  \f$C(n,k) = \frac{n/2}{k}\frac{n/2-1}{k-1}...\f$
     *  @author Vanya BELYAEV Ivan.Belyaev@irep.ru
     *  @date 2015-03-08
     */
    GAUDI_API 
    double choose_half ( const int            n ,
                         const unsigned short k ) ;
    // ========================================================================
  } //                                             end of namespace Gaudi::Math
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LHCBMATH_CHOOSE_H
// ============================================================================
