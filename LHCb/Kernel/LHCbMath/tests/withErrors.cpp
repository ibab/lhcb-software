// $Id: withErrors.cpp,v 1.1 2009-06-13 18:30:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
//  STD & STL 
// ============================================================================
#include <iostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
// ============================================================================
// Local 
// ============================================================================
#include "LHCbMath/SVectorWithError.h"
// ============================================================================
/** @file 
 *  Simple file to test class Gaudi::Math::SVectorWithError 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-06-13
 */
// ============================================================================
int main () 
{
 
  Gaudi::Vector3      v1 ( 0 , 1 , 2 ) ;
  Gaudi::Vector3      v2 ( 1 , 1 , 1 ) ;
  Gaudi::SymMatrix3x3 m1 ;
  m1(0,0) = 1 ;
  m1(1,1) = 4 ;
  m1(2,2) = 9 ;
  
  Gaudi::Math::SVectorWithError<3,double> d1 ( v1 , m1    ) ;
  Gaudi::Math::SVectorWithError<3,double> d2 ( v1 + 10*v1 ) ;
  Gaudi::Math::SVectorWithError<3,double> d3 ( m1 + 100*m1 ) ;
  Gaudi::Math::SVectorWithError<3,double> d4 ( v1 + v1*2 , m1 + 4*m1 ) ;

  std::cout 
    << " v1:     " <<  v1      << std::endl 
    << " d1:     " <<  d1      << std::endl 
    << " d2:     " <<  d2      << std::endl 
    << " d3:     " <<  d3      << std::endl 
    << " d4:     " <<  d4      << std::endl 
    << " d4*=2:  " << (d4*=2)  << std::endl 
    << " d1-d2:  " << (d1-d2)  << std::endl 
    << " d1-v1:  " << (d1-v1)  << std::endl 
    << " v1-d1:  " << (v1+d1)  << std::endl 
    << " v1+d2:  " << (v1-d2)  << std::endl 
    << " c21:    " << Gaudi::Math::chi2(v1,d4)  << std::endl 
    << " c22:    " << Gaudi::Math::chi2(d4,v1)  << std::endl 
    << " c23:    " << Gaudi::Math::chi2(d1,d4)  << std::endl ;
  
}
// ============================================================================
// The END 
// ============================================================================
