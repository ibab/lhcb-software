// $Id: pow.cpp,v 1.1 2008-08-01 10:16:02 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/ChronoEntity.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/IPower.hpp"
#include "LHCbMath/Power.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
#include "boost/assert.hpp"
// ============================================================================
/** @file
 *  Set of simple tests for various"pow" functions 
 *
 *  @see Gaudi::Math::pow
 *  @see Gaudi::Math::IPower
 *
 *  It also illustrate some CPU performance:
 * 
 *  On Linux:
 *
 *    - std::pow(double,int)                  
 *      is 1000% faster than std::pow(double,"double")
 *    - Gaudi::Math::pow(double,unsigned int) 
 *      is 10%   faster than std::pow(double,int)
 *    - Gaudi::Math::pow(int,unsigned int)    
 *      is 30%   faster than std::pow("int",int)
 *    - Gaudi::Math::IPower<...,int,int>      
 *      is 500%  faster than std::Gaudi::Math::pow(int,int)
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-07-31
 */
// ============================================================================
int main()
{
  
  const size_t I = 10000 ;
  const size_t K =   100 ;
  const size_t J =    15 ;
  
  ChronoEntity counter1 ;
  ChronoEntity counter2 ;
  ChronoEntity counter3 ;
  
  const double base   = 1.123456 ;
  
  double value1 = 0 ;  
  double value2 = 0 ;
  double value3 = 0 ;
  
  { // test with std::pow 
    for ( unsigned int i = 0 ; i < I ; ++i ) 
    {
      Chrono _cnt1 ( &counter1 ) ;
      for ( unsigned int k = 0 ; k < K ; ++k )
      { 
        for ( unsigned int j = 0 ; j < J ; ++j ) 
        { value1 += std::pow ( base , (int) j ) ; }      // std::pow 
      }
    }    
  }
  { // test with std::pow 
    for ( unsigned int i = 0 ; i < I ; ++i ) 
    {
      Chrono _cnt2 ( &counter2 ) ;
      for ( unsigned int k = 0 ; k < K ; ++k )
      { 
        for ( unsigned int j = 0 ; j < J ; ++j ) 
        { value2 += std::pow ( base , (double) j ) ; }  // std::pow 
      }    
    }
  }
  { // test with Gaudi::Math::pow
    for ( unsigned int i = 0 ; i < I ; ++i ) 
    {
      Chrono _cnt3 ( &counter3 ) ;
      for ( unsigned int k = 0 ; k < K ; ++k )
      { 
        for ( unsigned int j = 0 ; j < J ; ++j ) 
        { value3 += Gaudi::Math::pow( base , j ) ; }  // Gaudi::Math::pow
      }    
    }
  }

  ChronoEntity counter4 ;
  ChronoEntity counter5 ;
  ChronoEntity counter6 ;
  
  long double   value4 = 0 ;
  unsigned long value5 = 0 ;
  unsigned long value6 = 0 ;
  
  { // test with std::pow
    for ( unsigned int i = 0 ; i < I ; ++i ) 
    {
      Chrono _cnt4 ( &counter4 ) ;
      for ( unsigned int k = 0 ; k < K ; ++k )
      { 
        for ( unsigned int j = 0 ; j < J ; ++j ) 
        { value4 += std::pow( 3. , 14 )  ; }            // std::pow
      }    
    }
  }
  { // test with Gaudi::Math::pow
    for ( unsigned int i = 0 ; i < I ; ++i ) 
    {
      Chrono _cnt5 ( &counter5 ) ;
      for ( unsigned int k = 0 ; k < K ; ++k )
      { 
        for ( unsigned int j = 0 ; j < J ; ++j ) 
        { value5 += Gaudi::Math::pow( 3 , 14 )  ; } // Gaudi::Math::pow
      }    
    }
  }
  { // test with Gaudi::Math::IPower
    for ( unsigned int i = 0 ; i < I ; ++i ) 
    {
      Chrono _cnt6 ( &counter6 ) ;
      for ( unsigned int k = 0 ; k < K ; ++k )
      { 
        for ( unsigned int j = 0 ; j < J ; ++j ) 
        { value6 += Gaudi::Math::IPower<unsigned long,3,14>::value ; } // IPower
      }    
    }
  }
  
  std::cout 
    << " std::pow(double,int)         " << value1        << std::endl
    << " \t\t : " << counter1 . outputUserTime ()        << std::endl 
    << " std::pow(double,double)      " << value2        << std::endl 
    << " \t\t : " << counter2 . outputUserTime ()        << std::endl 
    << " Gaudi::Math::pow(double,int) " << value3        << std::endl 
    << " \t\t : " << counter3 . outputUserTime ()        << std::endl 
    << " std::pow(3.,14)           "    << value4        << std::endl 
    << " \t\t : " << counter4 . outputUserTime ()        << std::endl 
    << " Gaudi::Math::pow(3,14)    "    << value5        << std::endl 
    << " \t\t : " << counter5 . outputUserTime ()        << std::endl 
    << " Gaudi::Math::IPower<3,14> "    << value6        << std::endl 
    << " \t\t : " << counter6 . outputUserTime ()        << std::endl 
    ;
  
  
  // ==========================================================================
  exit(0) ;                                                             // EXIT
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
