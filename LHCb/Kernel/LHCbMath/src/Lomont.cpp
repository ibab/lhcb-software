// $Id: Lomont.cpp,v 1.1 2008-11-08 15:03:39 ibelyaev Exp $
// ============================================================================
// CVS Tag %Name:%, version $Revision: 1.1 $
// ============================================================================
// include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <limits>
#include <cassert>
#include <cmath>
#include <vector>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Lomont.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
#include "boost/integer_traits.hpp"
// ============================================================================
/*  equality comparion of float numbers using as the metric the maximal 
 *  number of Units in the Last Place (ULP).
 *  It is a slightly modified version of very efficient implementation 
 *  of the initial Bruce Dawson's algorithm by Chris Lomont.
 *
 *  @see www.lomont.org 
 *  @see http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
 *
 *  Lomont claims the algorithm is factor 2-10 more efficient 
 *  with respect to  Knuth's algorithm fomr comparions of floating number 
 *  using the relative precision.
 *
 *  The effective relative difference depends on the choice of 
 *   <c>maxULPS</c>:
 *  - For the case of maxULPs=1, (of cource it is totally unphysical case!!!)
 *  the effective relative precision r = |a-b|/(|a|+|b|)is 
 *  between 3.5e-8 and 5.5e-8 for |a|,|b|>1.e-37, and 
 *  then it quickly goes to ~1 
 *  - For the case of maxULPS=10 
 *  the effective relative precision is 
 *  between 3e-8 and 6e-7 for |a|,|b|>1.e-37, and 
 *  then it quickly goes to ~1 
 *  - For the case of maxULPS=100 
 *  the effective relative precision is 
 *  around ~6e-6 for |a|,|b|>1.e-37, and 
 *  then it quickly goes to ~1 
 *  - For the case of maxULPS=1000 
 *  the effective relative precision is 
 *  around ~6e-5 for |a|,|b|>1.e-37, and 
 *  then it quickly goes to ~1 
 *  
 *  @param  af the first number 
 *  @param  bf the second number 
 *  @param  maxULPS the maximal metric deciation in the terms of 
 *                 maximal number of units in the last place
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
bool LHCb::Math::lomont_compare_float 
( const float          af      , 
  const float          bf      , 
  const unsigned short maxULPs ) 
{
  // ==========================================================================
  // prerequisites:
  BOOST_STATIC_ASSERT( std::numeric_limits<float>          ::is_specialized &&
                       boost::integer_traits<int>          ::is_specialized && 
                       boost::integer_traits<unsigned int> ::is_specialized &&
                       sizeof(float)==sizeof(int)                           && 
                       sizeof(float)==sizeof(unsigned int)                  &&
                       32 == boost::integer_traits<unsigned int>::digits    ) ;
  // ==========================================================================
  
  int ai = *reinterpret_cast<const int*>( &af ) ;
  int bi = *reinterpret_cast<const int*>( &bf ) ;
  
  int test = (((unsigned int)(ai^bi))>>31)-1;
  
  // assert ( (0==test) || ( boost::integer_traits<unsigned int>::const_max == test ) ) ;
  
  int diff = ((( boost::integer_traits<int>::const_min - ai ) & (~test)) | ( ai& test )) - bi ;
  
  int maxDiff_ = maxULPs ;
  
  int v1 = maxDiff_ + diff ;
  int v2 = maxDiff_ - diff ;
  
  return 0<=(v1|v2) ;
}
// ============================================================================
/*  get the floating number that representation 
 *  is different with respect  to the argument for 
 *  the certain number of "Units in the Last Position".
 *  For ulps=1, it is just next float number, for ulps=-1 is is the 
 *  previous one.
 *
 *  This routine is very convinient to test the parameter maxULPS for
 *  the routine LHCb::Math::lomont_compare 
 *
 *  @see LHCb:Math::lomont_compare
 *  @param af the reference number 
 *  @param ulps the bias 
 *  @return the biased float number (on distance "ulps")
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-11-08
 */  
// ============================================================================
float LHCb::Math::next_float ( const float af , const short ulps ) 
{
  /// the final check
  BOOST_STATIC_ASSERT( std::numeric_limits<float> ::is_specialized &&
                       std::numeric_limits<int>   ::is_specialized && 
                       sizeof(float)==sizeof(int) ) ;
  
  int ai = *reinterpret_cast<const int*>( &af ) ;
  ai += ulps ;
  return *reinterpret_cast<float*>(&ai) ;
}
// ============================================================================
namespace 
{
  // ========================================================================
#ifdef _WIN32
  struct _Longs 
  {
    typedef __int64            Long   ;
    typedef unsigned __int64   ULong  ;
  } ;  
#else
  template <bool I> 
  struct __Longs ;
  template <>
  struct __Longs<true>
  {
    typedef long               Long   ;
    typedef unsigned long      ULong  ;
  } ;
  template <>
  struct __Longs<false>
  {
    typedef long long               Long   ;
    typedef unsigned long long      ULong  ;
  } ;
  struct _Longs : public __Longs<sizeof(double)==sizeof(long)>{};
#endif 
  
  /// get the final types:
  typedef _Longs::Long  Long  ;
  typedef _Longs::ULong ULong ;
  
  /// the final check
  BOOST_STATIC_ASSERT( std::numeric_limits<double>        ::is_specialized &&
                       std::numeric_limits<Long>          ::is_specialized && 
                       std::numeric_limits<ULong> ::is_specialized &&
                       boost::integer_traits<ULong> ::is_specialized &&
                       boost::integer_traits<Long> ::is_specialized &&
                       sizeof(double)==sizeof(Long)          && 
                       sizeof(double)==sizeof(ULong)  &&
                       64 == std::numeric_limits<ULong>::digits ) ;
}
// ============================================================================
/*  equality comparison of float numbers using as the metric the maximal 
 *  number of Units in the Last Place (ULP).
 *  It is a slightly modified version of very efficient implementation 
 *  of the initial Bruce Dawson's algorithm by Chris Lomont.
 *
 *  @see www.lomont.org 
 *  @see http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
 *
 *  C.Lomont claims the algorithm is factor 2-10 more efficient 
 *  with respect to  Knuth's algorithm fomr comparions of floating number 
 *  using the relative precision.
 *
 *  The effective relative difference depends on the choice of 
 *   <c>maxULPS</c>:
 *  - For the case of maxULPs=1, (of cource it is totally unphysical case!!!)
 *  the effective relative precision r = |a-b|/(|a|+|b|)is 
 *  ~6e-16 for |a|,|b|>1.e-304, and 
 *  then it quickly goes to ~1 
 *  
 *  @param  af the first number 
 *  @param  bf the second number 
 *  @param  maxULPS the maximal metric deciation in the terms of 
 *                 maximal number of units in the last place
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-11-08
 */
// ============================================================================
bool LHCb::Math::lomont_compare_double 
( const double       af      , 
  const double       bf      , 
  const unsigned int maxULPs ) 
{
  /// the final check
  BOOST_STATIC_ASSERT( std::numeric_limits<double>        ::is_specialized &&
                       std::numeric_limits<Long>          ::is_specialized && 
                       std::numeric_limits<ULong> ::is_specialized &&
                       boost::integer_traits<ULong> ::is_specialized &&
                       boost::integer_traits<Long> ::is_specialized &&
                       sizeof(double)==sizeof(Long)          && 
                       sizeof(double)==sizeof(ULong)  &&
                       64 == std::numeric_limits<ULong>::digits ) ;
  
  Long ai = *reinterpret_cast<const Long*>( &af ) ;
  Long bi = *reinterpret_cast<const Long*>( &bf ) ;
  
  Long test = (((ULong)(ai^bi))>>63)-1;
  
  // assert ( (0==test) || ( boost::integer_traits<ULong>::const_max == test ) ) ;
  
  Long diff = ((( boost::integer_traits<Long>::const_min - ai ) & (~test)) | ( ai& test )) - bi ;
  
  Long maxDiff_ = maxULPs ;
  
  Long v1 = maxDiff_ + diff ;
  Long v2 = maxDiff_ - diff ;
  
  return 0<=(v1|v2) ;
}
// ============================================================================
/*  Get the floating number that representation 
 *  is different with respect  to the argument for 
 *  the certain number of "Units in the Last Position".
 *  For ulps=1, it is just next float number, for ulps=-1 is is the 
 *  previous one.
 *
 *  This routine is very convinient to test the parameter maxULPS for
 *  the routine LHCb::Math::lomont_compare_float 
 *
 *  @see LHCb:Math::lomont_compare
 *  @param af the reference number 
 *  @param ulps the bias 
 *  @return the biased float number (on distance "ulps")
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date 2008-11-08
 */  
// ============================================================================
double LHCb::Math::next_double ( const double ad , const short ulps ) 
{
  /// the final check
  BOOST_STATIC_ASSERT( std::numeric_limits<double> ::is_specialized &&
                       std::numeric_limits<Long>   ::is_specialized && 
                       sizeof(double)==sizeof(Long) ) ;
  
  Long al = *reinterpret_cast<const Long*>( &ad ) ;
  al += ulps ;
  return *reinterpret_cast<double*>(&al) ;
}
// ============================================================================
  

// ============================================================================
// The END 
// ============================================================================
  
