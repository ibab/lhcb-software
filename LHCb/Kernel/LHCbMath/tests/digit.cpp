// $Id: digit.cpp,v 1.1 2008-08-01 10:16:02 ibelyaev Exp $
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
    Digit<unsigned int,64>  bit64 ;
    Digit<unsigned int,200> bit200 ;
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
      <<  " Digit<64>       " << bit64  ( value )  << std::endl 
      <<  " Digit<200>      " << bit200 ( value )  << std::endl 
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
      <<  " IDigit<724,64>  " << IDigit<unsigned int,724,64> ::value << std::endl 
      <<  " IDigit<724,200> " << IDigit<unsigned int,724,200>::value << std::endl 
      ;  
  }
  
  // ==========================================================================
  exit(0) ;                                                             // EXIT
  // ==========================================================================
}    
// ============================================================================
// The END 
// ============================================================================
