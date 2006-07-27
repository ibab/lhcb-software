
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
      //const typename Space::TPointType off = typename Space::TPointType();
      //return 
	//1./sqrt(2*3.1*1)*exp(-0.5*((point+off).modSq()/0.25)) + 
	//point.modSq()<9?1:0;
	//exp(-point.modSq())*pow(sin(point.mod()*3.1),2);
      if (point[0]>0.5 && point[0]<4) {
	return log(2);
      } else if (point[0]>=4 && point[0]<8) {
	return log(1);
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
	//};
      //if (fabs(t[0])>1000000.) {
      //std::cout << "strange t[0]="<<t[0]<<" **this="<<**this<<" wid="<<width<<std::endl;
      //};
      //return t+(**this);
    };
    double logProbOfSuggestingFirstGivenLast(const TPointType & suggestedPoint,
					   const TPointType & lastPoint) const {
      //return 1;
      //return (1./sqrt(2*3.1*pow((lastPoint.modSq()+0.3),2)))*exp(-0.5*pow((  (suggestedPoint[0]-lastPoint[0])/(lastPoint.modSq()+0.3)  ),2));

      //const double width=1./(lastPoint.mod()+0.3);
      //const double width=naughtyGlobalWidth;
      //const double thing=(lastPoint[0]-suggestedPoint[0])/width;
      //const double thingSq=thing*thing;
      //return 1./sqrt(2.*3.1)/width * exp(-0.5*thingSq);

      return log(1./suggestedPoint[0]);

      /*
      const double width=lastPoint.modSq()+0.3;
      if (fabs(suggestedPoint[0]-lastPoint[0])<width) {
	return 1./width;
      } else {
	return 0;
      };
      */

     };
  };
  
};

#endif
