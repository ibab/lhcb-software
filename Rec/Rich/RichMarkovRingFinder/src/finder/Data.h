
#ifndef RICHMARKOV_DATA_H
#define RICHMARKOV_DATA_H

#include <vector>
#include <sstream>
#include <cassert>
#include "Hit.h"
#include "Constants.h"
#include "FinderExternalException.h"

//fwd decs
#include "CircleParams.fwd"
#include "RichPriors.fwd"
namespace RichMarkov {
  class Canvas;
}

namespace RichMarkov {

  template <class Mode, const Mode & mode>
  class Data {
  public:
    typedef RichPriors<Mode,mode>   RichPriorsT;
    typedef CircleParams<Mode,mode> CircleParamsT;
  public:
    typedef std::vector<CircleParamsT> Circs;
  public:
    class SuppliedHitWasNotInAcceptance : public FinderExternalException {
    public:
      const Hit hit;
    public:
      SuppliedHitWasNotInAcceptance(std::string mess = "Supplied hit was not in acceptance") : 
        FinderExternalException(mess) {
      };
    };
  private:
    static void confirmHitLiesInAcceptance(const Hit & h);
  public:
    typedef std::vector<Hit> Hits;
  private:
    Circs secretCircs;
    Hits secretBg;
  public:
    Hits hits;

  public:
    inline const Hits & getHits() const {
      return hits;
    };

    inline void addHit(const Hit & hit) {
      confirmHitLiesInAcceptance(hit);
      hits.push_back(hit);
    };

    void draw(Canvas & canvas, const bool showTruth=false) const;

    void jokeSetRandom(); /* deprecated */

  public:
    inline const Circs & secretCircles() const {
      return secretCircs;
    }
    inline Circs & secretCircles() {
      return secretCircs;
    }

    inline void initialiseCache() const {
      for ( Hits::const_iterator it = hits.begin();
            it != hits.end();
            ++it) {
        it->confirmProposedCircleProbabilityCache();
      };
    };
    inline void popCacheFront() const {
      for (Hits::const_iterator it = hits.begin();
           it != hits.end();
           ++it) {
        it->popCacheFront();
        it->confirmProposedCircleProbabilityCache();
      };
    };
    inline void popCacheBack() const {
      static bool first = true;
      if (first) {
	std::cerr << "I am not sure whether the it->abandonProposedCircleProbabilityCache() call below (as opposed to the it->popCacheBack() call) is appropriate to be invoked after a m.test() initial point fail as happens at present" << std::endl;
	first = false;
      };
      for (Hits::const_iterator it = hits.begin();
           it != hits.end();
           ++it) {
        it->popCacheBack();
        it->abandonProposedCircleProbabilityCache();
      };
    };
  };

}

#include "Data.tcc"

#endif
