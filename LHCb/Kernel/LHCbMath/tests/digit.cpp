// $Id: digit.cpp,v 1.2 2008-08-02 10:14:05 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <cmath>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Digit.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
#include "boost/assert.hpp"
// ============================================================================
/** @file
 *  Set of simple tests for various "digit" functions 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-31
 */
// ============================================================================
int main()
{
  using namespace Gaudi::Math  ;
  
  {
    unsigned int value = 724 ;
    std::cout 
      <<  " Value           " << value  << std::endl  
      <<  " digit(0)        " << digit ( value ,   0 )  << std::endl 
      <<  " digit(1)        " << digit ( value ,   1 )  << std::endl 
      <<  " digit(2)        " << digit ( value ,   2 )  << std::endl 
      <<  " digit(3)        " << digit ( value ,   3 )  << std::endl 
      <<  " digit(4)        " << digit ( value ,   4 )  << std::endl 
      <<  " digit(5)        " << digit ( value ,   5 )  << std::endl 
      <<  " digit(6)        " << digit ( value ,   6 )  << std::endl 
      <<  " digit(7)        " << digit ( value ,   7 )  << std::endl 
      <<  " digit(64)       " << digit ( value ,  64 )  << std::endl 
      <<  " digit(200)      " << digit ( value , 200 )  << std::endl 
      ;
  }
  
  { 
    unsigned int value = 724 ;
    
    Digit<unsigned int,0>   bit0 ;
    Digit<unsigned int,1>   bit1 ;
    Digit<unsigned int,2>   bit2 ;
    Digit<unsigned int,3>   bit3 ;
    Digit<unsigned int,4>   bit4 ;
    Digit<unsigned int,5>   bit5 ;
    Digit<unsigned int,6>   bit6 ;
    Digit<unsigned int,7>   bit7 ;
    //
    std::cout 
      <<  " Digit<0>        " << bit0   ( value )  << std::endl 
      <<  " Digit<1>        " << bit1   ( value )  << std::endl 
      <<  " Digit<2>        " << bit2   ( value )  << std::endl 
      <<  " Digit<3>        " << bit3   ( value )  << std::endl 
      <<  " Digit<4>        " << bit4   ( value )  << std::endl 
      <<  " Digit<5>        " << bit5   ( value )  << std::endl 
      <<  " Digit<6>        " << bit6   ( value )  << std::endl 
      <<  " Digit<7>        " << bit7   ( value )  << std::endl 
      ;  
  }
  

  { 
    std::cout 
      <<  " IDigit<724,0>   " << IDigit<unsigned int,724,0>  ::value << std::endl 
      <<  " IDigit<724,1>   " << IDigit<unsigned int,724,1>  ::value << std::endl 
      <<  " IDigit<724,2>   " << IDigit<unsigned int,724,2>  ::value << std::endl 
      <<  " IDigit<724,3>   " << IDigit<unsigned int,724,3>  ::value << std::endl 
      <<  " IDigit<724,4>   " << IDigit<unsigned int,724,4>  ::value << std::endl 
      <<  " IDigit<724,5>   " << IDigit<unsigned int,724,5>  ::value << std::endl 
      <<  " IDigit<724,6>   " << IDigit<unsigned int,724,6>  ::value << std::endl 
      <<  " IDigit<724,7>   " << IDigit<unsigned int,724,7>  ::value << std::endl 
      ;  
  }
  

  {
    const unsigned short value = static_cast<unsigned short> ( -1 ); 
    std::cout
      <<  std::endl 
      <<  " Value "           <<          value              << std::endl 
      <<  " digits(0,0)     " << digits ( value ,  0 ,   0 ) << std::endl 
      <<  " digits(0,1)     " << digits ( value ,  0 ,   1 ) << std::endl 
      <<  " digits(0,2)     " << digits ( value ,  0 ,   2 ) << std::endl 
      <<  " digits(0,3)     " << digits ( value ,  0 ,   3 ) << std::endl 
      <<  " digits(0,4)     " << digits ( value ,  0 ,   4 ) << std::endl 
      <<  " digits(0,5)     " << digits ( value ,  0 ,   5 ) << std::endl 
      <<  " digits(0,6)     " << digits ( value ,  0 ,   6 ) << std::endl 
      <<  " digits(0,7)     " << digits ( value ,  0 ,   7 ) << std::endl 
      <<  " digits(0,8)     " << digits ( value ,  0 ,   8 ) << std::endl 
      <<  " digits(0,9)     " << digits ( value ,  0 ,   9 ) << std::endl 
      <<  " digits(2,10)    " << digits ( value ,  2 ,  10 ) << std::endl 
      <<  " digits(3,4)     " << digits ( value ,  3 ,   4 ) << std::endl 
      <<  " digits(5,64)    " << digits ( value ,  5 ,  64 ) << std::endl 
      <<  " digits(0,200)   " << digits ( value ,  0 , 200 ) << std::endl 
      <<  " digits(85,200)  " << digits ( value , 85 , 200 ) << std::endl 
      ;
  }
  
  
  {
    const unsigned short value = static_cast<unsigned short> ( -1 ); 
    std::cout
      <<  std::endl 
      <<  " Value "           <<          value              << std::endl 
      <<  " digits<0,1>     " << Digits<unsigned short,0,1> () ( value ) << std::endl 
      <<  " digits<0,2>     " << Digits<unsigned short,0,2> () ( value ) << std::endl 
      <<  " digits<0,3>     " << Digits<unsigned short,0,3> () ( value ) << std::endl 
      <<  " digits<0,4>     " << Digits<unsigned short,0,4> () ( value ) << std::endl 
      <<  " digits<0,5>     " << Digits<unsigned short,0,5> () ( value ) << std::endl 
      ;
  }

  {
    std::cout
      <<  std::endl 
      <<  " IDigits<0,1>     " << IDigits<unsigned short,65535,0,1>::value << std::endl 
      <<  " IDigits<0,2>     " << IDigits<unsigned short,65535,0,2>::value  << std::endl 
      <<  " IDigits<0,3>     " << IDigits<unsigned short,65535,0,3>::value  << std::endl 
      <<  " IDigits<0,4>     " << IDigits<unsigned short,65535,0,4>::value  << std::endl 
      <<  " IDigits<0,5>     " << IDigits<unsigned short,65535,0,5>::value  << std::endl 
      ;
  }
  
  // ==========================================================================
  exit(0) ;                                                             // EXIT
  // ==========================================================================
}    
// ============================================================================
// The END 
// ============================================================================
