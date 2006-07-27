
#ifndef LESTER_FINITE_RELATIVE_PROBABILITY_CHOOSER_H
#define LESTER_FINITE_RELATIVE_PROBABILITY_CHOOSER_H

#include <map>
#include <iostream>
#include <fstream>
#include "CLHEP/Random/RandFlat.h"

namespace Lester {

  template <class Index>
  class FiniteRelativeProbabilityChooser {
  private:
    class NoSamplesPossible {
    };
    class NoMaxExists {
    };
    double totalProb;
    typedef std::map<double, Index> Map; // Want it to store biggest probabilities first  We are choosing this direction for speed of sampling not for mathematical accuraccy in summing!
    Map cache;
  public:
    FiniteRelativeProbabilityChooser() : totalProb(0), cache() {
    };
    FiniteRelativeProbabilityChooser(std::string fileName) : totalProb(0), cache() {
      std::ifstream f(fileName.c_str());
      readMoreFromStream(f);
    };
    FiniteRelativeProbabilityChooser(std::istream & is) : totalProb(0), cache() {
      readMoreFromStream(is);
    };
    void readMoreFromStream(std::istream & is) {
      Index index;
      double probability;
      while (is>>index) {
        is>>probability;
        addIndexAndProbability(index,probability);
      };
    };
    void clear() {
      // Resets everything
      cache.clear();
      totalProb=0;
    };
    void addIndexAndProbability(const Index index, const double probability) {
      assert(probability>=0);
      const double oldTotalProb = totalProb;
      totalProb+=probability;
      if (oldTotalProb!=totalProb) {
        // We can resolve the new probability from the old one so
        const typename Map::value_type v(totalProb, index);
        const typename Map::iterator hint = cache.end();
        cache.insert(hint, v);
      };
    };
    Index sampleAnIndex() const {
      //if (cache.empty()) {
      //std::cerr<<"Oh dear .. sampled too early" << std::endl;
      //throw NoSamplesPossible();
      //};
      // If we got this far, we have at least one element!
      const double target = totalProb * RandFlat::shoot();
      const typename Map::const_iterator ge_target_it=cache.lower_bound(target);
      //std::cerr << "FROOM " << cache.size() << " " << target << " " << totalProb << std::endl;
      //std::cerr << "JOOM" << (--cache.end())->first << std::endl;
      //std::cerr << "JOOM" << (--cache.end())->second << std::endl;
      //assert(ge_target_it!=cache.end());
      if (cache.empty()) {
        throw NoSamplesPossible();
      };
      if (ge_target_it==cache.end()) {
        throw NoSamplesPossible();
      };
      return ge_target_it->second;
    };
    Index sampleAnIndexAbove(const Index index) const {
#ifndef NDEBUG
      static bool first=true;
      if (first) {
        first = false;
        std::cerr<<"FiniteRelativeProbabilityChooser::sampleAnIndexAbove is implemented horrendously and should only be used _if_at_all_ in caching functions!"<<std::endl;
      };
#endif
      try {
        const Index mi = maxIndex();
        if (mi<index) {
          throw NoSamplesPossible();
        } else if (mi==index) {
          return index;
        };
        // go for REALLY DUMB METHOD: sample till we get one above!
        //std::cerr << "Taking ages ..." <<std::flush;
        while (true) {
          const Index i = sampleAnIndex();
          if (i>=index) {
            //std::cerr << " ... done" << std::endl;
            return i;
          };
          // try again!
        };
      } catch (NoMaxExists nme) {
        throw NoSamplesPossible();
      };
    };
  private:
    Index maxIndex() const {
      // Don't optimise me! I suck!  You'll only encourage my use!
      Index maxIndexSoFar(0);
      if (cache.empty()) {
        throw NoMaxExists();
      };
      for (typename Map::const_iterator it = cache.begin();
           it!=cache.end();
           ++it) {
        const bool first = (it==cache.begin());
        const Index currentIndex = it->second;
        if (first || (currentIndex > maxIndexSoFar)) {
          maxIndexSoFar=currentIndex;
        };
      };
      return maxIndexSoFar;
    };
    void printMapTo(std::ostream & i) const {
      for (typename Map::const_iterator it = cache.begin();
           it!=cache.end();
           ++it) {
        i <<  "prob=" << it->first << " index=" << it->second << std::endl;
      };
    };
  };

};

#endif
