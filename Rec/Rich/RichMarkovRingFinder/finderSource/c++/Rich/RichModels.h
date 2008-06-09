
#ifndef LESTER_RICH_MODELS_H
#define LESTER_RICH_MODELS_H

#include "FiniteRelativeProbabilityChooser.h"
#include "RegularProbabilityDistribution1D.h"
#include "ProbabilityAbove.h"

namespace Lester 
{

  typedef FiniteRelativeProbabilityChooser<double> Rich2AThetaSampler;
  typedef RegularProbabilityDistribution1D<double> Rich2AThetaDistribtion;
  typedef ProbabilityAbove<double>                 Rich2AProbabilityThetaAbove;

  extern const Rich2AThetaSampler          rich2AThetaSampler;
  extern const Rich2AThetaDistribtion      rich2AThetaDistribution;
  extern const Rich2AProbabilityThetaAbove rich2AProbabilityThetaAbove;
}

#endif
