// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/FitParameter.h"
#include "Mint/FitParRef.h"
#include "Mint/FitParDependent.h"

#include <iostream>

using namespace std;
using namespace MINT;


class sumOfTwo : public FitParDependent{
  FitParRef a, b;
public:
  sumOfTwo(const FitParameter& fa, const FitParameter& fb)
    : a(fa, this), b(fb, this){}
  
    double getSum(){ return a + b; }
};

int testFitPar(){

  FitParameter tau("tau"), mau("mau");
  sumOfTwo s(tau, mau);

  cout << tau << endl;

  FitParRef tauRef(tau);

  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;

  cout << "changed? " << tauRef.changedSinceLastCall() << ", " 
       << tauRef << endl;

  tau.setCurrentFitVal( - tauRef);
  cout << "tauRef should now be " << (double) tau << endl;
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;
  
  tau.setCurrentFitVal( - 500);
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;
  mau.setCurrentFitVal( - 500);
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;


  return 0;
}

int main(){
  return testFitPar();
}
//
