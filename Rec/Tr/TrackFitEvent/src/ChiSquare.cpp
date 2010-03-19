// local
#include "Event/ChiSquare.h"
#include "gsl/gsl_cdf.h"

namespace LHCb
{
  double ChiSquare::prob() const
  {
    double val(0) ;
    if (nDoF()>0) {
      const double limit = 1e-15;
      double chi2max = gsl_cdf_chisq_Qinv (limit, nDoF());
      val = chi2() < chi2max ? gsl_cdf_chisq_Q(chi2(),nDoF()) : 0;
    }
    return val ;
  }
}
