
#ifndef RICHMARKOV_DATA_TCC
#define RICHMARKOV_DATA_TCC

#include "Data.h"
#include "RichPriors.h"

/*
void RichMarkov::Data::jokeSetRandom() { // deprecated 
  hits.clear();
  secretCircs.clear();
  if (!Constants::useRealData) {
    // First put in some "data" hits
    {
      const int nc = RichPriorsT::sampleFromNumberOfCirclesDistribution();
      for (int i=0; i<nc; ++i) {
        CircleParams cp;
        cp.jokeSetRandom();
        secretCircs.push_back(cp);
        const int nh = RichPriorsT::sampleFromNumberOfHitsDueToCircle(cp);
        for (int h=0; h<nh; ++h) {
          addHit(RichPriorsT::sampleHitDueToCircle(cp));
        };
      };
    };
    // Now put in some "background" hits
    {
      const int nd = RichPriorsT::sampleFromNumberOfHitsDueToBackground();
      for (int i=0; i<nd; ++i) {
        const Hit h=RichPriorsT::sampleHitDueToBackground();
        secretBg.push_back(h);
        addHit(h);
      };
    };
  } else {
    std::ifstream f("/usera/lester/proj/c++/rich/DATA/rich2_3.txt");
    double x,y;
    while (f>>x) {
      f>>y;

      x*=Constants::realXYDataInputScaleFactor;
      y*=Constants::realXYDataInputScaleFactor;
      Hit h(x,y);
      addHit(h);
    };

  };
};
*/

template <class Mode, const Mode & mode>
void RichMarkov::Data<Mode, mode>::confirmHitLiesInAcceptance(const Hit & h) {
  if (!(RichPriorsT::hitLiesInAcceptanceCrude(h))) {
    std::ostringstream mess;
    mess << "Hit out of acceptance. Position = (" 
         << h.x()/mode.realXYDataInputScaleFactor() << ", " 
         << h.y()/mode.realXYDataInputScaleFactor() << ")";
    throw SuppliedHitWasNotInAcceptance(mess.str());
  };
};

#endif
