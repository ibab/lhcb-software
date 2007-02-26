// ============================================================================
#ifndef LHCBMATH_POWER_H 
#define LHCBMATH_POWER_H 1
// ============================================================================
// Include files
// ============================================================================

#include <cmath>

// ============================================================================
/** @file
 *
 *  This file has been taken from the LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace Gaudi 
{  
  namespace Math 
  {
    
    /** @fn pow 
     *  Simple utility for efficient "pow"
     *  Works only for positive integer powers.
     *
     *
     *  @code 
     *   
     *   const double result = Gaudi::Math::pow ( value , 10 ) ;
     *
     *  @endcode 
     *
     *  The actual code is copied from 
     *     std::__cmath_power bits/cmath.tcc 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-04-09 
     */
    template<typename TYPE>
    inline TYPE pow ( TYPE __x , unsigned long __n )
    {
      TYPE __y = __n % 2 ? __x : 1;
    
      while ( __n >>= 1 )
      {
        __x = __x * __x;
        if ( __n % 2) { __y = __y * __x; }
      } ;
    
      return __y ;
    };

    /** @brief Implements the cube root function cbrt(double x) on all platforms.
     *
     *  On gcc systems uses the built in cbrt(x) method,
     *  on other systems defaults to the slow but safe pow(x,1.0/3.0) method.
     *
     *  @param __x Number to cube root
     *
     *  @return cube root of the given number
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date 27/10/2006
     */
    inline double cbrt( double __x )
    {
#ifdef __GNUC__
      return ::cbrt ( __x           );
#else
      return ::pow  ( __x , static_cast<double>(1.0/3.0) );
#endif
    }  

    /** @brief Implements the cube root function cbrt(float x) on all platforms.
     *
     *  On gcc systems uses the built in cbrt(x) method,
     *  on other systems defaults to the slow but safe pow(x,1.0/3.0) method.
     *
     *  @param __x Number to cube root
     *
     *  @return cube root of the given number
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date 27/10/2006
     */
    inline float cbrt( float __x )
    {
#ifdef __GNUC__
      return ::cbrtf ( __x           );
#else
      return ::pow   ( __x , static_cast<float>(1.0/3.0) );
#endif
    }  

    /** @brief Implements the cube root function cbrt(long double x) on all platforms.
     *
     *  On gcc systems uses the built in cbrt(x) method,
     *  on other systems defaults to the slow but safe pow(x,1.0/3.0) method.
     *
     *  @param __x Number to cube root
     *
     *  @return cube root of the given number
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date 27/10/2006
     */
    inline long double cbrt( long double __x )
    {
#ifdef __GNUC__
      return ::cbrtl ( __x           );
#else
      return ::pow   ( __x , static_cast<long double>(1.0/3.0) );
#endif
    }

    /** @brief Implements the cube root function cbrt(long x) on all platforms.
     *
     *  On gcc systems uses the built in cbrt(x) method,
     *  on other systems defaults to the slow but safe pow(x,1.0/3.0) method.
     *
     *  @param __x Number to cube root
     *
     *  @return cube root of the given number
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date 27/10/2006
     */
    inline double cbrt( long __x )
    {
      return Gaudi::Math::cbrt ( static_cast<double>(__x) ) ;
    }

    /** @brief Implements the cube root function cbrt(int x) on all platforms.
     *
     *  On gcc systems uses the built in cbrt(x) method,
     *  on other systems defaults to the slow but safe pow(x,1.0/3.0) method.
     *
     *  @param __x Number to cube root
     *
     *  @return cube root of the given number
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date 27/10/2006
     */
    inline double cbrt( int __x )
    {
      return Gaudi::Math::cbrt ( static_cast<double>(__x) ) ;
    }

    /** @brief Implements the cube root function cbrt(unsigned long x) on all platforms.
     *
     *  On gcc systems uses the built in cbrt(x) method,
     *  on other systems defaults to the slow but safe pow(x,1.0/3.0) method.
     *
     *  @param __x Number to cube root
     *
     *  @return cube root of the given number
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date 27/10/2006
     */
    inline double cbrt( unsigned long __x )
    {
      return Gaudi::Math::cbrt ( static_cast<double>(__x) ) ;
    }

    /** @brief Implements the cube root function cbrt(unsigned int x) on all platforms.
     *
     *  On gcc systems uses the built in cbrt(x) method,
     *  on other systems defaults to the slow but safe pow(x,1.0/3.0) method.
     *
     *  @param __x Number to cube root
     *
     *  @return cube root of the given number
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date 27/10/2006
     */
    inline double cbrt( unsigned int __x )
    {
      return Gaudi::Math::cbrt ( static_cast<double>(__x) ) ;
    }

  }; // end of namespace Math
  
}; // end of namespace Gaudi

// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_POWER_H
// ============================================================================
