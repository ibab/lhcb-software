
#ifndef LESTER_DOMINANT_FINITE_RELATIVE_PROBABILITY_CHOOSER_H
#define LESTER_DOMINANT_FINITE_RELATIVE_PROBABILITY_CHOOSER_H

#include <map>
#include <iostream>
#include "CLHEP/Random/RandFlat.h"

namespace Lester
{

  template <class Index>
  class DominantFiniteRelativeProbabilityChooser {
  private:
    class SampledTooEarly {
    };
    double totalProb;
    typedef std::multimap<double, Index, std::greater_equal<double> > Multimap; // Want it to store biggest probabilities first  We are choosing this direction for speed of sampling not for mathematical accuraccy in summing!
    Multimap cache;
  public:
    DominantFiniteRelativeProbabilityChooser() : totalProb(0), cache() {
    };
    DominantFiniteRelativeProbabilityChooser(std::ifstream & is) : totalProb(0), cache() {
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
      const int initialSize = cache.size();
      const typename Multimap::value_type v(probability, index);
      cache.insert(v);
      const int finalSize = cache.size();
      assert(finalSize==initialSize+1); // not a throw as this should NEVER happen unless I am much mistaken
      totalProb+=probability;
    };
    Index sampleAnIndex() const {
      if (cache.empty()) {
        throw SampledTooEarly();
      };
      // If we got this far, we have at least one element!
      const double target = totalProb * RandFlat::shoot();
      typename Multimap::const_iterator it=cache.begin();
      double sumSoFar=0;
      while (true) {
        sumSoFar += (it->first);
        if (sumSoFar>=target) {
          return it->second;
        };
        ++it;
        if (it==cache.end()) {
          // might get here due to strange rounding errors ...
          // since eventuality is very unlikely, just return the
          // most likely element ... it shouldn't
          // perturb its probability much
          return cache.begin()->second;
        };
      };
    };
  private:
    void printMapTo(std::ostream & i) const {
      for ( typename Multimap::const_iterator it = cache.begin();
            it!=cache.end();
            ++it )
      {
        i <<  "[prob=" << it->first << ",index=" << it->second << "],";
      }
    };
  };

};

#endif
