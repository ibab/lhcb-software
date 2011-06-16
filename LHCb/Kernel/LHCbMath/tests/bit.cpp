// $Id$
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
      <<  std::endl 
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
    //
    std::cout 
      <<  std::endl 
      <<  " Bit<0>      " << bit0   ( value )  << std::endl 
      <<  " Bit<1>      " << bit1   ( value )  << std::endl 
      <<  " Bit<2>      " << bit2   ( value )  << std::endl 
      <<  " Bit<3>      " << bit3   ( value )  << std::endl 
      <<  " Bit<4>      " << bit4   ( value )  << std::endl 
      <<  " Bit<5>      " << bit5   ( value )  << std::endl 
      <<  " Bit<6>      " << bit6   ( value )  << std::endl 
      <<  " Bit<7>      " << bit7   ( value )  << std::endl 
      ;  
  }
  
  {
    std::cout 
      <<  std::endl 
      <<  " IBit<7,0>   " << IBit<unsigned int,7,0>  ::value << std::endl 
      <<  " IBit<7,1>   " << IBit<unsigned int,7,1>  ::value << std::endl 
      <<  " IBit<7,2>   " << IBit<unsigned int,7,2>  ::value << std::endl 
      <<  " IBit<7,3>   " << IBit<unsigned int,7,3>  ::value << std::endl 
      <<  " IBit<7,4>   " << IBit<unsigned int,7,4>  ::value << std::endl 
      <<  " IBit<7,5>   " << IBit<unsigned int,7,5>  ::value << std::endl 
      <<  " IBit<7,6>   " << IBit<unsigned int,7,6>  ::value << std::endl 
      <<  " IBit<7,7>   " << IBit<unsigned int,7,7>  ::value << std::endl 
      ;  
  }
  
  
  {
    unsigned int value = 7 ;
    std::cout
      << std::endl
      << " Bits<0,1>   " << Bits<unsigned int,0,1>() ( value ) << std::endl 
      << " Bits<0,2>   " << Bits<unsigned int,0,2>() ( value ) << std::endl 
      << " Bits<0,3>   " << Bits<unsigned int,0,3>() ( value ) << std::endl 
      << " Bits<0,4>   " << Bits<unsigned int,0,4>() ( value ) << std::endl 
      << " Bits<0,5>   " << Bits<unsigned int,0,5>() ( value ) << std::endl 
      << " Bits<1,5>   " << Bits<unsigned int,1,5>() ( value ) << std::endl 
      << " Bits<2,5>   " << Bits<unsigned int,2,5>() ( value ) << std::endl 
      << " Bits<8,9>   " << Bits<unsigned int,8,9>() ( value ) << std::endl 
      ;
  }
  
  {
    std::cout 
      << std::endl 
      << " IBits<0,1>  " << IBits<unsigned int,7,0,1>::value << std::endl 
      << " IBits<0,2>  " << IBits<unsigned int,7,0,2>::value << std::endl 
      << " IBits<0,3>  " << IBits<unsigned int,7,0,3>::value << std::endl 
      << " IBits<0,4>  " << IBits<unsigned int,7,0,4>::value << std::endl 
      << " IBits<0,5>  " << IBits<unsigned int,7,0,5>::value << std::endl 
      << " IBits<1,5>  " << IBits<unsigned int,7,1,5>::value << std::endl 
      << " IBits<2,5>  " << IBits<unsigned int,7,2,5>::value << std::endl 
      << " IBits<8,9>  " << IBits<unsigned int,7,8,9>::value << std::endl 
      ;
  }

  {
    unsigned int value = 7 ;
    std::cout 
      << std::endl 
      << " Value       " << value  << std::endl  
      << " bits(0,1)   " << bits ( value ,   0 ,   1 )  << std::endl 
      << " bits(0,2)   " << bits ( value ,   0 ,   2 )  << std::endl 
      << " bits(0,3)   " << bits ( value ,   0 ,   3 )  << std::endl 
      << " bits(0,4)   " << bits ( value ,   0 ,   4 )  << std::endl 
      << " bits(0,5)   " << bits ( value ,   0 ,   5 )  << std::endl 
      << " bits(1,5)   " << bits ( value ,   1 ,   5 )  << std::endl 
      << " bits(2,5)   " << bits ( value ,   2 ,   5 )  << std::endl 
      << " bits(8,9)   " << bits ( value ,   8 ,   9 )  << std::endl 
      << " bits(0,64)  " << bits ( value ,   0 ,  64 )  << std::endl 
      << " bits(64,200)" << bits ( value ,  64 , 200 )  << std::endl 
      ;  
  }
  
  // ==========================================================================
  exit(0) ;                                                             // EXIT
  // ==========================================================================
}    
// ============================================================================
// The END 
// ============================================================================
