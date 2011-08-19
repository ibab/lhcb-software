#include "Mint/FitParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Minimisable.h"
#include "Mint/MinuitParameterSet.h"

using namespace MINT;

class toyFitFun : public Minimisable{
  FitParameter& _fp;
public:
  toyFitFun(FitParameter& fp, MinuitParameterSet* mps) 
  : Minimisable(mps) , _fp(fp){}
  double getVal(){return (_fp - 4.)*(_fp - 4.);}
};

int main(){

  MinuitParameterSet mps_1, mps_2;
  FitParameter x("x", &mps_1), y("y", &mps_2);
  toyFitFun fcn(x, &mps_1);
  Minimiser mini(&fcn);
  mini.doFit();

  return 0;
}
