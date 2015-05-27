// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/FitParameter.h"
#include "Mint/FitParRef.h"
#include "Mint/FitParDependent.h"
#include "Mint/ResonanceProperties.h"
#include "Mint/ResonancePropertiesFitRef.h"

#include "Mint/DalitzEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/CachedByEvent.h"

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

class ResonanceFitChecker : public FitParDependent, public CachedByEvent<double>{

  ResonancePropertiesFitRef _fp;
public:
  ResonanceFitChecker(ResonanceProperties& rp) : _fp(rp, this){}
  
  double getVal(IDalitzEvent& evt){return getValWithCaching(evt);}
  double getNewVal(IDalitzEvent& evt){return _fp.mass();}
};

int testResonanceProperties(){
  ResonanceProperties rp(323);
  ResonancePropertiesFitRef rp_fitRef(rp);

  DalitzEventPattern pat(421, +211, -211, +211, -211);
  DalitzEvent evt1(pat), evt2(pat);

  ResonanceFitChecker rchecker(rp);


  cout << "0 rc changed?" << rchecker.changedSinceLastCall() << endl;

  cout << " getVal " << rchecker.getVal(evt1);
  cout << "1 rc changed? " << rchecker.changedSinceLastCall() << endl;
  cout << " getVal " << rchecker.getVal(evt1);
  cout << "2 rc changed? " << rchecker.changedSinceLastCall() << endl;

  rp.changeMassForDebug(5.0);
  cout << "3 rc changed? " << rchecker.changedSinceLastCall() << endl;
  cout << " getVal " << rchecker.getVal(evt1);
  cout << "4 rc changed? " << rchecker.changedSinceLastCall() << endl;
  cout << " getVal " << rchecker.getVal(evt1);
  cout << "5 rc changed? " 
       << rp_fitRef.changedSinceLastCall() << endl;
  rp_fitRef.rememberFitParValues();
  cout << "after remember values?" << endl;
  cout << "rp changed? " << rp_fitRef.changedSinceLastCall() << endl;
  cout << "rp changed? " << rp_fitRef.changedSinceLastCall() << endl;

}

int testFitPar(){

  FitParameter tau("tau"), mau("mau");
  sumOfTwo s(tau, mau);

  cout << tau << endl;

  FitParRef tauRef(tau);
  s.rememberFitParValues();
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
  s.rememberFitParValues();
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;
  
  tau.setCurrentFitVal( - 500);
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;
  s.rememberFitParValues();
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;
  s.rememberFitParValues();
  mau.setCurrentFitVal( - 500);
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;
  s.rememberFitParValues();
  cout << "changed? " << tauRef.changedSinceLastCall() 
       << "sum changed? " << s.changedSinceLastCall()
       << ", "  << tauRef << ", " << s.getSum() << endl;


  return 0;
}

int main(){
  //return testFitPar();
  return testResonanceProperties();
}
//
