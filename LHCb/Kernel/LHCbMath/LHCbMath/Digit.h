// $Id: Digit.h,v 1.2 2008-08-01 10:16:02 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_DIGIT_H 
#define LHCBMATH_DIGIT_H 1
// ============================================================================
// Include files
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/TypeWrapper.h"
#include "LHCbMath/IPower.hpp"
#include "LHCbMath/Power.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/mpl/if.hpp"
#include "boost/integer_traits.hpp"
#include "boost/static_assert.hpp"
// ============================================================================
/** @file
 *
 *  The collection of useful utilities for evaluation of 
 *  Nth decimal digit for the unsigned integral values 
 *
 *   -  Gaudi::Math::IDigit 
 *      The most efficient compile-time evaluator.  
 *      It is applicable if both N and value are compile-time constanst 
 *   -  Gaudi::Math::Digit 
 *      Rather efficient fuctor, N is compile-time constanst 
 *   -  Gaudi::Math::digit
 *      The regular function, the least efficient evaluator 
 *  
 *  @attention the least significant decimal digit is numbered as #0
 *
 *  @see Gaudi::Math::IDigit 
 *  @see Gaudi::Math::Digit 
 *  @see Gaudi::Math::digit
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-08-01
 */
// ============================================================================
namespace Gaudi
{
  namespace Math 
  {
    namespace detail 
    {
      // ======================================================================
      /** @struct Check10 
       *  Simple Helper structure to check if the type is able to 
       *  contain the certain number of decimal digits 
       *
       *   - "safe" 
       *   - "value"
       *
       *  @attention the least significat decimal digir is numbered as #0
       *
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-08-01
       */
      template <class TYPE,unsigned int N>
      struct Check10 
      {
        // ====================================================================
        BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized 
                             && boost::integer_traits<TYPE>::is_integral  
                             &&!boost::integer_traits<TYPE>::is_signed  );
        // ====================================================================
        enum
          {
            safe  = ( N < (unsigned int)     boost::integer_traits<TYPE>::digits10 ) ,
            value = ( N < (unsigned int) 2 + boost::integer_traits<TYPE>::digits10 )
          } ;
        // ====================================================================
      };
      // ======================================================================
      /** @struct _IDigit
       *  Simple structure form compile-time evaluation 
       *  of the decimal 
       *  digit for the given number 
       *
       *  @attention the least significat decimal digir is numbered as #0
       *
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-08-01
       */
      template <class TYPE,
                typename TypeWrapper<TYPE>::value_type I , 
                unsigned int N>
      struct _IDigit
      {
        // ====================================================================
        BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized 
                             && boost::integer_traits<TYPE>::is_integral  
                             &&!boost::integer_traits<TYPE>::is_signed  );
        // ====================================================================
        enum 
          { 
            value = 
            Check10<TYPE,N>::safe  ? 
            (I/Gaudi::Math::IPower<TYPE              ,10,N>::value )%10 :
            (I/Gaudi::Math::IPower<unsigned long long,10,N>::value )%10 
          } ;
      } ; 
      // ======================================================================
      /** @struct _Digit 
       *  Helper structire to geth nth decimal digits from the integral type 
       *
       *  @attention the least significat decimal digit is numbered as #0
       *
       *  @author Vanya BELYAEV Ivan.Belyaev@nilkhef.nl
       *  @date 2008-07-31
       */
      template <class TYPE, unsigned int N>
      struct _Digit : public std::unary_function<TYPE,int>
      {
        // ====================================================================
        BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized 
                             && boost::integer_traits<TYPE>::is_integral  
                             &&!boost::integer_traits<TYPE>::is_signed  );
        // ====================================================================
        enum { value = Gaudi::Math::IPower<unsigned long long,10,N>::value } ;
        // ====================================================================
        inline int operator () ( const TYPE v ) const { return (v/value)%10 ; }
        // ====================================================================
      } ;
    } // end of namespace detail 
    // ========================================================================
    /** @struct IDigit
     *  Simple structure form compile-time evaluation 
     *  of the decimal 
     *  digit for the given number 
     *  @code
     * 
     *  BOOST_STATIC_ASSERT ( 0 == IDigit<0,10>::value  ) ;
     *  BOOST_STATIC_ASSERT ( 1 == IDigit<1,10>::value  ) ;
     * 
     *  BOOST_STATIC_ASSERT ( 5 == IDigit<0,315>::value ) ;
     *  BOOST_STATIC_ASSERT ( 3 == IDigit<2,315>::value ) ;
     * 
     *  @endcode 
     *
     *  @attention the least significat decimal digit is numbered as #0
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nilkhef.nl
     *  @date 2008-07-31
     */ 
    // ========================================================================
    template <class TYPE, 
              typename Gaudi::Math::TypeWrapper<TYPE>::value_type I , 
              unsigned int N>
    struct IDigit : public boost::mpl::if_c<detail::Check10<TYPE,N>::value,
                                            detail::_IDigit<TYPE,I,N>     ,
                                            detail::Null                  >::type 
    {
      // ======================================================================
      BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized 
                           && boost::integer_traits<TYPE>::is_integral  
                           &&!boost::integer_traits<TYPE>::is_signed  );
      // ======================================================================
    };
    // ========================================================================
    /** @struct Digit 
     *  simple structure for evaluation of Nth digit for the integral type 
     * 
     *  @code
     *   
     *   const unsigned int value  = ... ;
     * 
     *   Digit<unsigned int,5> digit5 ;
     *  
     *   const int dig5 = digit5 ( value ) ;
     *
     *  @endcode 
     *
     *
     *  @attention the least significat decimal digit is numbered as #0
     *
     *  @author Vanya BELYAEV Ivan/BElyaev@nikhef.nl
     *  @date 2008-07-31 
     */
    template <class TYPE, unsigned int N>
    struct Digit : public boost::mpl::if_c<detail::Check10<TYPE,N>::value ,
                                           detail::_Digit<TYPE,N>         ,
                                           detail::Null_<TYPE,int>        >::type 
    {
      // ======================================================================
      BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized 
                           && boost::integer_traits<TYPE>::is_integral  
                           &&!boost::integer_traits<TYPE>::is_signed  );
      // ======================================================================
    };
    // ========================================================================
    /** simple function which evaluate N-th decimal digit for the 
     *  integral value 
     *
     *  @code
     *
     *   unsigned int value = ... ;
     *   unsigned int N     = ... ;
     * 
     *   const bool bitN = Gaudi::Math::bit ( value , N ) ;
     *
     *  @endcode 
     * 
     *  @attention the least significat decimal digit is numbered as #0
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @daet 2008-07-09
     */
    template <class TYPE>
    inline TYPE digit ( const TYPE value , const unsigned int N  ) 
    {
      // ======================================================================
      BOOST_STATIC_ASSERT ( boost::integer_traits<TYPE>::is_specialized
                            && boost::integer_traits<TYPE>::is_integral
                            &&!boost::integer_traits<TYPE>::is_signed  ) ;
      // ======================================================================
      if      (  N >= 2 +            boost::integer_traits<TYPE>::digits10 ) 
      { return 0 ; }                                                  // RETURN 
      else if (  N <  (unsigned int) boost::integer_traits<TYPE>::digits10 ) 
      {
        // ====================================================================
        const TYPE ten = 10 ;
        const TYPE aux = Gaudi::Math::pow ( ten , N ) ;
        return (value/aux)%10 ;                                       // RETURN 
        // ====================================================================
      }
      // ======================================================================
      const unsigned long long ten = 10 ;
      const unsigned long long aux = Gaudi::Math::pow ( ten , N ) ;
      return (value/aux)%10 ;                                         // RETURN
      // ======================================================================
    }  
    // ========================================================================
  } // end of namespace Gaudi::Math
} // end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_DIGIT_H
// ============================================================================
