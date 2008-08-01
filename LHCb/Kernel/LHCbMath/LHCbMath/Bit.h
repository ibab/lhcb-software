// $Id: Bit.h,v 1.2 2008-08-01 10:16:02 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_BIT_H 
#define LHCBMATH_BIT_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/TypeWrapper.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/integer.hpp"
#include "boost/integer_traits.hpp"
#include "boost/static_assert.hpp"
#include "boost/mpl/if.hpp"
// ============================================================================
/** @file
 *
 *  The collection of utilities for evaluation of Nth bit for 
 *  the unsigned integral values 
 *
 *   -  Gaudi::Math::IBit 
 *      The most efficient compile-time evaluator.
 *      It is applicable if both N and value are compile-time constanst 
 *   -  Gaudi::Math::Bit
 *      Rather efficient fuctor, N is compile-time constanst 
 *   -  Gaudi::Math::bit
 *      The regular function, the least efficient evaluator 
 *  
 *  @attention the least significant bit is numbered as #0
 *
 *  @see Gaudi::Math::IBit
 *  @see Gaudi::Math::Bit 
 *  @see Gaudi::Math::bit
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
      /** @struct Check 
       *  Simple structure to check the if the type has sufficient 
       *  length to address Nth bit 
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-08-01
       */
      template <class TYPE,unsigned int N>
      struct Check 
      {
        // ====================================================================
        BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized
                             && boost::integer_traits<TYPE>::is_integral 
                             &&!boost::integer_traits<TYPE>::is_signed ) ;      
        // ====================================================================
        enum { value =  N < (unsigned int) boost::integer_traits <TYPE>::digits } ;
        // ====================================================================
      };
      // ======================================================================
      /** @struct _IBit 
       *  Simple helper structure to extract Nth bit of the integral type 
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-08-01
       */
      template <class TYPE, 
                typename Gaudi::Math::TypeWrapper<TYPE>::value_type I, 
                unsigned int N>
      struct _IBit
      {
      private:
        // ====================================================================
        BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized 
                             &&!boost::integer_traits<TYPE>::is_signed   ) ;
        // ====================================================================
      public:
        // ====================================================================
        enum { value = 0 < ( I & ( static_cast<TYPE>(1) << N ) ) } ;
        // ====================================================================
      };
      // ======================================================================
      /** @struct _Bit 
       *  Simple helper structure to extract Nth bit of the integral type 
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-08-01
       */
      template <class TYPE, unsigned int N>
      struct _Bit : public std::unary_function<TYPE,bool>
      {
      private:
        // ====================================================================
        BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized
                             && boost::integer_traits<TYPE>::is_integral 
                             &&!boost::integer_traits<TYPE>::is_signed ) ;
        // ====================================================================
        enum { OK = Check<TYPE,N>::value } ;
        // ====================================================================
      public:
        // ====================================================================
        inline bool operator() ( const TYPE value ) const
        {
          static const TYPE s_one = OK ? ( static_cast<TYPE> ( 1 ) << N ) : 0 ;
          return OK ? ( value & s_one ) : 0 ;
        } 
        // ====================================================================
      };
      // ======================================================================
    }
    // ========================================================================
    /** @struct IBit 
     *  (Compile-time) evaluation of the Nth bit for unsigned integral I:
     *
     *  @code 
     * 
     *  const bool bit0 = Gaudi::Math::IBit<111,0>::value ;
     *  const bool bit1 = Gaudi::Math::IBit<111,1>::value ;
     *  const bool bit2 = Gaudi::Math::IBit<111,2>::value ;
     *  const bool bit3 = Gaudi::Math::IBit<111,3>::value ;
     *
     *  @code 
     *
     *  @attention the least significant bit is numbered as bit#0 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-09
     */
    template <class TYPE, 
              typename Gaudi::Math::TypeWrapper<TYPE>::value_type I, 
              unsigned int  N>
    struct IBit 
      : public boost::mpl::if_c< detail::Check<TYPE,N>::value ,
                                 detail::_IBit<TYPE,I,N>      ,
                                 detail::Null                 >::type
    {
      // ======================================================================
      BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized
                           && boost::integer_traits<TYPE>::is_integral 
                           &&!boost::integer_traits<TYPE>::is_signed   ) ;
      // ======================================================================
    };
    // ========================================================================
    /** @struct Bit
     *  Simple strcuture which evaluates N-th bit for the 
     *  integral value, where N is compile-time constant  
     *     
     *  
     *  @code
     * 
     *  TYPE value = ... ;
     *
     *  Gaudi::Math::Bit<TYPE,10> bit ; 
     *  const bool bit10 = bit ( value ) ; // check bit #10 
     *
     *  @code 
     *
     *  @attention the least significant bit is numbered as bit#0 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-09
     */
    template <class TYPE, unsigned int N>
    struct Bit : public boost::mpl::if_c< detail::Check<TYPE,N>::value ,
                                          detail::_Bit<TYPE,N>         ,
                                          detail::Null_<TYPE>          >::type
    {
      // ======================================================================
      BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized
                           && boost::integer_traits<TYPE>::is_integral 
                           &&!boost::integer_traits<TYPE>::is_signed   ) ;
      // ======================================================================
    };
    // ========================================================================
    /** simple function which evaluates N-th bit for the 
     *  integral value 
     *
     *  @code
     * 
     *  TYPE value = ... ;
     * 
     *  const bool bit10 = Gaudi::Math::bit ( value , 10 ) ; // check bit #10 
     *
     *  @code 
     *
     *  @attention the least significant bit is numbered as bit#0 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-09
     */
    template <class TYPE>
    inline bool bit ( const TYPE value , const unsigned int N  ) 
    {
      // ======================================================================
      BOOST_STATIC_ASSERT( boost::integer_traits<TYPE>::is_specialized
                           && boost::integer_traits<TYPE>::is_integral 
                           &&!boost::integer_traits<TYPE>::is_signed ) ;
      // ======================================================================
      static const TYPE one = static_cast<TYPE>(1) ;
      return N < (unsigned int) boost::integer_traits <TYPE>::digits ? 
        ( value & ( one << N ) ) : 0 ;
      // ======================================================================
    }  
    // ========================================================================
  } // end of namespace Gaudi::Math
  // ==========================================================================
} // end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_DIGIT_H
// ============================================================================
