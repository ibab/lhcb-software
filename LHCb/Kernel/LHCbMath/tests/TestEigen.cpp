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
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/EigenSystem.h"
// ============================================================================
/** @file
 *  Test some Win32 compilation problme with matrix eigenvalues
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2010-02-09
 */
// ============================================================================
int main()
{
  
  // create the evaluator 
  Gaudi::Math::GSL::EigenSystem eval ;  
  
  Gaudi::SymMatrix3x3 cov ;
  cov(0,0) = 1 ;
  cov(1,1) = 2 ;
  cov(2,2) = 3 ;
  // get the sorted vector of eigenvalues and eigenvectors
  Gaudi::Vector3   eigval;
  Gaudi::Matrix3x3 eigvects;
  StatusCode sc = eval.eigenVectors( cov , 
                                     eigval, eigvects , true );
  if( sc.isFailure() ) {
    std::cout << "Bad StatusCode from eval.eigenVectors = " << sc << std::endl;
  }
  
  std::cout 
    << " eigenvalues : " << eigval   << std::endl 
    << " matrix      : " << eigvects << std::endl ;
  
  return 0 ;  
}




//=============================================================================
