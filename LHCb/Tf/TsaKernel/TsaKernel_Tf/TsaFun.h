// $Id: TsaFun.h,v 1.1 2007-09-16 09:44:45 mneedham Exp $
#ifndef _TsaFun_H_
#define _TsaFun_H_

#include "gsl/gsl_math.h"

namespace Tf
{
  namespace Tsa
  {

    inline double estimateCurvature(const double parabCurvature, const double cnvFactor)
    {

      const double curvInMeV = cnvFactor*parabCurvature;

      // upper p limit
      double pInMeV = TsaConstants::pMax;
      if ( fabs(1.0/curvInMeV) < pInMeV ){
        pInMeV = fabs(1.0/curvInMeV);
      }

      // lower limit
      pInMeV  = GSL_MAX(pInMeV, TsaConstants::pMin);

      return ((double)GSL_SIGN(curvInMeV))/pInMeV;
    }

  }
}

#endif
