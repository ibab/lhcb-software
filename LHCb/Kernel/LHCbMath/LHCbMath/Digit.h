// $Id: Digit.h,v 1.1 2008-07-09 17:19:49 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_DIGIT_H 
#define LHCBMATH_DIGIT_H 1
// ============================================================================
// Include files
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Power.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/integer_traits.hpp"
#include "boost/static_assert.hpp"
// ============================================================================
namespace Gaudi
{
  namespace Math 
  {
    // ========================================================================
    /** simple function which evaluate N-th decimal digit for the 
     *  integral value 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @daet 2008-07-09
     */
    template <class TYPE>
    inline TYPE digit ( const TYPE value , const unsigned int N  ) 
    {
      BOOST_STATIC_ASSERT(boost::integer_traits<TYPE>::is_integral);
      if ( !N ) { return 0 ; }
      const TYPE ten = 10 ;
      const TYPE aux = Gaudi::Math::pow ( ten , N-1 ) ;
      return (value/aux)%10 ;
    }  
    // ========================================================================
  } // end of namespace Gaudi::Math
} // end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_DIGIT_H
// ============================================================================
