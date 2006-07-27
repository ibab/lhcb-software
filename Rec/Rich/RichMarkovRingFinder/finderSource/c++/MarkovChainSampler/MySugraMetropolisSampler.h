
#ifndef LESTER_MY_SUGRA_METROPOLIS_SAMPLER_H
#define LESTER_MY_SUGRA_METROPOLIS_SAMPLER_H

#include "MarkovChainSampler/MetropolisSampler.h"
#include "CLHEP/Random/RandGauss.h"
#include "SugraSpace.h"
#include "SoftSusyInterface.h"

namespace Lester {

  class MySugraMetropolisSampler : 
    public MetropolisSampler<SugraSpace::PointType> {
  public:
    //typedef SugraSpace SpaceType;
    typedef SugraSpace::PointType PointType;
    MySugraMetropolisSampler(const PointType & initialPoint) :
      MetropolisSampler<PointType> (initialPoint) {
    };
    double targetDistribution(const PointType & point) const {
      callSoftsusy();
      if (point.mZero()>1 && point.mHalf()>1 && point.tanBeta()>0.5 ) {
	return 1./(point.mZero()/100. + point.mHalf()/100.+fabs(point.A0())/100+point.tanBeta()/10.+4.);
      } else {
	return 0;
      };
    };
    double probOfSuggestingFirstGivenLast(const PointType & suggestedPoint,
					  const PointType & lastPoint) const {
      //      const double width=2;
      //const double thing=(lastPoint[0]-suggestedPoint[0])/width;
      //const double thingSq=thing*thing;
      return 1.; // approx!
     };
  private:
    inline double jitterFactor(const double fractionalWidth=0.1) const {
      const double ans = RandGauss::shoot(0.,fractionalWidth);
      //while ( (ans=RandGauss::shoot(0.,fractionalWidth)+1)<=0 ) {};
      //std::cout << "ans="<<ans<<std::endl;
      return exp(ans);
    };
  public:    
    PointType suggestNewPoint() const {
      const double jitterFac=0.1;
      const double newMZero = (*this)->mZero() * jitterFactor(jitterFac); 
      const double newMHalf = (*this)->mHalf() * jitterFactor(jitterFac); 
      const double newA0 = (*this)->A0() + RandGauss::shoot(0,(fabs((*this)->A0())+100.)*jitterFac); 
      const double newTanBeta = (*this)->tanBeta() * jitterFactor(jitterFac); 
      const int newSgnMu = ((RandFlat::shoot()<jitterFac)?-1:+1)*(*this)->sgnMu(); 
      return PointType(newMZero,
		       newMHalf,
		       newA0,
		       newTanBeta,
		       newSgnMu);
   };
  };
  
};

#endif
