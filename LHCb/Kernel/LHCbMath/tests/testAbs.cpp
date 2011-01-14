// $Id:$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <iomanip>
#include <cmath>
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
#include "boost/assert.hpp"
// ============================================================================
/** @file
 *  Set of simple tests for various "std::abs" functions 
 *
 *  It prints:
 * @code 
 * Value:  -4115   std::fabs - std::abs :          0       fabs - abs :    0
 * Value:  1371.6667       std::fabs - std::abs :          0       fabs - abs :    0.66666667
 * Value:  -457.22222      std::fabs - std::abs :          0       fabs - abs :    0.22222222
 * Value:  152.40741       std::fabs - std::abs :          0       fabs - abs :    0.40740741
 * Value:  -50.802469      std::fabs - std::abs :          0       fabs - abs :    0.80246914
 * Value:  16.934156       std::fabs - std::abs :          0       fabs - abs :    0.93415638
 * Value:  -5.6447188      std::fabs - std::abs :          0       fabs - abs :    0.64471879
 * Value:  1.8815729       std::fabs - std::abs :          0       fabs - abs :    0.88157293
 * Value:  -0.62719098     std::fabs - std::abs :          0       fabs - abs :    0.62719098
 * Value:  0.20906366      std::fabs - std::abs :          0       fabs - abs :    0.20906366
 * Value:  -0.069687886    std::fabs - std::abs :          0       fabs - abs :    0.069687886
 * Value:  0.023229295     std::fabs - std::abs :          0       fabs - abs :    0.023229295
 *
 * @endcode 
 *
 * @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 * @date 2011-01-14
 */
// ============================================================================
int main()
{
  //
  // It prints:
  //
  // Value:  -4115   std::fabs - std::abs :          0       fabs - abs :    0
  // Value:  1371.6667       std::fabs - std::abs :          0       fabs - abs :    0.66666667
  // Value:  -457.22222      std::fabs - std::abs :          0       fabs - abs :    0.22222222
  // Value:  152.40741       std::fabs - std::abs :          0       fabs - abs :    0.40740741
  // Value:  -50.802469      std::fabs - std::abs :          0       fabs - abs :    0.80246914
  // Value:  16.934156       std::fabs - std::abs :          0       fabs - abs :    0.93415638
  // Value:  -5.6447188      std::fabs - std::abs :          0       fabs - abs :    0.64471879
  // Value:  1.8815729       std::fabs - std::abs :          0       fabs - abs :    0.88157293
  // Value:  -0.62719098     std::fabs - std::abs :          0       fabs - abs :    0.62719098
  // Value:  0.20906366      std::fabs - std::abs :          0       fabs - abs :    0.20906366
  // Value:  -0.069687886    std::fabs - std::abs :          0       fabs - abs :    0.069687886
  // Value:  0.023229295     std::fabs - std::abs :          0       fabs - abs :    0.023229295
  //
  double value = 12345. ;
  //
  while ( std::fabs( value ) > 0.05 ) 
  {
    value /= -3.0 ;
    std::cout 
      << "Value:"
      << " \t" << std::setprecision ( 8 ) << value 
      << " \tstd::fabs - std::abs : "
      << " \t" << std::setprecision ( 8 ) 
      << ( std::fabs ( value ) - std::abs( value ) ) 
      << " \tfabs - abs :  "      
      << " \t" << std::setprecision ( 8 ) 
      << ( ::fabs ( value ) - ::abs( value ) ) 
      << std::endl;
  }
  // ==========================================================================
  exit(0) ;                                                             // EXIT
  // ==========================================================================
}    
// ============================================================================
// The END 
// ============================================================================
