

#ifndef LESTER_DATA_H
#define LESTER_DATA_H

#include "Hit.h"
#include <vector>
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"
#include "Constants.h"
#include "RichPriors.h"

namespace Lester {

  class Data {
  public:
    typedef std::vector<Hit> Hits;
    inline void doNothing() const throw() {};
  public: // should be private: but temporarily public for revision of NIM paper
    typedef std::vector<CircleParams> Circs;
    Circs secretCircs;
    Hits secretBg;
  public:
    Hits hits;
#ifdef LESTER_USE_GRAPHICS
    void draw(BasicCanvasInterface & canvas, const bool showTruth=false,
              const double hitScaleFactor=1.0) const {
      if (showTruth) {
        //std::cout << "Drawing secret circs " << secretCircs.size() << std::endl;
        for (Circs::const_iterator it = secretCircs.begin();
             it!=secretCircs.end();
             ++it) {
          it->draw(canvas);
        };
      };
      for (Hits::const_iterator it = hits.begin();
           it!=hits.end();
           ++it) {
        it->draw(canvas, hitScaleFactor);
      };
      if (showTruth) {
        for (Hits::const_iterator it = secretBg.begin();
             it!=secretBg.end();
             ++it) {
          it->draw(canvas);
        };
      };
    };
#endif
    void setFromFile(const std::string & file) {
      hits.clear();
      secretCircs.clear();
      std::ifstream f(file.c_str());
      if ( f.is_open() )
      {
        double x,y;
        while (f>>x) {
          f>>y;
          Hit h(x,y);
          hits.push_back(h);
        };
      }
      else { throw std::exception(); }
    };
    void jokeSetRandom() /* deprecated */ {
      hits.clear();
      secretCircs.clear();
      // First put in some "data" hits
      {
        const int nc = RichPriors::sampleFromNumberOfCirclesDistribution();
        for (int i=0; i<nc; ++i) {
          CircleParams cp;
          cp.jokeSetRandom();
          secretCircs.push_back(cp);
          const int nh = RichPriors::sampleFromNumberOfHitsDueToCircle(cp);
          for (int h=0; h<nh; ++h) {
            hits.push_back(RichPriors::sampleHitDueToCircle(cp));
          };
        };
      };
      // Now put in some "background" hits
      {
        const int nd = RichPriors::sampleFromNumberOfHitsDueToBackground();
        for (int i=0; i<nd; ++i) {
          const Hit h=RichPriors::sampleHitDueToBackground();
          secretBg.push_back(h);
          hits.push_back(h);
        };
      };
    };
  };

};

#endif
