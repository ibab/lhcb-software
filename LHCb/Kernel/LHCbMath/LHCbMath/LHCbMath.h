
//---------------------------------------------------------------------------------
/** @file LHCbMath.h
 *
 *  Collection of math related functions for general use in LHCb
 *
 *  CVS Log :-
 *  $Id: LHCbMath.h,v 1.1 2006-12-15 13:42:59 cattanem Exp $
 *
 *  @author Juan PALACIOS
 *  @date   2005-11-21
 */
//---------------------------------------------------------------------------------

#ifndef LHCBMATH_LHCBMATH_H 
#define LHCBMATH_LHCBMATH_H 1

// Include files

/** @namespace LHCbMath LHCbMath.h LHCbMath/LHCbMath.h 
 *  
 *  Collection of math related functions for general use in LHCb
 *
 *  @author Marco Cattaneo
 *  @date   2004-03-02
 */

namespace LHCbMath 
{
  // Parameters for numerical calculations (M.Needham)
  static const double hiTolerance = 1e-40;
  static const double lowTolerance = 1e-20;
  static const double looseTolerance = 1e-5;
  static const double     sqrt_12 = 3.4641016151377546; // sqrt(12.)
  static const double inv_sqrt_12 = 0.2886751345948129; // 1./sqrt(12.)
 
  /// Round to nearest integer. Rounds half integers to the nearest even integer.
  /// Provided by Matt Needham 
  inline int round(const double x)  {
    int i;
    if (x >= 0.0) {
      i = int(x + 0.5);
      if (x + 0.5 == double(i) && i & 1) --i;
    }
    else {
      i = int(x - 0.5);
      if (x - 0.5 == double(i) && i & 1) ++i;

    }
    return i;
  }

  inline int round(const float x)  {
    return LHCbMath::round( double(x) );
  }
  
};

#endif // LHCBMATH_LHCBMATH_H
