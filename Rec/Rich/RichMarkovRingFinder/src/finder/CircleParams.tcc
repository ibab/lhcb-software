#ifndef LESTER_CIRCLE_PARAMS_TCC
#define LESTER_CIRCLE_PARAMS_TCC

#include "CircleParams.h"
#include "CirclePriors.h"
#include "CLHEP/Random/RandGauss.h"
#include "geometry/FragmentedCircle.h"

template<class Mode,const Mode & mode>
int Lester::CircleParams<Mode,mode>::_creationCount = 0;

template<class Mode,const Mode & mode>
double Lester::CircleParams<Mode,mode>::priorProbability() const {
  const Hep2Vector c=centre();
  const double r=radius();
  //const double mu=c.mu();
  
  const double part1 = 
    CirclePriorsT::priorProbabilityOfCentre(c);
  const double part2 =
    CirclePriorsT::priorProbabilityOfRadius(r);
  
  //std::cout << "JORR = " << part1 << " " <<part2<<std::endl;
  const double ans = part1*part2;
  //ans *= exponentialProb(mu,circleMuMuParameter);
  
  return ans;
};

template<class Mode,const Mode & mode>
void Lester::CircleParams<Mode,mode>::jokeSetRandom() {
  _centre = CirclePriorsT::sampleFromCircleCentreDistribution();
  //mu=RandExponential::shoot(circleMuMuParameter);
  _radius = CirclePriorsT::sampleFromCircleRadiusDistribution();
  setMeanNumberOfHitsBasedOnRadiusAndCentre();
};


template<class Mode,const Mode & mode>
Lester::CircleParams<Mode,mode> Lester::CircleParams<Mode,mode>::jitterSymm1() const {
  const Hep2Vector newcen = centre() + Hep2Vector(0.1 * mode.characteristicCircleRadius() * RandGauss::shoot(), 
                                                  0.1 * mode.characteristicCircleRadius() * RandGauss::shoot());
  const double trialnewrad( radius() + 0.1 * mode.characteristicCircleRadius() * RandGauss::shoot());
  const double newrad( (trialnewrad > 0) ? trialnewrad : radius() );
  //const double newmu = fabs(mu+0.1*RandGauss::shoot());

  return CircleParams(newcen, newrad);
};


template<class Mode,const Mode & mode>
void Lester::CircleParams<Mode,mode>::internallySetMeanNumberOfHitsGeometricallyCorrected() {
  static bool first=true;
  if (first) {
    std::cerr << "Could probably try to optimise access to AcceptanceClippedArcs in Lester::CircleParams as at the moment they are recalculated all over the place and never cached" << std::endl;
    first = false;
  };
  
  FragmentedCircle c(centre(), radius());
  c.  leftClip(mode.geometricThetaAcceptanceLeftBound()  );
  c. rightClip(mode.geometricThetaAcceptanceRightBound() );
  c.bottomClip(mode.geometricThetaAcceptanceBottomBound());
  c.   topClip(mode.geometricThetaAcceptanceTopBound()   );
  const double totalSubtendedAngleInAcceptance = c.totalSubtendedAngle();
  const double acceptanceFraction = totalSubtendedAngleInAcceptance / MathsConstants::twopi;

  const double muUncorrected = _mu;

  //std::cerr << "Found acceptance fraction of " << acceptanceFraction << std::endl;

  _muGeometricallyCorrected = muUncorrected * acceptanceFraction;
};

#endif
