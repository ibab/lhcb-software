// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

/* to run this:
   > make
   and then either:

   > testMinimiser < testMinimiser.txt
   to test the minimisation, or
   
   > testMinimiser < fitparameters.txt
   to see how fit parameters are set, or
   
   > testMinimiser < do_fit_k0spipi
   to check that FitParameter is compatible
   with Mikhail's file format.
   
*/


#include <iostream>

#include "Mint/NamedParameter.h"
#include "Mint/Minimisable.h"
#include "Mint/FitParameter.h"
#include "Mint/Minimiser.h"

using namespace std;
using namespace MINT;

class parabola : public Minimisable{
  FitParameter _fp;
  NamedParameter<double> _min, _curv;
public:
  parabola() : _fp("x"), _min("min"), _curv("curv"){}
  const FitParameter& fp() const{return _fp;}
  double min()const{return _min;}
  double curv()const{return _curv;}
  double getVal(){return _curv*0.5*(_fp - _min)*(_fp - _min);}
};


int MinimiseParabola(){
  /* to run this:
     > make
     > testMinimese < testMinimise.txt
  */

  parabola p;
  {
    // these curly brackets are not needed. They
    // are just there to see if FitParameter
    // survives after Minimiser has gone out
    // of scope.
    Minimiser mini(&p);
    mini.doFit();
  }
  cout << "\n Done. If it worked, the result should be:"
       << " mean = " << (double) p.min()
       << ", error = " << sqrt(2./p.curv()) <<"."
       << endl;
  cout << " The FitParameter is now:\n\t";
  FitParameter::printResultFormat();
  cout <<"\t" << p.fp() << "\n\n" << endl;

  return 0;
}

int testFitParameter(){
  /* to run this:
     > make
     > testMinimise < fitparameters.txt
     
     or, to test compatibility with Mikhail's file:
     > testMinimise < do_fit_k0spipi
  */
  
  FitParameter AmpX_K1680("AmpX_K*1680");
  FitParameter PhiX_K1680("PhiX_K*1680");
  FitParameter PhiX_K892("PhiX_K*892");

  cout << "declared a couple of FitParameters"
       << " here they are:\n"
       << AmpX_K1680 << "\n"
       << PhiX_K1680 << "\n"
       << PhiX_K892  << endl;

  return 0;
}


int main(){
  NamedParameter<string> doWhat("which program to run");
  if("Minimise" == (string) doWhat){
    return MinimiseParabola();
  }else{
    return testFitParameter();
  }
}

