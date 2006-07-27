
#ifndef LESTER_MATHSFUNCTIONS_H
#define LESTER_MATHSFUNCTIONS_H

#include "Utils/MathsConstants.h"

namespace MathsFunctions {

  // Dimesion D referrs to the dimension of the EMBEDDING SPACE not the dimension inttrinsic to the surface itself == eg returns 2Pi when N=1. 4Pi whem N=2 etc.
  double surfaceAreaOfUnitSphereInDimesion(unsigned int D) {
    const double dOver2=static_cast<double>(D)/2.0;
    return 2.*pow(MathsConstants::pi,dOver2)/tgamma(dOver2);
  };
  
}

#endif
