// $Id: LHCbMath.cpp,v 1.2 2010-01-03 08:41:17 ibelyaev Exp $
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
