
#ifndef RICHMARKOV_RICH_H
#define RICHMARKOV_RICH_H

#include "RichStuff.fwd"
#include "RichParams.fwd"
#include "Data.fwd"
#include "Alterations.fwd"

#include "CircleParams.h"
#include "CLHEP/Vector/TwoVector.h"

namespace RichMarkov {

  template <class Mode, const Mode & mode>
  class RichStuff {

  public:
    typedef Data<Mode, mode> DataT;
    typedef Alterations<Mode, mode> AlterationsT;
    typedef RichParams<Mode, mode>  RichParamsT;
    

    static double totalLogProb(const RichParamsT & rp, const DataT & d, const AlterationsT & a);
    //static double backgroundProbDist(const Hep2Vector & p) {
    //const double sigSq = backgroundRadius*backgroundRadius;
    //const double rSqOnSigSq = p.mag2()/sigSq;
    //const double half = 0.5;
    //const double one = 1;
    //const double two = 2;
    //const double ans = (one/(two*pi*sigSq))*exp(-half*rSqOnSigSq);
    //return ans;
    //};
    //static double circleProbDist(const Hep2Vector & p, const CircleParams & cp) {
    //const double alpha = circleProbabilityDistributionAlphaParameter;
    //const double alphaSq = alpha*alpha;
    //const double epsilon = circleProbabilityDistributionEpsilonParameter;
    //const double epsSq = epsilon*epsilon;
    //const double r=(p-cp.centre()).mag();
    //const double r0=cp.radius();
    //const double logg = log(r/r0);
    //const double logsq = logg*logg;
    //
    //return 1./(2.*pi)/sqrt(2.*pi*epsSq)*pow(r/r0,alpha-2)*exp(-0.5*(logsq/epsSq + alphaSq*epsSq  )) / (r*r);
    //};
    static void draw(Canvas & canvas, const RichParamsT & rp, const DataT & d, const bool showTruth=false);
  };

}

#include "RichStuff.tcc"

#endif
