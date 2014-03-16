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
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Chi2Solution.h"
// ============================================================================
int main() 
{
  //
  typedef Gaudi::Math::Chi2Solution<4,2> SOLUTION ;
  //
  SOLUTION::DATA  data ;
  data[0] = 110 ;
  data[1] =  90 ;
  data[2] = 105 ;
  data[3] =  95 ;
  //
  SOLUTION::COV2 cov2 ;
  for ( unsigned short i = 0 ; i < 4 ; ++i ) 
  { cov2(i,i) = data[i] ; }
  //
  // matrix of constraints 
  SOLUTION::CMTRX1 D ;
  D ( 0 , 0 ) =  1 ;
  D ( 0 , 1 ) = -1 ;
  D ( 1 , 2 ) =  1 ;
  D ( 1 , 3 ) = -1 ;
  //
  std::cout << " input  solution     " << data << std::endl ;
  std::cout << " input  covariance \n" << cov2 << std::endl ;
  std::cout << " constrain matrix  \n" << D << std::endl ;
  //
  double     chi2  = -100 ;
  StatusCode sc    = SOLUTION::solve ( data , 
                                       cov2 , 
                                       D    ,
                                       SOLUTION::COFF () , 
                                       chi2 ) ;
  
  std::cout << " StatusCode "          << sc   << std::endl ;
  std::cout << " output solution     " << data << std::endl ;
  std::cout << " output covariance \n" << cov2 << std::endl ;
  std::cout << " Chi2 "                << chi2 << std::endl ;
  
  exit(0) ;
}
// ============================================================================

  
