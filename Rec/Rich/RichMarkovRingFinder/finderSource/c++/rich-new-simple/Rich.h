
#ifndef LESTER_RICH_H
#define LESTER_RICH_H

namespace Lester {
  
  class Rich;
  class RichParams;
  class Data;

};

#include "CircleParams.h"
#include "CLHEP/Vector/TwoVector.h"
#include "GraphicsObjects.h"

namespace Lester {

  class Rich {
  public:
    static double totalLogProb(const RichParams & rp, const Data & d);
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
#ifdef LESTER_USE_GRAPHICS
    static void draw(BasicCanvasInterface & canvas, const RichParams & rp, const Data & d, const bool showTruth=false);
#endif
  };

};

#endif
