
#ifndef LESTER_RICH_PRIORS_H
#define LESTER_RICH_PRIORS_H

#include "Constants.h"
#include "ProbabilityUtils.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "Hit.h"
#include "CircleParams.h"

namespace Lester {

  class RichPriors {
  public:

    static inline int sampleFromNumberOfCirclesDistribution() {
      return RandPoisson::shoot(Constants::meanNumberOfRings);
    };
    static inline double priorProbabilityOfNumberOfCircles(const int n) {
      return poissonProb(n, Constants::meanNumberOfRings);
    };


    static inline int sampleFromNumberOfHitsDueToCircle(const CircleParams & cp) {
      return RandPoisson::shoot(cp.mu());
    };
    // probability counterpart not yet implemented

    static inline int sampleFromNumberOfHitsDueToBackground() {
      return RandPoisson::shoot(Constants::backgroundMeanParameter);
    };
    // probability counterpart not yet implemented

    static inline Hit sampleHitDueToBackground() {
      const double x = RandGauss::shoot(0,Constants::backgroundRadius);
      const double y = RandGauss::shoot(0,Constants::backgroundRadius);
      const Hit h(x,y);
      return h;
    };
    static inline double priorProbabilityOfHitDueToBackground(const Hep2Vector & p) {
      const double sigSq = Constants::backgroundRadius*Constants::backgroundRadius;
      const double rSqOnSigSq = p.mag2()/sigSq;
      const double half = 0.5;
      const double one = 1;
      const double ans = (one/(MathsConstants::twoPi*sigSq))*exp(-half*rSqOnSigSq);
      return ans;
    };



    static inline Hit sampleHitDueToCircle(const CircleParams & c) {
      const double theta=RandFlat::shoot(0.,MathsConstants::twoPi);
      const double r0=c.radius();
      const double epsilon=Constants::circleProbabilityDistributionEpsilonParameter;
      // the maths of the next bit depends on alpha being 2 (seelab book 5) so:
      assert (Constants::circleProbabilityDistributionAlphaParameter==2);
      const double t = RandGauss::shoot(log(r0),epsilon);
      const double rWobbled = exp(t);
      const double x=rWobbled*cos(theta)+c.centre().x();
      const double y=rWobbled*sin(theta)+c.centre().y();
      const Hit ans(x,y);
      return ans;
    };
    static inline double priorProbabilityOfHitDueToCircle(const Hep2Vector & p, const CircleParams & cp) {
      const double alpha = Constants::circleProbabilityDistributionAlphaParameter;
      const double alphaSq = alpha*alpha;
      const double epsilon = Constants::circleProbabilityDistributionEpsilonParameter;
      const double epsSq = epsilon*epsilon;
      const double r=(p-cp.centre()).mag();
      const double r0=cp.radius();
      const double logg = log(r/r0);
      const double logsq = logg*logg;
      const double two = 2;
      const double substituteForPow = (alpha==2?1:pow(r/r0,alpha-two)); 
      const double ans = 1./(MathsConstants::twoPi)/sqrt(MathsConstants::twoPi*epsSq)*substituteForPow*exp(-0.5*(logsq/epsSq + alphaSq*epsSq  )) / (r*r);
      return ans;
    };
    

  };

};

#endif
