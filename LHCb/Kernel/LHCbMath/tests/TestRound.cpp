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
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/ChronoEntity.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Lomont.h"
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// Boost 
// ============================================================================
using namespace std;
#ifdef __INTEL_COMPILER         // Disable ICC remark from Boost
  #pragma warning(disable:1572) // floating-point equality and inequality comparisons are unreliable
#endif
#include "boost/progress.hpp"
// ============================================================================
/** @file 
 *  The test file for comparison of roundings 
 *   - LHCb::Math::round, based on Knuth's  compare
 *   - LHCb::Math::round, based on Lomont's compare 
 *   - boost::numeric - based 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-10-22
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  inline int roundOld ( const double x )  
  {
    int i;
    if (x >= 0.0) {
      i = int(x + 0.5);
      if (LHCb::Math::knuth_equal_to_double(x + 0.5, double(i)) && i & 1) --i;
    }
    else {
      i = int(x - 0.5);
      if (LHCb::Math::knuth_equal_to_double(x - 0.5 , double(i)) && i & 1) ++i;
      
    }
    return i;
  }
  // ==========================================================================
  inline int roundLomont ( const double x )  
  {
    int i;
    if (x >= 0.0) {
      i = int(x + 0.5);
      if ( LHCb::Math::lomont_compare_double (x + 0.5, double(i) , 500 ) && i & 1) --i;
    }
    else {
      i = int(x - 0.5);
      if ( LHCb::Math::lomont_compare_double (x - 0.5 , double(i), 500 ) && i & 1) ++i;
      
    }
    return i;
  }
  // ==========================================================================
  typedef boost::numeric::make_converter_from
  <double,
   boost::numeric::silent_overflow_handler  ,
   boost::numeric::RoundEven<double> > ::to<int>::type Double2Int ;
  // =========================================================================
} //                                                 end of anonymous namespace
// ============================================================================
int main() 
{
  //
  long bad = 0 ;
  for ( int i = 0 ; i < 10000 ; ++i ) 
  {
    const double v = sin( double(i) ) * exp( 8 * cos( double(i) ) ) ;
    
    long i1 = roundOld            ( v ) ;
    long i2 = roundLomont         ( v ) ;
    long i3 = LHCb::Math::round   ( v ) ;
    long i4 = LHCb::Math::round   ( static_cast<float>(v) ) ;
    
    if ( i1 != i2 || i2 != i3 || i3 != i4 || 
         i1 != i3 || i2 != i4 ||
         i1 != i4                          ) 
    { 
      ++bad ;
      std::cout 
        << i1 << " " << i2 << " " << i3 << "  " << i4 
        << " " << v << std::endl ; 
    } 
    
  }
  
  std::cout << " Differences: " << bad << std::endl ;
  
 
  ChronoEntity counter1 ;
  ChronoEntity counter2 ;
  ChronoEntity counter3 ;
 
  double value1 = 0 ;
  double value2 = 0 ;
  double value3 = 0 ;

  const unsigned int nMax = 1000 ;
  
  boost::progress_display show_progress ( nMax );
  for ( unsigned int m = 0 ; m < nMax ; ++m )
  {
    const double v = sin( double(m) ) * exp( 8 * cos( double(m) ) ) ;
    {  // LHCb::Math::round 
      for ( int j = 0 ; j < 500 ; ++j ) 
      {
        Chrono cnt ( &counter1 ) ;
        for ( int k = 0 ; k < 500 ; ++k ) 
        {
          value1 += roundOld ( v ) + ( j - k ) ;
        }
      }
    }
    {  // LHCb::Math::round using lomont 
      for ( int j = 0 ; j < 500 ; ++j ) 
      {
        Chrono cnt ( &counter2 ) ;
        for ( int k = 0 ; k < 500 ; ++k ) 
        {
          value2 += roundLomont ( v ) + ( j - k ) ;
          
        }
      }
    }
    {  // boost::numeric 
      for ( int j = 0 ; j < 500 ; ++j ) 
      {
        Chrono cnt ( &counter3 ) ;
        for ( int k = 0 ; k < 500 ; ++k ) 
        {
          value3 += LHCb::Math::round( v ) + ( j - k ) ;
        }
      }
    }
    ++show_progress ;
  }
  
  std::cout 
    << " round-Knuth     " << value1 << std::endl 
    << " \t\t"             << counter1.outputUserTime() << std::endl 
    << " round&Lomont    " << value2 << std::endl 
    << " \t\t"             << counter2.outputUserTime() << std::endl 
    << " round-New       " << value3 << std::endl 
    << " \t\t"             << counter3.outputUserTime() << std::endl ;
  
  return 0 ;
}

// ============================================================================
// The END 
// ============================================================================
