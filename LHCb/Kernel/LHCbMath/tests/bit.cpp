// $Id: bit.cpp,v 1.1 2008-08-01 10:16:02 ibelyaev Exp $
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
#include "LHCbMath/Bit.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
#include "boost/assert.hpp"
// ============================================================================
/** @file
 *  Set of simple tests for various "bit" functions 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-31
 */
// ============================================================================
int main()
{
  using namespace Gaudi::Math  ;
  
  {
    unsigned int value = 7 ;
    std::cout 
      <<  " Value       " << value  << std::endl  
      <<  " bit(0)      " << bit ( value ,   0 )  << std::endl 
      <<  " bit(1)      " << bit ( value ,   1 )  << std::endl 
      <<  " bit(2)      " << bit ( value ,   2 )  << std::endl 
      <<  " bit(3)      " << bit ( value ,   3 )  << std::endl 
      <<  " bit(4)      " << bit ( value ,   4 )  << std::endl 
      <<  " bit(5)      " << bit ( value ,   5 )  << std::endl 
      <<  " bit(6)      " << bit ( value ,   6 )  << std::endl 
      <<  " bit(7)      " << bit ( value ,   7 )  << std::endl 
      <<  " bit(64)     " << bit ( value ,  64 )  << std::endl 
      <<  " bit(200)    " << bit ( value , 200 )  << std::endl 
      ;
  }
  
  { 
    unsigned int value = 7 ;
    
    Bit<unsigned int,0>   bit0 ;
    Bit<unsigned int,1>   bit1 ;
    Bit<unsigned int,2>   bit2 ;
    Bit<unsigned int,3>   bit3 ;
    Bit<unsigned int,4>   bit4 ;
    Bit<unsigned int,5>   bit5 ;
    Bit<unsigned int,6>   bit6 ;
    Bit<unsigned int,7>   bit7 ;
    Bit<unsigned int,64>  bit64 ;
    Bit<unsigned int,200> bit200 ;
    //
    std::cout 
      <<  " Bit<0>      " << bit0   ( value )  << std::endl 
      <<  " Bit<1>      " << bit1   ( value )  << std::endl 
      <<  " Bit<2>      " << bit2   ( value )  << std::endl 
      <<  " Bit<3>      " << bit3   ( value )  << std::endl 
      <<  " Bit<4>      " << bit4   ( value )  << std::endl 
      <<  " Bit<5>      " << bit5   ( value )  << std::endl 
      <<  " Bit<6>      " << bit6   ( value )  << std::endl 
      <<  " Bit<7>      " << bit7   ( value )  << std::endl 
      <<  " Bit<64>     " << bit64  ( value )  << std::endl 
      <<  " Bit<200>    " << bit200 ( value )  << std::endl 
      ;  
  }
  
  {
    std::cout 
      <<  " IBit<7,0>   " << IBit<unsigned int,7,0>  ::value << std::endl 
      <<  " IBit<7,1>   " << IBit<unsigned int,7,1>  ::value << std::endl 
      <<  " IBit<7,2>   " << IBit<unsigned int,7,2>  ::value << std::endl 
      <<  " IBit<7,3>   " << IBit<unsigned int,7,3>  ::value << std::endl 
      <<  " IBit<7,4>   " << IBit<unsigned int,7,4>  ::value << std::endl 
      <<  " IBit<7,5>   " << IBit<unsigned int,7,5>  ::value << std::endl 
      <<  " IBit<7,6>   " << IBit<unsigned int,7,6>  ::value << std::endl 
      <<  " IBit<7,7>   " << IBit<unsigned int,7,7>  ::value << std::endl 
      <<  " IBit<7,64>  " << IBit<unsigned int,7,64> ::value << std::endl 
      <<  " IBit<7,200> " << IBit<unsigned int,7,200>::value << std::endl 
      ;  
  }

  // ==========================================================================
  exit(0) ;                                                             // EXIT
  // ==========================================================================
}    
// ============================================================================
// The END 
// ============================================================================
