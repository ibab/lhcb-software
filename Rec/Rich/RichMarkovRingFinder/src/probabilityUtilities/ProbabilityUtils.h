
#ifndef RICHMARKOV_PROBABILITY_UTILS
#define RICHMARKOV_PROBABILITY_UTILS

//#include "gmpxx.h"
#include <vector>
#include <map>
#include "CLHEP/Random/RandFlat.h"
#include "MathsConstants/MathsConstants.h"
#include <cassert>

namespace RichMarkov {

  class LogOfZero {
  };

  inline double factorial(int n) {
    class FactorialOutOfRange {};

    const int numberCached = 60;
    if (n<0 || n>=numberCached) {
      throw FactorialOutOfRange();
    };

    static const double cache[numberCached]=
      {
        //#include "factorialInitialiser.h"
        // putting this here explicitly since otherwise Doxygen cocks up line numbers
        1.,
        1.,
        2.,
        6.,
        24.,
        120.,
        720.,
        5040.,
        40320.,
        362880.,
        3628800.,
        39916800.,
        479001600.,
        6227020800.,
        87178291200.,
        1307674368000.,
        20922789888000.,
        355687428096000.,
        6402373705728000.,
        121645100408832000.,
        2432902008176640000.,
        51090942171709440000.,
        1124000727777607680000.,
        25852016738884976640000.,
        620448401733239439360000.,
        15511210043330985984000000.,
        403291461126605635584000000.,
        10888869450418352160768000000.,
        304888344611713860501504000000.,
        8841761993739701954543616000000.,
        265252859812191058636308480000000.,
        8222838654177922817725562880000000.,
        263130836933693530167218012160000000.,
        8683317618811886495518194401280000000.,
        295232799039604140847618609643520000000.,
        10333147966386144929666651337523200000000.,
        371993326789901217467999448150835200000000.,
        13763753091226345046315979581580902400000000.,
        523022617466601111760007224100074291200000000.,
        20397882081197443358640281739902897356800000000.,
        815915283247897734345611269596115894272000000000.,
        33452526613163807108170062053440751665152000000000.,
        1405006117752879898543142606244511569936384000000000.,
        60415263063373835637355132068513997507264512000000000.,
        2658271574788448768043625811014615890319638528000000000.,
        119622220865480194561963161495657715064383733760000000000.,
        5502622159812088949850305428800254892961651752960000000000.,
        258623241511168180642964355153611979969197632389120000000000.,
        12413915592536072670862289047373375038521486354677760000000000.,
        608281864034267560872252163321295376887552831379210240000000000.,
        30414093201713378043612608166064768844377641568960512000000000000.,
        1551118753287382280224243016469303211063259720016986112000000000000.,
        80658175170943878571660636856403766975289505440883277824000000000000.,
        4274883284060025564298013753389399649690343788366813724672000000000000.,
        230843697339241380472092742683027581083278564571807941132288000000000000.,
        12696403353658275925965100847566516959580321051449436762275840000000000000.,
        710998587804863451854045647463724949736497978881168458687447040000000000000.,
        40526919504877216755680601905432322134980384796226602145184481280000000000000.,
        2350561331282878571829474910515074683828862318181142924420699914240000000000000.,
        138683118545689835737939019720389406345902876772687432540821294940160000000000000.
      };
    //    double ans=1;
    //for (double i=2; i<=n; ++i) {
    //  ans *= i;
    //};
    //return ans;
    return cache[n];
  };

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
  inline double poissonProb(const int nInt, const double mu) {
    const double nDoub = static_cast<double>(nInt);
    if (nInt<=40) {
      return pow(mu,nDoub)*exp(-mu)/factorial(nInt);
    } else {
      return (exp(nDoub*log(mu)-mu-nDoub*log(nDoub)-0.5*log(nDoub)+nDoub)/sqrt(MathsConstants::twopi)); // check!
    };
  };

  inline double gaussianProb(const double x,
                             const double mu,
                             const double sig) {
    const double thing=(x-mu)/sig;
    const double thingSq=thing*thing;
    return 1./(sqrt(MathsConstants::twopi)*sig)*exp(-0.5*thingSq);
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

  inline std::vector<unsigned int> getSetOfMDifferentUnsignedIntsFromN(const unsigned int n, const unsigned int m) {
    assert(n>=m);
    typedef std::map<unsigned int,unsigned int> Map;
    Map contains;
    std::vector<unsigned int> isAt(m);
    for (unsigned int i=0; i<m; ++i) {
      contains[i]=i;
      isAt[i]=i;
    };
    for (unsigned int i=0; i<m; ++i) {
      const unsigned int moving = i;
      const unsigned int presentPosition = isAt[moving];
      const unsigned int destination = static_cast<unsigned int>(RandFlat::shoot(n));
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
    assert(isAt.size()==m); //Still!
    return isAt;
  }

  inline double exponentialProb(const double x,
                                const double mean) {
    return (exp(-x/mean))/mean;
  }

  inline double exponentialProbAbove(const double x,
                                     const double mean) {
    //prob that var distrib expntly with mean mu is greater than x
    return exp(-x/mean);
  }

  inline double reciprocalSum(const unsigned int n) {
    static std::map<int, double> recipSumCache;
    recipSumCache[1] = 1.0;

    // Could speed up
    /*
      if (n >= 20 && n < 40) {
      static bool first(true);
      if (first) {
      std::cerr << "You should probably optimise reciprocalSum!" << std::endl;
      first = false;
      }
      }
      if (n >= 40) {
      static bool first=true;
      if (first) {
      std::cerr << "You should DEFINITELY optimise reciprocalSum!" << std::endl;
      first = false;
      }
      }
    */

    // Sped-up version:
    if ( n <= recipSumCache.size() ) {
      // We've already calculated this one
      //std::cout << "Reciprocal sum I've seen before: sum to " << n << " = " 
      //          << recipSumCache[n] << ". Cache size = " << recipSumCache.size() << std::endl;
      return recipSumCache[n];
    } else {
      std::pair<int, double> biggestCached( *max_element(recipSumCache.begin(), recipSumCache.end()) );
      double ans(biggestCached.second);
      for (unsigned int i = biggestCached.first + 1; i <= n; ++i) {
        const double one(1.0);
        const double d( static_cast<double>(i) );
        const double reciprocal( one / d );
        ans += reciprocal;
        recipSumCache[i] = ans; // cache as we calculate
        //std::cout << "Reciprocal sum I've not seen before: sum to " << i << " = " 
        //          << ans << ". Cache size = " << recipSumCache.size() << std::endl;
      }
      return ans;
    }
  }

  inline double reciprocalSqSum(const unsigned int n) {
    static std::map<int, double> recipSqSumCache;
    recipSqSumCache[1] = 1.0;

    // Could speed up
    /*
      if (n >= 20 && n < 40) {
      static bool first=true;
      if (first) {
      std::cerr << "You should probably optimise reciprocalSqSum!" <<std::endl;
      first = false;
      }
      }
      if (n>=40) {
      static bool first=true;
      if (first) {
      std::cerr << "You should DEFINITELY optimise reciprocalSqSum!" <<std::endl;
      first=false;
      }
      }
    */

    // Sped-up version:
    if ( n <= recipSqSumCache.size() ) {
      //std::cout << "Reciprocal sq sum I've seen before: sum to " << n << " = " 
      //          << recipSqSumCache[n] << ". Cache size = " << recipSqSumCache.size() << std::endl;
      return recipSqSumCache[n];
    } else {
      std::pair<int, double> biggestCached( *max_element(recipSqSumCache.begin(), recipSqSumCache.end()) );
      double ans(biggestCached.second);
      for (unsigned int i = biggestCached.first + 1; i <= n; ++i) {
        const double one(1.0);
        const double d( static_cast<double>(i) );
        const double reciprocal( one / d );
        ans += reciprocal * reciprocal;
        recipSqSumCache[i] = ans; // cache as we calculate
        //std::cout << "Reciprocal sq sum I've not seen before: sum to " << i << " = " 
        //          << ans << ". Cache size = " << recipSqSumCache.size() << std::endl;
      }
      return ans;
    }
  }

  inline unsigned int lestersRandomNumberToJitter1(const unsigned int nTot) {
    assert(nTot>=1);
    const double tot=reciprocalSum(nTot);
    const double mark=tot*RandFlat::shoot();
    double sum=1;
    unsigned int i=1;
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
  inline unsigned int lestersRandomNumberToJitter2(const unsigned int nTot) {
    const double tot=reciprocalSqSum(nTot);
    const double mark=tot*RandFlat::shoot();
    double sum=1;
    unsigned int i=1;
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

}

#endif
