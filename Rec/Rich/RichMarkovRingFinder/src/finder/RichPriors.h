#ifndef RICHMARKOV_RICH_PRIORS_H
#define RICHMARKOV_RICH_PRIORS_H

// GSL
#include "gsl/gsl_math.h"

// Local
#include "Constants.h"
#include "probabilityUtilities/ProbabilityUtils.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "Hit.h"
#include "CircleParams.h"

namespace RichMarkov {

  template <class Mode, const Mode & mode>
  class RichPriors {

  public:
    typedef CircleParams<Mode,mode> CircleParamsT;

  public:

    static inline int sampleFromNumberOfCirclesDistribution() {
      return RandPoisson::shoot( mode.meanNumberOfRings() );
    }

    static inline double priorProbabilityOfNumberOfCircles(const int n) {
      return poissonProb(n, mode.meanNumberOfRings() );
    }

    static inline int sampleFromNumberOfHitsDueToCircle(const CircleParamsT & cp) {
      return RandPoisson::shoot(cp.meanNumberOfHits());
    }
    // probability counterpart not yet implemented

    static inline int sampleFromNumberOfHitsDueToBackground() {
      return RandPoisson::shoot( mode.backgroundMeanParameter() );
    }
    // probability counterpart not yet implemented

    static inline Hit sampleHitDueToBackground() {
      const double x = RandGauss::shoot(0, mode.backgroundRadius() );
      const double y = RandGauss::shoot(0, mode.backgroundRadius() );
      const Hit h(x,y);
      return h;
    }

    static inline double priorProbabilityOfHitDueToBackground(const Hep2Vector & p) {
      const double sigSq = mode.backgroundRadius() * mode.backgroundRadius();
      const double rSqOnSigSq = p.mag2()/sigSq;
      const double half = 0.5;
      const double one = 1;
      const double ans = (one / (MathsConstants::twopi*sigSq)) * exp(-half*rSqOnSigSq);
      return ans;
    }

    static inline Hit sampleHitDueToCircle(const CircleParamsT & c)
    {
      const double theta    = RandFlat::shoot(0.0 ,MathsConstants::twopi);
      const double r0       = c.radius();
      const double epsilon  = Constants::circleProbabilityDistributionEpsilonParameter;
      // the maths of the next bit depends on alpha being 2 (see Lester lab book 5) so:
      assert (Constants::circleProbabilityDistributionAlphaParameter == 2);
      const double t        = RandGauss::shoot(log(r0),epsilon);
      const double rWobbled = exp(t);
      const double x        = rWobbled * cos(theta) + c.centre().x();
      const double y        = rWobbled * sin(theta) + c.centre().y();
      return Hit(x,y);
    }

    // First version from lester
    static inline double priorProbabilityOfHitDueToCircle( const Hep2Vector & p,
                                                           const CircleParamsT & cp )
    {
      if ( Constants::lesterFirst == Constants::probFuncType ) {

        //const double rSq     = (p-cp.centre()).mag2();
        const Hep2Vector & cen = cp.centre();
        const double deltax = p.x()-cen.x();
        const double deltay = p.y()-cen.y();
        const double rSq = deltax*deltax + deltay*deltay;
	
        const double r0      = cp.radius();
        const double r0Sq    = r0*r0;
        const double rOnR0Sq = rSq/r0Sq;

        if (false /* rOnR0 > 1.3  || rOnR0 < 0.3 */) {
          // This can be used as a sanity cut on distance from the "active area" of 
          // the function to avoid piddling around with tiny exponentials which are
          // basically equal to zero
          return 0;
        } else {

          // Alpha and epsilon parameters correct for the Gaussian width and the skewing
          // due to evaluating in log space
          const double alpha   = Constants::circleProbabilityDistributionAlphaParameter;
          const double alphaSq = alpha*alpha;
          const double epsilon = Constants::circleProbabilityDistributionEpsilonParameter;
          const double epsSq   = epsilon*epsilon;
          const double logg    = 0.5*log(rOnR0Sq); // hopefully the same as old val:  log(rOnR0);
          const double logsq   = logg*logg;
          const double two     = 2;

          // This corrects for some sort of probability peaking in the centre of the
          // Yorkshire Pudding function since the Gaussian is parameterised in log space
          // to ensure it is well-defined, since [0, infty] in r -> [-infty, infty] in log r.
          const double substituteForPow = ( alpha == 2 ? 1 : pow(rOnR0Sq,0.5*(alpha-two)) );
          return ( 1.0/(MathsConstants::twopi) / sqrt(MathsConstants::twopi*epsSq)*
                   substituteForPow*exp(-0.5*(logsq/epsSq + alphaSq*epsSq )) / (rSq) );
        }

      } else if ( Constants::topHat == Constants::probFuncType ) {

        const double r0  = cp.radius();
        const double r   = (p-cp.centre()).mag();
        const double dr  = r-r0;
        const double eps = 4. * ( mode.characteristicCircleRadius() *
                                  Constants::circleProbabilityDistributionEpsilonParameter );
        // full
        //const double ep1 = (1./6.)*(3.*eps + 3.*r0 - sqrt( 3.*(3.*r0*r0-eps*eps) ));
        // taylor expand sqrt
        const double ep1 = eps * ( 0.5 + eps/(12.*r0) );
        const double ep2 = eps-ep1;
        return ( dr<-ep1 || dr>ep2 ? 0 : 2./(MathsConstants::twopi*eps*(eps+2.*(r0-ep1))) );

      } else if ( Constants::power1 == Constants::probFuncType ) {

        const double eps = 10. *( mode.characteristicCircleRadius() *
                                 Constants::circleProbabilityDistributionEpsilonParameter );
        const double norm = 2.*eps*eps/3.;
        const double r0  = cp.radius();
        const double r   = (p-cp.centre()).mag();
        const double dr  = r - r0;
        return ( fabs(dr) > eps ? 0 :
                 ( dr<0 ? gsl_pow_2(dr+eps)/(norm*r) : gsl_pow_2(eps-dr)/(norm*r) ) );

      }

    }

    static inline bool hitLiesInAcceptanceCrude( const Hit & hit ) {
      // returns true if the hit lands on the sensitive part of the detector and false otherwise.
      // This "crude" version assumes that there are no gaps _between_ photo detectors
      const double x = hit.x();
      const double y = hit.y();
      return ( x >= mode.geometricThetaAcceptanceLeftBound()   &&
               x <= mode.geometricThetaAcceptanceRightBound()  &&
               y >= mode.geometricThetaAcceptanceBottomBound() &&
               y <= mode.geometricThetaAcceptanceTopBound() );
    }

    //static inline double circleGeometricAcceptanceCrude(const CircleParamsT & circ) {
    //  // returns a number bewteen 0 and 1 indicating what fraction of the ring lands on photo-detectors
    //  return 1;
    //};

  };

}

#endif
