// $Id: Compare.h,v 1.4 2009-11-15 16:37:53 ibelyaev Exp $
// ============================================================================
#ifndef DETDESC_COMPARE_H 
#define DETDESC_COMPARE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Lomont.h"
// ============================================================================
namespace DetDesc
{
  // ==========================================================================
  /** @var s_tolerance_ULPs
   *  the tolerance in ULPs-units of tick/intersection comparison 
   *  @see Gaudi::Math::Lomont 
   *  @see Gaudi::Math::lomont_comapre_double 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-12-13
   */
  const unsigned int s_tolerance_ULPs = 1000 ;
  // ==========================================================================
  /** Compare double/floating numbers 
   *  @see gsl_fcmp 
   *  @param x1 the  first value to be compared 
   *  @param x2 the  second valeu to be compared 
   *  @param ULPs the precision in ULPs-units 
   *  @return -1 if x1 < x2 , 0 if x1 == x2 and +1 if x1 > x2 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-12-13
   */
  inline int compare 
  ( const double        x1   , 
    const double        x2   , 
    const unsigned int  ULPs ) 
  { 
    return 
      Gaudi::Math::lomont_compare_double ( x1 , x2 , ULPs ) ? 0 : x1 < x2 ? -1 : 1 ;
  }
  // ==========================================================================
  /** Compare double/floating numbers 
   *  @see gsl_fcmp 
   *  @param x1 the  first value to be compared 
   *  @param x2 the  second valeu to be compared 
   *  @return -1 if x1 < x2 , 0 if x1 == x2 and +1 if x1 > x2 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-12-13
   */
  inline int compare 
  ( const double x1  , 
    const double x2  )
  { return DetDesc::compare ( x1 , x2 , s_tolerance_ULPs ) ; }
  // ==========================================================================
} //                                                   end of namespace DetDesc 
// ============================================================================
// The END 
// ============================================================================
#endif // DETDESC_COMPARE_H
// ============================================================================
