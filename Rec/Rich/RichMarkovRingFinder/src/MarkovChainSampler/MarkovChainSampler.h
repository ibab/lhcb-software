#ifndef MARKOV_CHAIN_SAMPLER_H
#define MARKOV_CHAIN_SAMPLER_H

#include <iterator>
#include "finder/FinderException.h"

namespace RichMarkov {

  /// @class MarkovChainSampler 
  /// @brief The (most basic) sampler interface
  /// Note: behaves sort of like an iterator cf. ++ and * operators
  template <class Space>
  class MarkovChainSampler 
  // : public std::iterator<std::forward_iterator_tag, typename Space::PointType>
  {
  public:
    class LogOfZero : public FinderException {};
    typedef Space SpaceType;
    typedef typename Space::PointType PointType;
#ifdef WIN32
	PointType pointless;
#endif
    /// "Dereference" sampler gives current point
    virtual const PointType & operator* () const 
#ifdef WIN32
	  { return pointless; };
#else
		  = 0;
#endif
      virtual const PointType * operator->() const { 
	return &(operator*()); // default provided!
      };
    /// Increment sampler to next point
    inline MarkovChainSampler & operator++()  /* ++i */ { 
	moveToNextState();
	return *this;
      };
    /// Returns log probablity of the given point
    virtual double logTargetDistribution(const PointType & point) const 
#ifdef WIN32
	{ return 0; };
#else
		= 0;
#endif
	virtual ~MarkovChainSampler() {};
  private:
    const MarkovChainSampler operator++(int); // deliberately not implemented
  private:
    virtual void moveToNextState() 
#ifdef WIN32
	{};
#else
		= 0;
#endif
  };

}

#endif
