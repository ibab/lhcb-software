
//-----------------------------------------------------------------------------
/** @file FastMaths.h
 *
 * Implementation of vaious fast mathematical functions for the Rich
 *
 * CVS Log :-
 * $Id: FastMaths.h,v 1.2 2007-09-10 13:27:01 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 02/09/2007
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_FASTMATHS_H
#define RICHKERNEL_FASTMATHS_H 1

#include <cmath>

namespace Rich
{
  namespace Maths
  {

    /** Fast (approx) implementation of atan2(x,y) using
     *  power series expansions
     *  @param y
     *  @param x
     *  @return Approximate calculation of atan2(y,x)
     */
    inline double atan2_f( const double y, const double x )
    {
      double z(0);
      if ( fabs(x) < 1e-10 )
      {
        z = ( y<0 ? -M_PI_2 : y>0 ? M_PI_2 : 0 );
      }
      else
      {
        z = y/x;
        if ( z>=-1 && z<=1 )
        {
          z = z*(15.0+4.0*z*z)/(15.0+9.0*z*z);
          if ( x<=0 ) z = ( y>=0 ? M_PI+z : z-M_PI );
        }
        else
        {
          z = M_PI_2-(15.0+4.0/(z*z))/(z*(15.0+9.0/(z*z)));
          if ( y<=0 ) z -= M_PI;
        }
      }
      return z;
    }

  }
}

/*
  #define INT_TO_FIXED(x)     ((x) << 16)
  #define FIXED_ONE       INT_TO_FIXED(1)
  #define FIXED_PIDIV2            102944L

  int32 fpatan(int32 x)
  {
  register int32 x2;
  register int32 numerator;
  register int32 denominator;
  if (x > FIXED_ONE)
  {
  return FIXED_PIDIV2 - fpatan(fpdiv(FIXED_ONE, x));
  }
  if (x < -FIXED_ONE)
  {
  return -FIXED_PIDIV2 - fpatan(fpdiv(FIXED_ONE, x));
  }
  x2 = fpmul(x, x);
  numerator = fpmul(x, INT_TO_FIXED(15) + fpmul(INT_TO_FIXED(4), x2));
  denominator = INT_TO_FIXED(15) + fpmul(INT_TO_FIXED(9), x2);
  return fpdiv(numerator, denominator);
  }
*/

#endif // RICHKERNEL_FASTMATHS_H
