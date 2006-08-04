#include "Data.h"

#include "RichPriors.h"

namespace Lester {

  void Data::doNothing() const throw() {};

#ifdef LESTER_USE_GRAPHICS
  void Data::draw(BasicCanvasInterface & canvas, const bool showTruth,
		  const double hitScaleFactor) const {
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
  }
#endif

  void Data::setFromFile(const std::string & file) {
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
  }

  void Data::jokeSetRandom() /* deprecated */ {
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
  }
  

} // namespace

