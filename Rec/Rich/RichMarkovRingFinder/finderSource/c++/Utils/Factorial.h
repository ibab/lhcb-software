
#ifndef LESTER_FACTORIAL_UTILS
#define LESTER_FACTORIAL_UTILS


namespace Lester {
  
  inline double factorial(int n) {
    class FactorialOutOfRange {};
    
    const int numberCached = 60;
    if (n<0 || n>=numberCached) {
      throw FactorialOutOfRange();
    };
    
    static const double cache[numberCached]=
      {
#include "Utils/factorialInitialiser.h"
      };
    //    double ans=1;
    //for (double i=2; i<=n; ++i) {
    //  ans *= i;
    //};
    //return ans;
    return cache[n];
  };
  
  
};

#endif
