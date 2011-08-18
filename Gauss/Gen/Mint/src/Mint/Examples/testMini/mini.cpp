#include "Mint/Mint/FitParameter/FitParameter.h"
#include "Mint/Mint/Minimiser/Minimiser.h"
#include "Mint/Mint/Minimiser/Minimisable.h"

using namespace MINT;

class toyFitFun : public Minimisable{
  FitParameter _fp;
public:
  toyFitFun() : _fp("x"){}
  double getVal(){return (_fp - 4.)*(_fp - 4.);}
};

int main(){

  toyFitFun f;
  Minimiser mini(&f);
  mini.doFit();

  return 0;
}
