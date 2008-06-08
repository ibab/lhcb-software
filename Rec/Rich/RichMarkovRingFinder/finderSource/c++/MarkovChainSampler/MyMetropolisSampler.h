
#ifndef LESTER_MY_METROPOLIS_SAMPLER_H
#define LESTER_MY_METROPOLIS_SAMPLER_H

#include "MarkovChainSampler/MetropolisSampler.h"
#include "CLHEP/Random/RandGauss.h"

namespace Lester {

  template <class TPointType>
  class MyMetropolisSampler :
    public MetropolisSampler<TPointType> {
  public:
    typedef TPointType PointType;
    MyMetropolisSampler(const TPointType & initialPoint) :
      MetropolisSampler<TPointType> (initialPoint) {
    };
    double logTargetDistribution(const TPointType & point) const {
      if (point[0]>0.5 && point[0]<4) {
        return std::log(2);
      } else if (point[0]>=4 && point[0]<8) {
        return std::log(1);
      } else {
        throw LogarithmicTools::LogOfZero();
      };
    };
    TPointType suggestNewPoint() const {
      //typename Space::TPointType t=typename Space::TPointType();
      //const double width=(RandFlat::shoot()<0.001)?5:0.5;
      //const double width=1./((**this).mod()+0.3);
      const double width=0.2 ;
      //for (int i=0; i<t.size(); i++) {
      //t[i]=RandFlat::shoot(-width,width);
      const double logShift = RandGauss::shoot(0.,width);

      const double presentLog = log((**this)[0]);
      const double newLog = presentLog + logShift;
      const double newPos = exp(newLog);
      return TPointType(newPos);
    };
    double logProbOfSuggestingFirstGivenLast(const TPointType & suggestedPoint,
                                             const TPointType & lastPoint) const 
    {
      return std::log(1./suggestedPoint[0]);
    };
  };

};

#endif
