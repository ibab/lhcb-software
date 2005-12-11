// $Id: GenCounters.h,v 1.2 2005-12-11 23:21:47 robbep Exp $
#ifndef GENERATORS_GENCOUNTERS_H 
#define GENERATORS_GENCOUNTERS_H 1

// Include files
#include "GaudiKernel/MsgStream.h"

/** @namespace GenCounters
 *  
 *  Utility functions to print counters and compute efficiencies
 * 
 *  @author Patrick Robbe
 *  @date   2005-11-30
 */

namespace GenCounters {
  /** Compute fraction of two numbers A/B
   *  @param[in] A  Numerator 
   *  @param[in] B  Denominator
   *  @return A/B
   */
  inline double fraction( const unsigned int A , const unsigned int B ) {
    return ( (double) A / (double) B ) ;
  }
  
  /** Compute statistical error on fraction
   *  @param[in] A  Numberator
   *  @param[in] B  Denominator
   *  @return Error on A/B
   */
  inline double err_fraction( const unsigned int A , const unsigned int B ) {
    return sqrt( A * ( 1. - ( (double) A / (double) B ) ) ) / ( (double) B ) ;
  }
  
  /** Print efficiency computed with 2 counters.
   *  @param[in] theStream  Print facility to print the counters and efficiency
   *                        after/before.
   *  @param[in] cutName    Description of the cut
   *  @param[in] after      Number of events after the cut
   *  @param[in] before     Number of events before the cut
   */
  inline void printEfficiency( MsgStream & theStream , 
                               const std::string & cutName , 
                               const unsigned int after , 
                               const unsigned int before ) {
    if ( 0 == before ) return ;
    theStream << "Number of events before " << cutName << " : " << before 
              << std::endl ;
    theStream << "Number of events after " << cutName << " : " << after 
              << std::endl ;
    theStream << "Efficiency of the " << cutName << " : " 
              << format( "%.5g +/- %.5g" , fraction( after , before ) , 
                         err_fraction( after , before ) ) << std::endl ;
  }
  
  inline void printCounter( MsgStream & theStream , const std::string & name , 
                            const unsigned int value ) {
    theStream << "Number of " << name << " : " << value << std::endl ; 
  }
  
  /** Print fraction computed from two counters.
   *  @param[in] theStream  Print facility to print the counters and fraction
   *                        number/total.
   *  @param[in] name       Description of the fraction
   *  @param[in] number     Number of particles contained in the fraction
   *  @param[in] total      Total number of particles.
   */
  inline void printFraction( MsgStream & theStream , 
                             const std::string & name , 
                             const unsigned int number , 
                             const unsigned int total ) {
    theStream << "Number of " << name << " : " << number << std::endl ;
    theStream << "Fraction of " << name << " : " 
              << format( "%.5g +/- %.5g" , fraction( number , total ) ,
                         err_fraction( number , total ) ) << std::endl ;  
  }
};

#endif // GENERATORS_GENCOUNTERS_H
