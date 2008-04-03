// $Id: LHCbMath.cpp,v 1.1 2008-04-03 11:26:18 cattanem Exp $
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
bool LHCb::Math::equal_to_double
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
