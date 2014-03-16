// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
// ============================================================================
/** @file
 *  Test SMatrix/SVector problem, see #bug 104111
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2014-03-16
 *
 *                    $Revision$
 *  LAst modification $Date$
 *                 by $Author$
 */
// ============================================================================
int main()
{
  
  Gaudi::SymMatrix3x3 matrix ;
  matrix(0,0) = 10 ;
  matrix(0,1) =  7 ;
  matrix(0,2) =  5 ;
  matrix(1,1) =  9 ;
  matrix(1,2) =  6 ;
  matrix(2,2) =  8 ;
  //
  std::cout << "\nMATRIX:\n" << matrix << std::endl ;

  Gaudi::Vector3 v1 ( 1 , 2 , 3 ) ;
  Gaudi::Vector3 v2 = v1          ;
  
  std::cout << "\nVECTOR: [" << v1 << "] "<< std::endl ;
  
  v1 = matrix *                  v1   ;
  v2 = matrix * Gaudi::Vector3 ( v2 ) ;
  
  std::cout << "\n\n\n" << std::endl ;
  std::cout << "RESULTS *MUST* BE IDENTICAL:"  << std::endl 
            << "[v1 = matrix*        v1 ] : " << v1 << std::endl
            << "[v1 = matrix*Vector3(v1)] : " << v2 << std::endl ;
  std::cout << "\n\n\n"  << std::endl ;
  return 0 ;  
}
// ============================================================================
// THE END 
// ============================================================================
