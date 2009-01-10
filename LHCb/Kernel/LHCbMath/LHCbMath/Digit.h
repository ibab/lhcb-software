// $Id: Digit.h,v 1.4 2009-01-10 12:58:54 ibelyaev Exp $
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
#include "boost/integer_traits.hpp"
#include "boost/static_assert.hpp"
// ============================================================================
/** @file
 *
 *  The collection of useful utilities for evaluation of 
 *  decimal digits for the unsigned integral values 
 *
 *  The utilities for evaluation of 
 *  Nth decimal digit for the unsigned integral values:
 *
 *   -  Gaudi::Math::IDigit 
 *      The most efficient compile-time evaluator.  
 *      It is applicable if both N and value are compile-time constanst 
 *   -  Gaudi::Math::Digit 
 *      Rather efficient fuctor, N is compile-time constanst 
 *   -  Gaudi::Math::digit
 *      The regular function, the least efficient evaluator 
 *  
 *  The utilities for evaluation of 
 *  decimal N1->N2 decimal digits for the unsigned integral values:
 *
 *   -  Gaudi::Math::IDigits 
 *      The most efficient compile-time evaluator.  
 *      It is applicable if both N1/N2 and value are compile-time constanst 
 *   -  Gaudi::Math::Digits 
 *      Rather efficient fuctor, N1&N2 are compile-time constanst 
 *   -  Gaudi::Math::digits
 *      The regular function, the least efficient evaluator 
 *
 *  @attention the least significant decimal digit is numbered as #0
 *
 *  @see Gaudi::Math::IDigit 
 *  @see Gaudi::Math::Digit 
 *  @see Gaudi::Math::digit
 *
 *  @see Gaudi::Math::IDigits 
 *  @see Gaudi::Math::Digits 
 *  @see Gaudi::Math::digits
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
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_specialized ) ;
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_integral    ) ;
        BOOST_STATIC_ASSERT( !boost::integer_traits<TYPE>::is_signed      ) ;
        // ====================================================================
        enum
          {
            /// Nth digit is "save"
            safe  = ( N <  (unsigned int) boost::integer_traits<TYPE>::digits10 ) ,
            /// Nth digit is still OK 
            value = ( N <= (unsigned int) boost::integer_traits<TYPE>::digits10 )
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
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_specialized ) ;
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_integral    ) ;
        BOOST_STATIC_ASSERT( !boost::integer_traits<TYPE>::is_signed      ) ;
        // ====================================================================
        enum 
          { 
            value = 
            Check10<TYPE,N>::safe ? 
            (I/Gaudi::Math::IPower<TYPE              ,10,N>::value )%10 :
            (I/Gaudi::Math::IPower<unsigned long long,10,N>::value )%10 
          } ;
      private:
        // ======================================================================
        enum {
          _imax10 = boost::integer_traits<TYPE>::digits10 ,
          check   = N <= _imax10 
        } ;
        // ======================================================================  
        BOOST_STATIC_ASSERT( check ) ;
        // ======================================================================
      } ;
      // ======================================================================
      /** @struct _IDigits 
       *  Helper structure for compile-time evaluation of 
       *  range N1->N2 of decimal digits from the integral type 
       *
       *  @attention the least significat decimal digit is numbered as #0
       *
       *  @author Vanya BELYAEV Ivan.Belyaev@nilkhef.nl
       *  @date 2008-07-31
       */
      template <class TYPE,
                typename TypeWrapper<TYPE>::value_type I , 
                unsigned int N1 ,
                unsigned int N2 >  
      struct _IDigits
      {
        // ====================================================================
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_specialized ) ;
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_integral    ) ;
        BOOST_STATIC_ASSERT( !boost::integer_traits<TYPE>::is_signed      ) ;
        // ====================================================================
        enum 
          { 
            value = 
            Check10<TYPE,N1>::safe && Check10<TYPE,N2-N1>::safe     ?
            (I/Gaudi::Math::IPower<TYPE              ,10,N1>::value )%
            Gaudi::Math::IPower<TYPE,10,N2-N1>::value :
            (I/Gaudi::Math::IPower<unsigned long long,10,N1>::value )%
            Gaudi::Math::IPower<unsigned long long,10,N2-N1>::value 
          };
        // ====================================================================
      private:
        // ====================================================================
        enum {
          _imax10 = boost::integer_traits<TYPE>::digits10 ,
          check1  = N1 < N2 ,
          check2  = N1 <= _imax10 ,
          check3  = N2 <= _imax10 + 1 
        } ;
        // ====================================================================
        BOOST_STATIC_ASSERT( check1 ) ;
        BOOST_STATIC_ASSERT( check2 ) ;
        BOOST_STATIC_ASSERT( check3 ) ;
        // ====================================================================`
      } ;
      // ======================================================================
      /** @struct _Digit 
       *  Helper structure to geth Nth decimal digits from the integral type 
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
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_specialized ) ;
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_integral    ) ;
        BOOST_STATIC_ASSERT( !boost::integer_traits<TYPE>::is_signed      ) ;
        // ====================================================================
        enum { value = Gaudi::Math::IPower<unsigned long long,10,N>::value } ;
        // ====================================================================
        inline int operator () ( const TYPE v ) const { return (v/value)%10 ; }
        // ====================================================================
      private:
        // ====================================================================
        enum {
          _imax10 = boost::integer_traits<TYPE>::digits10 
        } ;
        // ====================================================================
        BOOST_STATIC_ASSERT( N <= _imax10 ) ;
        // ====================================================================
      } ;
      // ======================================================================
      template <class TYPE, unsigned int N, bool OK>
      struct __Dig10 
      { enum { value = Gaudi::Math::IPower<TYPE,10,N>::value } ; } ;
      // ======================================================================
      template <class TYPE, unsigned int N>
      struct __Dig10<TYPE,N,false> 
      { enum { value = Gaudi::Math::IPower<unsigned long long,10,N>::value } ; } ;
      // ======================================================================
      /** @struct _Digits 
       *  Helper structure for evaluation of 
       *  range N1->N2 of decimal digits from the integral type 
       *
       *  @attention the least significat decimal digit is numbered as #0
       *
       *  @author Vanya BELYAEV Ivan.Belyaev@nilkhef.nl
       *  @date 2008-07-31
       */
      template <class TYPE, 
                unsigned int N1,
                unsigned int N2>
      struct _Digits : public std::unary_function<TYPE,int>
      {
      private:
        // ====================================================================
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_specialized ) ;
        BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_integral    ) ;
        BOOST_STATIC_ASSERT( !boost::integer_traits<TYPE>::is_signed      ) ;
        // ====================================================================
        enum
          {
            val1 = __Dig10<TYPE,N1,
            Check10<TYPE,N1>::safe && Check10<TYPE,N2-N1>::safe>:: value ,
            val2 = __Dig10<TYPE,N2-N1,
            Check10<TYPE,N1>::safe && Check10<TYPE,N2-N1>::safe>:: value 
          } ;
        // ====================================================================        
      public:
        // ====================================================================
        /// the only on eessential method 
        inline int operator() ( const TYPE v ) const { return (v/val1)%val2 ; }
        // ====================================================================
      private:
        // ====================================================================
        enum {
          _imax10 = boost::integer_traits<TYPE>::digits10 
        } ;
        // ====================================================================
        BOOST_STATIC_ASSERT( N1 < N2 ) ;
        BOOST_STATIC_ASSERT( N1 <= _imax10     ) ;
        BOOST_STATIC_ASSERT( N2 <= _imax10 + 1 ) ;
        // ====================================================================
      };
    } // end of namespace detail 
    // ========================================================================
    /** @struct IDigit
     *  Simple structure form compile-time evaluation 
     *  of the Nth decimal digit for the given number 
     
     *  @code
     * 
     *  const int dig5 = IDigit<unsigned int,4362736,5>::value ;
     * 
     *  @endcode 
     *
     *  @attention the least significant decimal digit is numbered as #0
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nilkhef.nl
     *  @date 2008-07-31
     */ 
    // ========================================================================
    template <class TYPE, 
              typename Gaudi::Math::TypeWrapper<TYPE>::value_type I , 
              unsigned int N>
    struct IDigit : public detail::_IDigit<TYPE,I,N>
    {
      // ======================================================================
      BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_specialized ) ;
      BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_integral    ) ;
      BOOST_STATIC_ASSERT( !boost::integer_traits<TYPE>::is_signed      ) ;
      // ======================================================================
    };
    // ========================================================================
    /** @struct IDigits 
     *  Simple structr efor compile-time evaluation of 
     *  the range of decimal digits N1->N2 (N2 is excluded) 
     *  for the given integral type 
     *  
     *  @code 
     *
     *  const int dig02 = IDigits<unsigned int,4362736,0,2>::value ;
     *
     *  @endcode 
     *
     *  @attention the least significant decimal digit is numbered as #0
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nilkhef.nl
     *  @date 2008-07-31
     */  
    template <class TYPE, 
              typename Gaudi::Math::TypeWrapper<TYPE>::value_type I , 
              unsigned int N1 , 
              unsigned int N2 >
    struct IDigits : public detail::_IDigits<TYPE,I,N1,N2>     
    {
      // ======================================================================
      BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_specialized ) ;
      BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_integral    ) ;
      BOOST_STATIC_ASSERT( !boost::integer_traits<TYPE>::is_signed      ) ;
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
    struct Digit : public detail::_Digit<TYPE,N>
    {
      // ======================================================================
      BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_specialized ) ;
      BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_integral    ) ;
      BOOST_STATIC_ASSERT( !boost::integer_traits<TYPE>::is_signed      ) ;
      // ======================================================================
    };
    // ========================================================================
    /** @struct Digits 
     *  simple structure for evaluation of 
     *  range of decomal digits N1->N2 (N2 is excluded) for the integral type 
     * 
     *  @code
     *   
     *   const unsigned int value  = ... ;
     * 
     *   Digits<unsigned int,0,5> digits05 ;
     *  
     *   const int dig05 = digit05 ( value ) ;
     *
     *  @endcode 
     *
     *  @attention the least significat decimal digit is numbered as #0
     *
     *  @author Vanya BELYAEV Ivan/BElyaev@nikhef.nl
     *  @date 2008-07-31 
     */
    template <class        TYPE , 
              unsigned int N1   , 
              unsigned int N2   >
    struct Digits : public detail::_Digits<TYPE,N1,N2>
    {
      // ======================================================================
      BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_specialized ) ;
      BOOST_STATIC_ASSERT(  boost::integer_traits<TYPE>::is_integral    ) ;
      BOOST_STATIC_ASSERT( !boost::integer_traits<TYPE>::is_signed      ) ;
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
     *   const int digN = Gaudi::Math::digit ( value , N ) ;
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
      BOOST_STATIC_ASSERT (  boost::integer_traits<TYPE>::is_specialized ) ;
      BOOST_STATIC_ASSERT (  boost::integer_traits<TYPE>::is_integral    ) ;
      BOOST_STATIC_ASSERT ( !boost::integer_traits<TYPE>::is_signed      ) ;
      // ======================================================================
      if      (  N > (unsigned int) boost::integer_traits<TYPE>::digits10 ) { return 0 ; } // RETURN 
      else if (  N < (unsigned int) boost::integer_traits<TYPE>::digits10 ) 
      {
        // ====================================================================
        const TYPE ten = 10 ;
        const TYPE aux = Gaudi::Math::pow ( ten , N ) ;
        return (value/aux)%10 ;                                       // RETURN 
        // ====================================================================
      }
      // ======================================================================
      const unsigned long long val = value ;
      const unsigned long long ten = 10    ;
      const unsigned long long aux = Gaudi::Math::pow ( ten , N ) ;
      return (val/aux)%10 ;                                         // RETURN
      // ======================================================================
    }  
    // ========================================================================
    /** simple function which evaluate the range of decimal digits 
     *  N1-.N2 (N2 is excluded) for the integral values 
     *
     *  @code
     *
     *   unsigned int value = ... ;
     *   unsigned int N1 = ... ;
     *   unsigned int N2 = ... ;
     * 
     *   const bool digN1N2 = Gaudi::Math::digits( value , N1 , N2  ) ;
     *
     *  @endcode 
     * 
     *  @attention the least significat decimal digit is numbered as #0
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @daet 2008-07-09
     */
    template <class TYPE>
    inline TYPE digits ( const TYPE         value , 
                         const unsigned int N1    ,
                         const unsigned int N2    ) 
    {
      // ======================================================================
      BOOST_STATIC_ASSERT (  boost::integer_traits<TYPE>::is_specialized ) ;
      BOOST_STATIC_ASSERT (  boost::integer_traits<TYPE>::is_integral    ) ;
      BOOST_STATIC_ASSERT ( !boost::integer_traits<TYPE>::is_signed      ) ;
      // ======================================================================
      if      (  N2 >  1 + boost::integer_traits<TYPE>::digits10 )
      { return digits ( value , N1 , 1 + boost::integer_traits<TYPE>::digits10 ) ; }
      // ======================================================================
      if      (  N1 >= N2 || 
                 N1 > (unsigned int) boost::integer_traits<TYPE>::digits10 )
      { return 0 ; }                                                  // RETURN 
      //
      if ( N1      < (unsigned int) boost::integer_traits<TYPE>::digits10 && 
           N2 - N1 < (unsigned int) boost::integer_traits<TYPE>::digits10 ) 
      {
        // ====================================================================
        const TYPE ten = 10 ;
        const TYPE aux1 = Gaudi::Math::pow ( ten ,      N1 ) ;
        const TYPE aux2 = Gaudi::Math::pow ( ten , N2 - N1 ) ;
        return (value/aux1)%aux2 ;                                    // RETURN 
        // ====================================================================
      }
      // ======================================================================
      const unsigned long long val  = value ;
      const unsigned long long ten  = 10 ;
      const unsigned long long aux1 = Gaudi::Math::pow ( ten , N1      ) ;
      const unsigned long long aux2 = Gaudi::Math::pow ( ten , N2 - N1 ) ;
      return (val/aux1)%aux2 ;                                        // RETURN
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
