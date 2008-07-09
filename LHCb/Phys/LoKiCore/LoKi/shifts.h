// $Id: shifts.h,v 1.2 2008-07-09 16:01:00 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_SHIFTS_H 
#define LOKI_SHIFTS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// Boost
// ============================================================================
#include "boost/integer_traits.hpp"
// ============================================================================
namespace LoKi
{
  /** @namespace LoKi::Shifts 
   *  Collectoin of utilities and function for "shifting" operations
   *  Many thanks to Kurt RINNER for the code fragment!
   *  @author Vanya BEYAEV ibelyaev@physics.syr.edu
   *  @date 2007-07-20
   */   
  namespace Shifts
  {
    // ========================================================================
    /// the generic declaration of non-existing structure
    template <class TYPE,bool> struct CyclicShiftLeft  ;
    // ========================================================================
    /// the generic definition of non-existing structure
    template <class TYPE,bool> struct CyclicShiftRight ;
    // ========================================================================
    /** @struct CyclisShiftLeft 
     *  Simple helper structure to allow the LEFT cyclic shift of bits for
     *  ingeger types.
     *  Many thanks to Kurt RINNER for the code fragment!
     *  @author Vanya BEYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    template <class TYPE>
    struct CyclicShiftLeft<TYPE,true> 
      : public std::binary_function<TYPE,unsigned int,TYPE>
    {
      enum { bSize 
             = boost::integer_traits<TYPE>::digits 
             + boost::integer_traits<TYPE>::is_signed } ;
      inline TYPE operator () ( TYPE input , unsigned int N ) const 
      {
        N = N%bSize ;
        return ( input << N ) | ( input >> ( bSize - N ) ) ;  
      }
    } ;
    // ========================================================================
    /** @struct CyclisShiftRight
     *  Simple helper structure to allow the RIGHT cyclic shift of bits for
     *  ingeger types.
     *  Many thanks to Kurt RINNER for the code fragment!
     *  @author Vanya BEYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */
    template <class TYPE>
    struct CyclicShiftRight<TYPE,true> 
      : public std::binary_function<TYPE,unsigned int,TYPE>
    {
      enum { bSize 
             = boost::integer_traits<TYPE>::digits 
             + boost::integer_traits<TYPE>::is_signed } ;
      inline TYPE operator() ( TYPE input , unsigned int N ) const 
      {
        N = N%bSize ;
        return ( input >> N ) | ( input << ( bSize - N ) ) ;  
      }
    } ;
    // ========================================================================
    /** helper function for LEFT cycling shifts.
     *
     *  @code
     * 
     *   int word = ... ;
     *
     *   word = cyclicShiftLeft ( word , 5 ) ;
     *
     *  @endcode
     * 
     *  Many thanks to Kurt RINNER for the code fragment!
     *
     *  @param input input word to be shifter
     *  @param N     number of shifting bits 
     *  @author Vanya BEYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */        
    template <class TYPE>
    inline TYPE 
    cyclicShiftLeft
    ( TYPE input , unsigned int N ) 
    {
      CyclicShiftLeft<TYPE,boost::integer_traits<TYPE>::is_integral> shift ;
      return shift ( input , N ) ;  
    }  
    // ========================================================================
    /** helper function for RIGHT cycling shifts.
     *
     *  @code
     * 
     *   int word = ... ;
     *
     *   word = cyclicShiftRight ( word , 5 ) ;
     *
     *  @endcode
     * 
     *  Many thanks to Kurt RINNER for the code fragment!
     *
     *  @param input input word to be shifter
     *  @param N     number of shifting bits 
     *  @author Vanya BEYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */        
    template <class TYPE>
    inline TYPE 
    cyclicShiftRight
    ( TYPE input , unsigned int N ) 
    {
      CyclicShiftRight<TYPE,boost::integer_traits<TYPE>::is_integral> shift ;
      return shift ( input , N ) ;  
    }
    // ========================================================================
    /** helper function for cycling shifts 
     *     - LEFT shifts for positive arguments 
     *     - RIGHT shifts for negative arguments
     *
     *  @code
     * 
     *   int word = ... ;
     *
     *   word = cyclicShift ( word , 5 ) ;
     *
     *  @endcode
     * 
     *  Many thanks to Kurt RINNER for the code fragment!
     *
     *  @param input input word to be shifter
     *  @param N     number of shifting bits 
     *  @author Vanya BEYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-20
     */        
    template <class TYPE>
    inline TYPE
    cyclicShift
    ( TYPE input , int N ) 
    {
      return 
        ( 0 == N ) ? input : 
        ( 0 <  N ) ?
        cyclicShiftLeft  ( input ,  N ) : cyclicShiftRight ( input , -N ) ;
    }  
    // ========================================================================
  } // end of the namespace LoKi::Shifts
} // end of the namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_SHIFTS_H
// ============================================================================
