
#ifndef RICHMARKOV_MATHS_CONSTANTS_H
#define RICHMARKOV_MATHS_CONSTANTS_H

#include "gsl/gsl_math.h"

namespace RichMarkov {

  namespace MathsConstants {

    const double      pi = 3.141592653589793238462643383279502884197169399375105820974944;
    const double   twopi = pi*2.0;
    const double piontwo = pi*0.5;
    const double  sqrtPi = 1.772453850905516027298167483341145182797549456122387128213807;
    
  }

  inline const int finite( const double x )
  {
	return gsl_finite(x);
  }

}

#endif
