// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/GeneralisedPareto.h"
#include "Mint/Minimiser.h"

#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace MINT;

/*
http://www.isse.ucar.edu/extremevalues/tutorial/Appendix.html#subsec:GPD

Formula:

G(z) = exp{ - [1 + xi*((z-mu)/sigma)]^(-1/xi) }

for large enough threshold u, cumulative distribution of y:=(x-u):

H(y) = 1 - (1 + xi*y/sigma_bar)^(-1/xi) <-- I use this
                                                                                                                        
with sigma_bar = sigma + xi*(u - mu)
*/

double MINT::generalisedPareto_cumulative(double y, double xi, double sigma_bar){

  return 1 - pow(1.0 + xi*y/sigma_bar, -1.0/xi);
}
double MINT::generalisedPareto_pdf(double y, double xi, double sigma_bar){
  double d = 1.0 + xi*y/fabs(sigma_bar);
  if(d < 0) return 0;
  return (1./fabs(sigma_bar))*pow(1.0 + xi*y/fabs(sigma_bar), -1.0/xi - 1.0);
}
double MINT::generalisedPareto_logPdf(double y, double xi, double sigma_bar){
  double d = 1.0 + xi*y/fabs(sigma_bar);
  if(d < 0) return -99999;
  return -log(fabs(sigma_bar)) + log(1.0 + xi*y/fabs(sigma_bar)) *(-1.0/xi - 1.0);
}

double MINT::generalisedPareto_xiFromMeanRMS(double mean, double rms){
  // assuming threshold == 0, shift parameters if necessary
  return 0.5*(1.0 - mean*mean/(rms*rms));
}
double MINT::generalisedPareto_sigmaFromMeanRMS(double mean, double rms){
  // assuming threshold == 0, shift parameters if necessary
  return 0.5*mean*(1.0 + mean*mean/(rms*rms));
}

double MINT::generalisedPareto_yFromCL(double CL, double xi, double sigma_bar){

  return sigma_bar/xi * ( pow( 1.0 - CL , -xi) - 1.0 );
}

double MINT::generalisedPareto_limit(double xi, double sigma_bar){
  if(xi >=0){
    cout << " MINT::generalisedPareto_limit: Limit only for negative xi."
	 << " This xi = " << xi << endl;
    return -9999.0;
  }
  return -sigma_bar/xi;
}

double MINT::generalisedPareto_estimateMaximum(const std::vector<double>& input
					 , double CL){
  double paretoMax=-9999;
  double actualMax=-9999;
  return MINT::generalisedPareto_estimateMaximum(input
						 , CL
						 , paretoMax
						 , actualMax);
}

MinuitParameterSet MINT::minimisePareto::_mps;    

MINT::minimisePareto::minimisePareto(std::vector<double>& data, double threshold
				     , double xiInit, double sigInit)
  : _data(data)
  , _threshold(threshold)
  , _xi("xi", 0, xiInit, fabs(xiInit)/2.0 + 0.1, 0, 0, &_mps)
  , _sigma("sigma", 0, fabs(sigInit), fabs(sigInit)/2.0 + 0.1, 0, 0, &_mps)
{
  this->setPset(&_mps);
}
double MINT::minimisePareto::getVal(){
  double sum=0;
  for(unsigned int i=0; i < _data.size(); i++){
    double x = _data[i] - _threshold;
    if(x > 0) sum -= 2*generalisedPareto_logPdf(x, _xi, _sigma);
  }
  return sum;
}

double MINT::minimisePareto::getXi() const{ return _xi;}
double MINT::minimisePareto::getSigma() const{ return _sigma;}

double MINT::generalisedPareto_estimateMaximum(std::vector<double> input_a
					       , double CL
					       , double& actualMax
					       , double& paretoMax
					       , int numEvents
					       ){
  bool dbThis=false;
  bool verbose=true;

  double max=-9999;
  std::vector<double> input;
  for(unsigned int i=0; i < input_a.size(); i++){
    if(i > 0 && i%200 == 0){
      input.push_back(max);
      max = -9999;
    }
    if(input_a[i] > max) max=input_a[i];
  }
  
  sort(input.begin(), input.end());

  unsigned int n=-9999;
//  unsigned int n=9999;

  if(numEvents > 0){
    n = (unsigned int) numEvents;
  }else{
    // let's take 0.2% of events, but at least 100, at most 1000:
    n = input.size()/50; // originally /500,
    if(n < 100) n=100;
    if(n > 1000) n=1000;
  }
  if(n > input.size()) n = input.size()-1;
  if(n == 0){
    cout << "MINT::generalisedPareto_estimateMaximum: "
	 << input.size() << " too few events."
	 << endl;
    return -9999;
  }
  double sum=0, sumsq=0;
  unsigned int counter=0;
  double lastVal=-9999;
  std::vector<double>::reverse_iterator it = input.rbegin();
  double maxVal = *it;
  for( ;
      it != input.rend() && counter < n;
      it++, counter++){
    if(dbThis) cout << *it << endl;
    lastVal = *it;
    sum += lastVal;
    sumsq += (lastVal) * (lastVal);
  }
  double threshold = 0.5*(lastVal + *it); // *it is one event beyond lastVal;

  double mean = sum/((double)n);
  double meansq = sumsq/((double)n);
  double var = meansq - mean*mean;
  double rms = sqrt(var);

  double xi = MINT::generalisedPareto_xiFromMeanRMS(mean - threshold, rms);
  double sg = MINT::generalisedPareto_sigmaFromMeanRMS(mean - threshold, rms);

  if(sg < -1.5*xi*(maxVal-threshold)) sg = fabs(1.5*xi*(maxVal-threshold));

  cout << "xi, sg before fit: xi = " << xi << ", sg = " << sg << endl;
  minimisePareto ll(input, threshold, xi, sg);
  Minimiser mini(&ll);
  mini.doFit();
  xi = ll.getXi();
  sg = ll.getSigma();
  cout << "xi, sg after fit: xi = " << xi << ", sg = " << sg << endl;

  double limity = -9999;
  if(xi < 0){
    limity = MINT::generalisedPareto_limit(xi, sg);
  }else{
    limity = MINT::generalisedPareto_yFromCL(CL, xi, sg);
  }

  double limit = limity + threshold;

  if(verbose || dbThis){
    cout << "MINT::generalisedPareto_estimateMaximum with CL = " << CL << endl;
    cout << "\n maxVal " << maxVal
	 << "\n mean   " << mean
	 << "\n rms    " << rms
	 << "\n threshold " << threshold
	 << "\n xi     " << xi
	 << "\n sg     " << sg
	 << "\n limity " << limity
	 << "\n limit  " << limit
	 << endl;
  }
  if(limit < maxVal){
    cout << "MINT::generalisedPareto_estimateMaximum with CL = " << CL
	 << ": Something clearly went wrong"
	 << "\n Estimated upper limit smaller than actual upper limit:"
	 << " actual: " << maxVal << ", estimated " << limit << endl;
    return maxVal;
  }
  actualMax = maxVal;
  paretoMax = limit;
  return limit;    
}
//
