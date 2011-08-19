#ifndef GENERALISED_PARETO_HH
#define GENERALISED_PARETO_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
// assuming threshold == 0, shift parameters if necessary

#include <vector>
#include "Mint/Minimisable.h"
#include "Mint/FitParameter.h"

namespace MINT{
  double generalisedPareto_cumulative(double y, double xi, double sigma_bar);
  
  double generalisedPareto_pdf(double y, double xi, double sigma_bar);
  double generalisedPareto_logPdf(double y, double xi, double sigma_bar);
  double generalisedPareto_xiFromMeanRMS(double mean, double rms);
  
  double generalisedPareto_sigmaFromMeanRMS(double mean, double rms);
  
  double generalisedPareto_yFromCL(double CL, double xi, double sigma_bar);
  
  double generalisedPareto_limit(double xi, double sigma_bar);
  
  double generalisedPareto_estimateMaximum(const std::vector<double>& input
					   , double CL = 0.001);
  double generalisedPareto_estimateMaximum(std::vector<double> input
					   , double CL
					   , double& actualMax
					   , double& paretoMax
					   , int numEvents = -9999
					   );
  
  class minimisePareto : public Minimisable{
    static MinuitParameterSet _mps;
    std::vector<double>& _data;
    double _threshold;
    FitParameter _xi, _sigma;
  public:
    minimisePareto(std::vector<double>& data, double threshold
		   , double xiInit, double sigInit);
    virtual double getVal();
    double getXi() const;
    double getSigma() const;
  };
  
}//namespace MINT
#endif
//

