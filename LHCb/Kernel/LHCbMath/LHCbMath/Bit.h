
// Include files
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
namespace Gaudi
{
  namespace Math 
  {
    // ========================================================================
    /** simple function which evaluates N-th bit for the 
     *  integral value 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @daet 2008-07-09
     */
    template <class TYPE>
    inline bool bit ( const TYPE value , const unsigned int N  ) 
    {
      BOOST_STATIC_ASSERT(boost::integer_traits<TYPE>::is_integral);
      const TYPE one = 1 ;
      return value & ( one << N ) ;
    }  
    // ========================================================================
  } // end of namespace Gaudi::Math
} // end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_DIGIT_H
// ============================================================================
