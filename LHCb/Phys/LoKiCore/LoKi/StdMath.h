// $Id: StdMath.h,v 1.1 2007-08-18 14:10:56 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_STDMATH_H 
#define LOKI_STDMATH_H 1
// ============================================================================
// Include 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <functional>
// ============================================================================
// Boost
// ============================================================================
#include "boost/call_traits.hpp"
// ============================================================================
namespace LoKi 
{
  /** @namespace LoKi::StdMath
   *  collection of primitive math-classes and functions, missing in STD/STL
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-17
   */
  namespace StdMath
  {
    // ========================================================================
    /** @struct abs_less 
     *  comparison by absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE>
    struct abs_less : std::binary_function<TYPE,TYPE,TYPE>
    {
      inline TYPE operator() 
      ( typename boost::call_traits<const TYPE>::param_type v1 ,
        typename boost::call_traits<const TYPE>::param_type v2 ) const 
      {
        return m_eval ( std::fabs( v1 ) , std::fabs( v2 ) ) ; 
      }
      /// evaluator: 
      std::less<TYPE> m_eval ;
    } ;
    // ========================================================================
    /** @struct abs_greater
     *  comparison by absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE>
    struct abs_greater : std::binary_function<TYPE,TYPE,TYPE>
    {
      inline TYPE operator() 
      ( typename boost::call_traits<const TYPE>::param_type v1 ,
        typename boost::call_traits<const TYPE>::param_type v2 ) const 
      {
        return m_eval ( std::fabs( v1 ) , std::fabs( v2 ) ) ; 
      }
      /// evaluator: 
      std::greater<TYPE> m_eval ;
    } ;
    // ========================================================================
    /** return "min_by_abs"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */        
    template <class TYPE> 
    inline TYPE absMin ( TYPE v1 , TYPE v2 ) 
    { return std::min ( std::fabs ( v1 ) , std::fabs ( v2 ) ) ; }
    // ========================================================================
    /** return  "max_by_abs"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE> 
    inline TYPE absMax ( TYPE v1 , TYPE v2 ) 
    { return std::max ( std::fabs ( v1 ) , std::fabs ( v2 ) ) ; }
    // ========================================================================      
  } // end of namespace LoKi::StdMath
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_STDMATH_H
// ============================================================================
