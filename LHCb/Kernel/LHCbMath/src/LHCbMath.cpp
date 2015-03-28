// $Id$
// ============================================================================
// Include files
// ============================================================================
// GSL 
// ============================================================================
#include "gsl/gsl_sys.h"
// ============================================================================
// Local
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/integer_traits.hpp"
// ============================================================================
/** @file 
 *  Implementation file for functions from namespace LHcb::Math 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-11-27
 */
// ============================================================================
/*  compare two double numbers with the relative
 *  precision   'epsilon'
 *
 *  Essentially it is a wrapper to gsl_fcmp function 
 *  from GSL library
 *
 *  See D.E.Knuth, "Seminumerical Algorithms", section 4.2.2
 *  
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-11-27
 */
// ============================================================================
#ifdef __INTEL_COMPILER         // Disable ICC remark
#pragma warning(disable:1572) // floating-point equality and inequality comparisons are unreliable
#endif

bool LHCb::Math::knuth_equal_to_double
( const double value1  , 
  const double value2  , 
  const double epsilon ) 
{
  return 
    !epsilon    ? 0 == gsl_fcmp ( value1 , value2 , 1.0e-6  ) : 
    0 < epsilon ? 0 == gsl_fcmp ( value1 , value2 , epsilon ) : 
    0 == gsl_fcmp ( value1 , value2 , -epsilon ) ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  /// check the specialization 
  static_assert ( boost::integer_traits<long> ::is_specialized     , 
                  "boost::integer_traits<long> is not specialized" ) ;
  static_assert ( boost::integer_traits<int>  ::is_specialized     , 
                  "boost::integer_traits<int>  is not specialized" ) ;
  // ==========================================================================
  const double s_MAX_L =  0.1 + boost::integer_traits<long>::const_max ;
  const double s_MIN_L = -0.1 - boost::integer_traits<long>::const_max ;
  const double s_MAX_I =  0.1 + boost::integer_traits<int>::const_max  ;
  const double s_MIN_I = -0.1 - boost::integer_traits<int>::const_max  ;
  // ==========================================================================
}
// ============================================================================
/*  is the value actually long ?
 *  @author Vanya BELYAEV Ivan.Belyaev       
 *  @date 2011-07-18
 */
// ============================================================================
bool LHCb::Math::islong ( const double x ) 
{
  return 
    x <= s_MIN_L  ? false :
    x >= s_MAX_L  ? false :
    Gaudi::Math::lomont_compare_double ( x                       , 
                                         LHCb::Math::round ( x ) , 
                                         mULPS_double            ) ;
}
// ============================================================================
/*  is the value actually long ?
 *  @author Vanya BELYAEV Ivan.Belyaev       
 *  @date 2011-07-18
 */
// ============================================================================
bool LHCb::Math::islong ( const float x ) 
{
  return 
    x <= s_MIN_L  ? false :
    x >= s_MAX_L  ? false :
    Gaudi::Math::lomont_compare_double ( x                       , 
                                         LHCb::Math::round ( x ) , 
                                         mULPS_float             ) ;
}
// ============================================================================
/*  is the value actually int ?
 *  @author Vanya BELYAEV Ivan.Belyaev       
 *  @date 2011-07-18
 */
// ============================================================================
bool LHCb::Math::isint ( const double x ) 
{
  return 
    x <= s_MIN_I  ? false :
    x >= s_MAX_I  ? false :
    Gaudi::Math::lomont_compare_double ( x                       , 
                                         LHCb::Math::round ( x ) , 
                                         mULPS_double            ) ;
}
// ============================================================================
/*  is the value actually int ?
 *  @author Vanya BELYAEV Ivan.Belyaev       
 *  @date 2011-07-18
 */
// ============================================================================
bool LHCb::Math::isint ( const float x ) 
{
  return 
    x <= s_MIN_I  ? false :
    x >= s_MAX_I  ? false :
    Gaudi::Math::lomont_compare_double ( x                       , 
                                         LHCb::Math::round ( x ) , 
                                         mULPS_float             ) ;
}
// ============================================================================
/* check if the double value is actually equal to the integer value  
 *  @param val value to be compared with the integer 
 *  @param ref the reference integer number 
 *  @param mULPS the precision 
 *  @see Gaudi::Math::lomont_compare_double 
 *  @see LHCb::Math::mULPS_double
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-09-17
 */        
// ============================================================================
bool LHCb::Math::equal_to_int 
( const double       val   , 
  const int          ref   , 
  const unsigned int mULPS ) 
{
  const double tmp = ref ;
  return Gaudi::Math::lomont_compare_double ( val , tmp , mULPS ) ;
}
// ============================================================================
/*  check if the double value is actually equal to the unsigned integer value  
 *  @param val value to be compared with the unsigned integer 
 *  @param ref the reference unsigned integer number 
 *  @param mULPS the precision 
 *  @see Gaudi::Math::lomont_compare_double 
 *  @see LHCb::Math::mULPS_double
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-09-17
 */        
// ============================================================================
bool LHCb::Math::equal_to_uint 
( const double       val   , 
  const unsigned int ref   , 
  const unsigned int mULPS ) 
{
  const double tmp = ref ;
  return Gaudi::Math::lomont_compare_double ( val , tmp , mULPS ) ;
}
// ============================================================================
// The END 
// ============================================================================
