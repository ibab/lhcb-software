// $Id: GenCounters.h,v 1.1 2005-12-07 23:00:09 robbep Exp $
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
  inline double fraction( const unsigned int A , const unsigned int B ) {
    return ( (double) A / (double) B ) ;
  }
  
  inline double err_fraction( const unsigned int A , const unsigned int B ) {
    return sqrt( A * ( 1. - ( (double) A / (double) B ) ) ) / ( (double) B ) ;
  }
  
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
