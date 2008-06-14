
#ifndef LESTER_PROBABILITY_ABOVE_H
#define LESTER_PROBABILITY_ABOVE_H

#include <map>
#include <iostream>
#include "CLHEP/Random/RandFlat.h"

namespace Lester
{

  template <class Index>
  class ProbabilityAbove
  {

  private:
    mutable bool changed;
    mutable double totalProb;

    typedef std::map<Index, double> Multimap;
    mutable Multimap cache;
    typedef std::map<Index,double, std::greater_equal<Index> > InputMap;
    mutable InputMap inputMap;

  public:

    ProbabilityAbove() : changed(true), totalProb(0), cache(), inputMap() { }
    ProbabilityAbove(const std::string & fileName) : changed(true), totalProb(0), cache(), inputMap()
    {
      readAllFrom(fileName);
    }

  private:

    void readAllFrom(const std::string & fileName)
    {
      std::ifstream is(fileName.c_str());
      if ( is.is_open() )
      {
        Lester::messHandle().debug() << "ProbabilityAbove : Reading data file '"
                                     << fileName << "'"
                                     << Lester::endmsg;
        Index index;
        double probability;
        while (is>>index)
        {
          is>>probability;
          addIndexAndProbability(index,probability);
          Lester::messHandle().debug() << " -> Read data point (x,y) : "
                                       << index << " " << probability
                                       << Lester::endmsg;
        }
      }
      else
      {
        Lester::messHandle().error() << "ProbabilityAbove : Failed to open data file '"
                                     << fileName << "'"
                                     << Lester::endmsg;
      }
    }

  public:

    void clear()
    {
      // Resets everything
      changed=true;
      cache.clear();
      inputMap.clear();
      totalProb=0;
    }
    inline double operator() (const Index index) const
    {
      return probabilityAbove(index);
    }
    inline double probabilityAbove(const Index index) const
    {
      buildCumulativeCache();
      typename Multimap::const_iterator ge_it = cache.lower_bound(index);
      return ( ge_it==cache.end() ? 0 :ge_it->second );
    }

  private:

    inline void buildCumulativeCache() const
    {
      // only need to do something if inputMap has changed:
      if (changed)
      {
        // input map should already be sorted by Index ... hopefully with largest index first!

        cache.clear();

        double tp=0;
        for ( typename InputMap::const_iterator it = inputMap.begin();
              it!=inputMap.end(); ++it)
        {
          const Index index = it->first;
          const double prob = it->second;
          tp += prob;
          assert(cache.find(index)==cache.end());
          cache[index]=tp/totalProb;
        }

        changed=false;
      }
    }

    void addIndexAndProbability(const Index index, const double probability)
    {
      const typename InputMap::iterator it = inputMap.find(index);
      if (it==inputMap.end())
      {
        // new entry
        inputMap[index]=probability;
      }
      else
      {
        // add to existing entry;
        (it->second) += probability;
      }
      totalProb+=probability;
      changed=true;
    }

  };

}

#endif
