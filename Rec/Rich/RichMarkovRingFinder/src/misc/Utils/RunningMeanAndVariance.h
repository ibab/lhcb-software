#ifndef LESTER_RUNNING_MEAN_AND_VARIANCE_H
#define LESTER_RUNNING_MEAN_AND_VARIANCE_H

namespace Lester {

  struct RunningMeanAndVariance {
    RunningMeanAndVariance() : n(0), mean_n(0), var_n(0) {
    }
    void takeNoteOf(const double x_nPlus1) {
      const double dn = n;

      const double newMean = (dn*mean_n  +  x_nPlus1)/(dn+1);

      const double del = x_nPlus1 - mean_n;
      const double newVariance = (dn-1)/(dn)*var_n   +   del*del/(dn+1);

      ++n;
      mean_n = newMean;
      var_n  = n==1 ? 0 : newVariance;
    }
    double mean() const {
      return mean_n;
    }
    double variance() const {
      return var_n;
    }
    unsigned long long N() const {
      return n;
    }
  private:
    unsigned long long n;
    double mean_n;
    double var_n;
  };

}

#endif
