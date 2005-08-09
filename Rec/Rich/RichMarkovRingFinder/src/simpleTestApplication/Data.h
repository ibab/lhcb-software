

#ifndef LESTER_DATA_H
#define LESTER_DATA_H

#include "Hit.h"
#include <vector>
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"
#include "Constants.h"
#include "RichPriors.h"
#include "GenericRingFinder/GenericInput.h"

namespace Lester {

  class Data {
  public:
    typedef std::vector<Hit> Hits;
  private:
    typedef std::vector<CircleParams> Circs;
    Circs secretCircs;
    Hits secretBg;
  public:
    Hits hits;
#ifdef LESTER_USE_GRAPHICS
    void draw(BasicCanvasInterface & canvas, const bool showTruth=false,
	      const double hitScaleFactor=1.0) const {
      if (showTruth) {
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
    void copyTo(GenRingF::GenericInput & input) const {
      {
	unsigned int i=0;
	for (Lester::Data::Hits::const_iterator it = hits.begin();
	     it!= hits.end();
	     ++it) {
	  input.hits.push_back(GenRingF::GenericHit(i++, it->x(), it->y()));
	};
      };
      {
	unsigned int i=0;    
	for (Lester::Data::Circs::const_iterator it = secretCircs.begin();
	     it!= secretCircs.end();
	     ++it) {
	  input.ringsFromMonteCarlo.push_back(GenRingF::GenericRing(i++, it->centre().x(), it->centre().y(), it->radius()));
	};
      };    
    };
    void setFrom(const GenRingF::GenericInput & input) {
      hits.clear();
      secretCircs.clear();
      secretBg.clear();
      for (std::list<GenRingF::GenericHit>::const_iterator it = input.hits.begin();
	   it != input.hits.end();
	   ++it) {
	hits.push_back(Lester::Hit(it->x(), it->y()));
      };
      for (std::list<GenRingF::GenericRing>::const_iterator it = input.ringsFromMonteCarlo.begin();
	   it != input.ringsFromMonteCarlo.end();
	   ++it) {
	secretCircs.push_back(Lester::CircleParams(Hep2Vector(it->x(), it->y()), it->radius()));
      };
      
    };
    void setFromFile(const std::string & file) {
      hits.clear();
      secretCircs.clear();
      secretBg.clear();
      std::ifstream f(file.c_str());
      double x,y;
      while (f>>x) {
	f>>y;
	
	x*=Constants::realDataInputScaleFactor;
	y*=Constants::realDataInputScaleFactor;
	  Hit h(x,y);
	  hits.push_back(h);
      };
    };
    void jokeSetRandom() /* deprecated */ {
      hits.clear();
      secretCircs.clear();
      secretBg.clear();
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
