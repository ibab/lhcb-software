
#ifndef LESTER_PROBABILITY_UTILS
#define LESTER_PROBABILITY_UTILS

//#include <gmpxx.h>
#include <vector>
#include <map>
#include <cmath>
#include <cassert>
#include "CLHEP/Random/RandFlat.h"
#include "Utils/MathsConstants.h"
#include "Utils/MessageHandler.h"

namespace Lester 
{

  inline double factorial(int n) {
    class FactorialOutOfRange {};

    const int numberCached = 60;
    if (n<0 || n>=numberCached) {
      throw FactorialOutOfRange();
    };

    static const double cache[numberCached]=
      {
#include "factorialInitialiser.h"
      };
    return cache[n];
  };

  inline double poissonProb(const int n, const double mu) {
    if (n<=40) {
      return std::pow(mu,n)*std::exp(-mu)/factorial(n);
    } else {
      const double logn = std::log((double)n);
      return (std::exp((double)n*std::log(mu)-mu-n*logn -
                       0.5*logn+n)/std::sqrt(MathsConstants::twoPi)); // check!
    };
  };

  inline double gaussianProb(const double x,
                             const double mu,
                             const double sig) {
    const double thing=(x-mu)/sig;
    const double thingSq=thing*thing;
    return 1./(std::sqrt(MathsConstants::twoPi)*sig)*std::exp(-0.5*thingSq);
  };
  inline std::vector<int> getSetOfMDifferentIntsFromN(const int n, const int m) {
    assert(n>=m);
    assert(n>=0);
    assert(m>=0);
    typedef std::map<int,int> Map;
    Map contains;
    std::vector<int> isAt(m);
    for (int i=0; i<m; ++i) {
      contains[i]=i;
      isAt[i]=i;
    };
    for (int i=0; i<m; ++i) {
      const int moving = i;
      const int presentPosition = isAt[moving];
      const int destination = static_cast<int>(RandFlat::shoot(n));
      const Map::const_iterator it = contains.find(destination);
      const bool destinationIsOccupied = (it!=contains.end());
      if (destinationIsOccupied) {
        const int thingAtDestination = it->second;
        //Swap the two:
        isAt[moving]=destination;
        isAt[thingAtDestination]=presentPosition;
        contains[destination]=moving;
        contains[presentPosition]=thingAtDestination;
      } else {
        isAt[moving]=destination;
        contains[destination]=moving;
        contains.erase(presentPosition);
      };
    };
    assert(static_cast<int>(isAt.size())==m); //Still!
    return isAt;
  };

  inline double exponentialProb(const double x,
                                const double mean) {
    return (std::exp(-x/mean))/mean;
  };
  inline double exponentialProbAbove(const double x,
                                     const double mean) {
    //prob that var distrib expntly with mean mu is greater than x
    return std::exp(-x/mean);
  };
  inline double reciprocalSum(const int n) {
    // Could speed up // LESTER
    if (n>=20&&n<40) {
      static bool first=true;
      if (first) {
        Lester::messHandle().debug() << "You should probably optimise reciprocalSum!" <<Lester::endmsg;
        first=false;
      };
    };
    if (n>=40) {
      static bool first=true;
      if (first) {
        Lester::messHandle().debug() << "You should DEFINITELY optimise reciprocalSum!" << Lester::endmsg;
        first=false;
      };
    };
    double ans=1;
    for (int i=2; i<=n; ++i) {
      const double one=1;
      const double d=static_cast<double>(i);
      const double reciprocal=one/d;
      ans += reciprocal;
    };
    return ans;
  };
  inline double reciprocalSqSum(const int n) {
    // Could speed up // LESTER
    if (n>=20&&n<40) {
      static bool first=true;
      if (first) {
        Lester::messHandle().debug() << "You should probably optimise reciprocalSqSum!" <<Lester::endmsg;
        first=false;
      };
    };
    if (n>=40) {
      static bool first=true;
      if (first) {
        Lester::messHandle().debug() << "You should DEFINITELY optimise reciprocalSqSum!" <<Lester::endmsg;
        first=false;
      };
    };
    double ans=1;
    for (int i=2; i<=n; ++i) {
      const double one=1;
      const double d=static_cast<double>(i);
      const double reciprocal=one/d;
      ans += reciprocal*reciprocal;
    };
    return ans;
  };
  inline int lestersRandomNumberToJitter1(const int nTot) {
    assert(nTot>=1);
    const double tot=reciprocalSum(nTot);
    const double mark=tot*RandFlat::shoot();
    double sum=1;
    int i=1;
    while (sum<mark) {
      ++i;
      const double d=static_cast<double>(i);
      const double one=1;
      const double reciprocal=one/d;
      sum += reciprocal;
    };
    assert(i<=nTot);
    return i;
  };
  inline int lestersRandomNumberToJitter2(const int nTot) {
    const double tot=reciprocalSqSum(nTot);
    const double mark=tot*RandFlat::shoot();
    double sum=1;
    int i=1;
    while (sum<mark) {
      ++i;
      const double d=static_cast<double>(i);
      const double one=1;
      const double reciprocal=one/d;
      sum += reciprocal*reciprocal;
    };
    assert(i<=nTot);
    return i;
  };

};

#endif
