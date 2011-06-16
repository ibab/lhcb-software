// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/ChronoEntity.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Lomont.h"
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// GSL
// =========================================================================
#include "gsl/gsl_sys.h"
// =========================================================================
// Boost 
// ============================================================================
using namespace std;
#ifdef __INTEL_COMPILER         // Disable ICC remark from Boost
  #pragma warning(disable:1572) // floating-point equality and inequality comparisons are unreliable
#endif
#include "boost/progress.hpp"
// ============================================================================
/** @file 
 *  Test for CPU-performance of "Lomont" versus "Knuth" copare of doubles 
 * 
 *  @code 
 *  0%   10   20   30   40   50   60   70   80   90   100%
 *  |----|----|----|----|----|----|----|----|----|----|
 *  ***************************************************
 *  Knuth/GSL-compare Time : Tot= 24.8  [s] Ave/Min/Max= 1.24(+-0.428)/    0/    2 [ms] #=20000
 *  Lomont-compare    Time : Tot= 5.86  [s] Ave/Min/Max=  293(+-  455)/    0/1e+03 [us] #=20000 
 *  @endcode 
 *
 *  Lomont-type compare seems to be around ~4 times more CPU-efficient 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-10-22
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  bool knuth_compare_double 
  ( const double x1          , 
    const double x2          , 
    const double eps = 1.e-8 ) 
  { return 0 == gsl_fcmp ( x1 , x2 , eps ) ; }
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
int main()
{
  using namespace LHCb::Math ;
  
 
  ChronoEntity counter1 ;
  ChronoEntity counter2 ;
  
  double value1 = 0 ;
  double value2 = 0 ;
  
  const unsigned int nMax  =  100 ;
  const unsigned int nMax1 =  200 ;
  const unsigned int nMax2 = 5000 ;
  
  boost::progress_display show_progress ( nMax );
  for ( unsigned int m = 0 ; m < nMax ; ++m )
  {
    
    const double v1 = sin(      double(m) ) * exp( 50 * cos(      double(m) ) ) ;
    const double v2 = sin( v1 * double(m) ) * exp( 50 * cos( v1 * double(m) ) ) ;
    
    {
      for ( unsigned int i = 0 ; i < nMax1 ; ++i ) 
      {
        Chrono cnt ( &counter1 ) ;
        for ( unsigned int j = 0 ; j < nMax2 ; ++j ) 
        {
          value1 += knuth_compare_double ( v1 , v2         ) ;
          value1 += knuth_compare_double ( v1 , v2 * i     ) ;
          value1 += knuth_compare_double ( v1 , v2 / j     ) ;
          value1 += knuth_compare_double ( v1 , v2 * i / j ) ;
        } 
      }
    }
    
    {
      for ( unsigned int i = 0 ; i < nMax1 ; ++i ) 
      {
        Chrono cnt ( &counter2 ) ;
        for ( unsigned int j = 0 ; j < nMax2 ; ++j ) 
        {
          value2 += lomont_compare_double ( v1 , v2          , 100 ) ;
          value2 += lomont_compare_double ( v1 , v2 * i      , 100 ) ;
          value2 += lomont_compare_double ( v1 , v2 / j      , 100 ) ;
          value2 += lomont_compare_double ( v1 , v2 * i / j  , 100 ) ;
        } 
      }
    }
    //
    ++show_progress ;
  }

  std::cout 
    << " Knuth/GSL-compare " <<  value1  << " " 
    << counter1.outputUserTime() << std::endl 
    << " Lomont-compare    " <<  value2  << " " 
    << counter2.outputUserTime() << std::endl ;
  
  return 0 ;
  
}
// ============================================================================
// The END 
// ============================================================================
