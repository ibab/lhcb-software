#ifndef MARKOV_CHAIN_SAMPLER_H
#define MARKOV_CHAIN_SAMPLER_H

#include <iterator>

namespace Lester {

  template <class PointType, typename SampledPointType=PointType> /* For single point methods (eg Metropolis) SampledPointType is the same as PointType .. but for MultiPointMethods (eg leapfrog) SampledPointType might be something like std::set<PointType> etc ... */
  class MarkovChainSampler 
  // : public std::iterator<std::forward_iterator_tag, typename Space::PointType> 
  {
  public:    
    //class LogOfZero {}; -- abandoned in favour of Lester::LogarithmicTools::LogOfZero
    //typedef Space SpaceType;
    //typedef typename Space::PointType PointType;

    

    virtual const SampledPointType operator* () const = 0;
    inline MarkovChainSampler & operator++()  /* ++i */ { 
      moveToNextState();
      return *this;
    };
    virtual double logTargetDistribution(const PointType & point) const = 0;
  private:
    const MarkovChainSampler & operator++(int); // deliberately not implemented
  private:
    virtual void moveToNextState() = 0;
  };
};

#endif
