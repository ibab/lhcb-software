// $Id: LHCbMath.h,v 1.1 2004-03-02 10:33:18 cattanem Exp $
#ifndef KERNEL_LHCBMATH_H 
#define KERNEL_LHCBMATH_H 1

// Include files

/** @namespace LHCbMath
 *  
 *  Collection of math related functions for general use in LHCb
 *
 *  @author Marco Cattaneo
 *  @date   2004-03-02
 */

namespace LHCbMath 
{

  inline int round(const double x)  {
  // Round to nearest integer. Rounds half integers to the nearest even integer.
  // Provided by Matt Needham 
    int i;
    if (x >= 0.0) {
      i = int(x + 0.5);
      if (x + 0.5 == double(i) && i & 1) i--;
    }
    else {
      i = int(x - 0.5);
      if (x - 0.5 == double(i) && i & 1) i++;
      
    }
    return i;
  }

  inline int round(const float x)  {
    return LHCbMath::round( double(x) );
  }
  
};

#endif // KERNEL_LHCBMATH_H
