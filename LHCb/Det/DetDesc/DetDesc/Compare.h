// $Id: Compare.h,v 1.1 2007-12-19 09:42:39 ibelyaev Exp $
// =========================================================================
#ifndef DETDESC_COMPARE_H 
#define DETDESC_COMPARE_H 1
// =========================================================================
// Include files
// =========================================================================
// GSL
// =========================================================================
#include "gsl/gsl_sys.h"
// =========================================================================
namespace DetDesc
{
  // =======================================================================
  /** @var s_tolerance 
   *  the relative tolerance of tick/intersection comparison 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-12-13
   */
  const double s_tolerance = 2.e-8 ;
  // =======================================================================  
  /** @compare double/floating numbers 
   *  @see gsl_fcmp 
   *  @param x1 the  first value to be compared 
   *  @param x2 the  second valeu to be compared 
   *  @param eps the RELATIVE precision 
   *  @return -1 if x1 < x2 , 0 if x1 == x2 and +1 if x1 > x2 
   *  @warning it is not a rigth method to compae with ZERO!
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-12-13
   */
  inline int compare 
  ( const double x1  , 
    const double x2  , 
    const double eps ) { return gsl_fcmp ( x1 , x2 , eps ) ; }
  // =======================================================================  
  /** @compare double/floating numbers 
   *  @see gsl_fcmp 
   *  @param x1 the  first value to be compared 
   *  @param x2 the  second valeu to be compared 
   *  @return -1 if x1 < x2 , 0 if x1 == x2 and +1 if x1 > x2 
   *  @warning it is not a rigth method to compare with ZERO!
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-12-13
   */
  inline int compare 
  ( const double x1  , 
    const double x2  )
  { return DetDesc::compare ( x1 , x2 , s_tolerance ) ; }
  // =======================================================================  
} //end of namespaceDetDesc 
// ============================================================================
// The END 
// ============================================================================
#endif // DETDESC_COMPARE_H
// ============================================================================
