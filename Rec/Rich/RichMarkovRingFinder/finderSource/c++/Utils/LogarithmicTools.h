
#ifndef LESTER_LOGARITHMICTOOLS_H
#define LESTER_LOGARITHMICTOOLS_H

#include <cmath>
#include <cassert>
#include <exception>
#include "Utils/CheckForNan.h"

namespace Lester
{

  namespace LogarithmicTools
  {

    class LogOfZero : public std::exception
    {
      virtual const char * what() const throw()
      {
        return "LogarithmicTools::LogOfZero";
      };
    };
    class LogOfNegative : public std::exception
    {
      virtual const char * what() const throw()
      {
        return "LogarithmicTools::LogOfNegative";
      };
    };

    inline double log(const double x) {
      if (x==0) {
        throw LogOfZero();
      };
      if (x<=0) {
        throw LogOfNegative();
      };
      const double ans = std::log(x);
      if ( Lester::linf(ans) == -1 )
      {
        // negative infinity
        throw LogOfZero();
      }
      return ans;
    };

    /// If you have a list of numbers {n1,n2,n3,...} that are so large (or small) that you can only hold them by their logs {l1,l2,l3,...} and what you want to know is the log of their sum: lt=log(n1+n2+n3), then this is the class to use.  The method never exponentiates the individual logs.

    /// The following class keeps a running total incremented after each new number added.  For best results, add numbers in order smallest to largest
    class IncrementalAdder {
    private:
      double m_lt;
      bool m_empty;
    public:
      void reset() {
        m_empty = true;
      };
      IncrementalAdder() :
        // yes I know m_lt has not been initialised!
        m_empty(true) {
      };
      IncrementalAdder(const double initialLog) :
        m_lt(initialLog),
        m_empty(false) {
      };
      /// bear in mind the following method throws LogOfZero if used too soon!
      double logOfTotal() const {
        if (m_empty) {
          throw LogOfZero();
        } else {
          return m_lt;
        };
      };
      void addNumberWhoseLogIs(const double anotherLog) {
        assert(Lester::lfin(anotherLog));
        if (m_empty) {
          m_empty=false;
          m_lt=anotherLog;
        } else {
          const bool totIsLargest = (m_lt >= anotherLog);
          const double la = (totIsLargest?m_lt:anotherLog);
          const double lb = (totIsLargest?anotherLog:m_lt);
          assert(la>=lb);
          const double one= 1;
          const double exponent=std::exp(lb-la);
          assert(exponent<=1);
          m_lt = la + std::log(one+exponent);
        };
      };
    };



  }


}


#endif
