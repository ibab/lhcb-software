// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Chi2Fit.h"
// ============================================================================
/** @file
 *  @date 2010-02-09
 */
// ============================================================================
int main()
{
  /// data 
  Gaudi::Math::Chi2Fit::DATA data ;
  /// components 
  Gaudi::Math::Chi2Fit::CMPS cmps ;

  typedef Gaudi::Math::Chi2Fit::VE VE ;
  
  data.push_back ( VE ( 100 ,  100 ) ) ;
  data.push_back ( VE ( 400 ,   20 ) ) ;
  data.push_back ( VE ( 500 ,   20 ) ) ;
  data.push_back ( VE ( 600 ,   10 ) ) ;
  data.push_back ( VE ( 700 ,  10 ) ) ;
  data.push_back ( VE ( 700 ,  200 ) ) ;
  data.push_back ( VE (-700 ,  2   ) ) ;
  data.push_back ( VE ( 700 ,  2   ) ) ;
  //   data.push_back ( VE ( 700 ,  200 ) ) ;
  //   data.push_back ( VE ( 700 ,  200 ) ) ;
  
  Gaudi::Math::Chi2Fit::DATA cmp1 ;  
  cmp1.push_back ( VE (   90 , 60   ) ) ;  
  cmp1.push_back ( VE (  390 , 10   ) ) ;
  cmp1.push_back ( VE (    0 , 60   ) ) ;
  cmp1.push_back ( VE (    0 , 100  ) ) ;
  cmp1.push_back ( VE (  512 , 100  ) ) ;
  cmp1.push_back ( VE (  290 , 100  ) ) ;
  cmp1.push_back ( VE (  190 , 100  ) ) ;
  cmp1.push_back ( VE (   90 , 100  ) ) ;
  //   cmp1.push_back ( VE (  -90 , 10  ) ) ;
  //   cmp1.push_back ( VE ( -190 , 10  ) ) ;
  
  Gaudi::Math::Chi2Fit::DATA cmp2 ;  
  cmp2.push_back ( VE (   10 , 10   ) ) ;  
  cmp2.push_back ( VE (   0  , 10   ) ) ;
  cmp2.push_back ( VE ( 460 , 30   ) ) ;
  cmp2.push_back ( VE ( 600 , 10   ) ) ;
  cmp2.push_back ( VE (-900 , 10   ) ) ;
  cmp2.push_back ( VE (  290 , 10  ) ) ;
  cmp2.push_back ( VE (  290 , 10  ) ) ;
  cmp2.push_back ( VE ( -290 , 10  ) ) ;
  //   cmp2.push_back ( VE ( -290 , 10  ) ) ;
  //   cmp2.push_back ( VE ( 19000 , 10  ) ) ;
  
  Gaudi::Math::Chi2Fit::DATA cmp3 ;  
  cmp3.push_back ( VE (    0 , 10  ) ) ;  
  cmp3.push_back ( VE ( -200 , 40  ) ) ;
  cmp3.push_back ( VE (  460 , 40  ) ) ;
  cmp3.push_back ( VE ( -600 , 40  ) ) ;
  cmp3.push_back ( VE ( -900 , 40  ) ) ;
  cmp3.push_back ( VE (  100 , 40  ) ) ;
  cmp3.push_back ( VE (-7000 , 10  ) ) ;
  cmp3.push_back ( VE (-5000 , 10  ) ) ;
  //   cmp3.push_back ( VE (    0 , 10  ) ) ;
  //   cmp3.push_back ( VE (  500 , 10  ) ) ;
  
  Gaudi::Math::Chi2Fit::DATA cmp4 ;  
  cmp4.push_back ( VE ( -500 , 0 ) ) ;  
  cmp4.push_back ( VE (  200 , 0 ) ) ;
  cmp4.push_back ( VE (  460 , 0 ) ) ;
  cmp4.push_back ( VE (  600 , 0 ) ) ;
  cmp4.push_back ( VE ( 1400 , 0 ) ) ;
  cmp4.push_back ( VE (-1000 , 0 ) ) ;
  cmp4.push_back ( VE (+1000 , 0 ) ) ;
  cmp4.push_back ( VE (-1000 , 0 ) ) ;
  cmp4.push_back ( VE (+1000 , 0 ) ) ;
  cmp4.push_back ( VE (  100 , 0 ) ) ;

  
  Gaudi::Math::Chi2Fit::DATA cmp5 ; 
  for ( Gaudi::Math::Chi2Fit::DATA::const_iterator iv = data.begin() ; 
         data.end() != iv ; ++iv)
  {
    cmp5.push_back ( 0.25 * ( *iv ) ) ;
  }

  Gaudi::Math::Chi2Fit::DATA cmp6 ; 
  for ( Gaudi::Math::Chi2Fit::DATA::const_iterator iv = data.begin() ; 
         data.end() != iv ; ++iv)
  {
    cmp6.push_back ( 0.25 * ( *iv ) ) ;
  }
  
  // cmp6[1] *= 1.1 ;
  cmp5[1] /= 1.1 ;
  
  cmps.push_back ( cmp1 ) ;
  cmps.push_back ( cmp2 ) ;
  cmps.push_back ( cmp3 ) ;
  cmps.push_back ( cmp4 ) ;
  // cmps.push_back ( cmp5 ) ;
  cmps.push_back ( cmp6 ) ;
  
  Gaudi::Math::Chi2Fit fit ( data , cmps ) ;
  // Gaudi::Math::Chi2Fit fit ( data , data  ) ;
  
  std::cout << fit << std::endl ;
  
  return 0 ;
  
} 
// ============================================================================
//                                                                      The END 
// ============================================================================
