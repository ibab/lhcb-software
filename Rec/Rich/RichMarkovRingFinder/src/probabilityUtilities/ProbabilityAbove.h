
#ifndef RICHMARKOV_PROBABILITY_ABOVE_H
#define RICHMARKOV_PROBABILITY_ABOVE_H

#include <map>
#include <iostream>
#include "CLHEP/Random/RandFlat.h"

namespace RichMarkov {

  template <class Index>
  class ProbabilityAbove {

  private:

    mutable bool changed;
    mutable double totalProb;

    typedef std::map<Index, double> Multimap;
    mutable Multimap cache;
    typedef std::map<Index,double, std::greater_equal<Index> > InputMap;
    mutable InputMap inputMap;

  public:

    ProbabilityAbove() : changed(true), totalProb(0), cache(), inputMap() {
    };

    ProbabilityAbove(const std::string file, const std::string dir ) 
      : changed(true), totalProb(0), cache(), inputMap() 
    {
      std::ifstream f((dir+file).c_str());
      readAllFrom(f);
    };

    ProbabilityAbove(std::istream & is) 
      : changed(true), totalProb(0), cache(), inputMap()
    {
      readAllFrom(is);
    };

  private:
    void readAllFrom(std::istream & is) {
      Index index;
      double probability;
      while (is>>index) {
        is>>probability;
        addIndexAndProbability(index,probability);
      };
    };

  public:
    void clear() {
      // Resets everything
      changed=true;
      cache.clear();
      inputMap.clear();
      totalProb=0;
    };
    inline double operator() (const Index index) const {
      return probabilityAbove(index);
    };
    inline double probabilityAbove(const Index index) const {
      buildCumulativeCache();
      typename Multimap::const_iterator ge_it = cache.lower_bound(index);
      if (ge_it==cache.end()) {
        return 0;
      } else {
        return ge_it->second;
      };
    };

  private:
    inline void buildCumulativeCache() const {
      // only need to do something if inputMap has changed:
      if (changed) {
        // input map should already be sorted by Index ... hopefully with largest index first!

        cache.clear();

        double tp=0;
        for (typename InputMap::const_iterator it = inputMap.begin();
             it!=inputMap.end();
             ++it) {
          const Index index = it->first;
          const double prob = it->second;
          tp += prob;
          assert(cache.find(index)==cache.end());
          cache[index]=tp/totalProb;
        };

        changed=false;
      };
    };
    void addIndexAndProbability(const Index index, const double probability) {
      const typename InputMap::iterator it = inputMap.find(index);
      if (it==inputMap.end()) {
        // new entry
        inputMap[index]=probability;
      } else {
        // add to existing entry;
        (it->second) += probability;
      };
      totalProb+=probability;
      changed=true;
    };
  };

}

#endif
