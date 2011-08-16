// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include "Mint/FitParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Minimisable.h"

using namespace MINT;

class toyFitFun : public Minimisable{
  FitParameter fp;
public:
  toyFitFun() : fp("x"){}
  double getVal(){return (fp - 4.)*(fp - 4.);}
};


int trivialMinimisationExample(){

  toyFitFun f;

  Minimiser mini(&f);
  mini.doFit();

  return 0;
}
//
