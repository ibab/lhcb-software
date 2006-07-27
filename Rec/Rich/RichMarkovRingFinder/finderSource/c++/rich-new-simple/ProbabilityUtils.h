
#ifndef LESTER_PROBABILITY_UTILS
#define LESTER_PROBABILITY_UTILS

//#include <gmpxx.h>
#include <vector>
#include <map>
#include "CLHEP/Random/RandFlat.h"
#include "Utils/MathsConstants.h"

namespace Lester {

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
    //    double ans=1;
    //for (double i=2; i<=n; ++i) {
    //  ans *= i;
    //};
    //return ans;
    return cache[n];
  };
  
  /*
  inline double oneOnNChooseSmall(const double n, const int r) {
    // evaluates 1/(n choose r) assuming r is a "small" integer!
    // if n<r it returns 0.
    if (n<r) {
      return 0;
    };
    double ans=1;
    for (int i=0; i<r; ++i) {
      const double ri = static_cast<double>(i);
      ans *= (r-ri)/(n-ri);
    };
    return ans;
    };
  */
  /*
 inline mpz_class nChooseSmall(const mpz_class & n, unsigned long int r) {
    // evaluates (n choose r) assuming r is a "small" integer!
    // if n<r it returns 0.
    if (n<r) {
      return mpz_class(0);
    };
    mpz_t nt;
    mpz_init(nt);
    mpz_set(nt, n.get_mpz_t());
    mpz_t result;
    mpz_init(result);
    mpz_bin_ui(result, nt, r);
    const mpz_class ans(result);
    mpz_clear(result);
    mpz_clear(nt);
    return ans;
  };
*/
  inline double poissonProb(const int n, const double mu) {
    if (n<=40) {
      return pow(mu,n)*exp(-mu)/factorial(n); 
    } else {
      return (exp(n*log(mu)-mu-n*log(n)-0.5*log(n)+n)/sqrt(MathsConstants::twoPi)); // check!
    };
  };

  inline double gaussianProb(const double x,
			     const double mu,
			     const double sig) {
    const double thing=(x-mu)/sig;
    const double thingSq=thing*thing;
    return 1./(sqrt(MathsConstants::twoPi)*sig)*exp(-0.5*thingSq);
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
    return (exp(-x/mean))/mean;
  };
  inline double exponentialProbAbove(const double x,
				     const double mean) {
    //prob that var distrib expntly with mean mu is greater than x
    return exp(-x/mean);
  };
  inline double reciprocalSum(const int n) {
    // Could speed up // LESTER
    if (n>=20&&n<40) {
      static bool first=true;
      if (first) {
	std::cerr << "You should probably optimise reciprocalSum!" <<std::endl;
	first=false;
      };
    };
    if (n>=40) {
      static bool first=true;
      if (first) {
	std::cerr << "You should DEFINITELY optimise reciprocalSum!" <<std::endl;
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
	std::cerr << "You should probably optimise reciprocalSqSum!" <<std::endl;
	first=false;
      };
    };
    if (n>=40) {
      static bool first=true;
      if (first) {
	std::cerr << "You should DEFINITELY optimise reciprocalSqSum!" <<std::endl;
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
    //std::cerr << "oh dear failed because i="<<i<<" and ntot=" << nTot<< std::endl;
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
