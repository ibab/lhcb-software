// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT
#include "FitParameter.h"
#include "Minimisable.h"
#include <string>

class toyFitFun : public Minimisable{
  FitParameterli fp;
 public:
  toyFitFun(std::string name="x") : fp(name){}
  double getVal(){return (double) fp * (double) fp;}
};

