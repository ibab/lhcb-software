// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/FitAmpSum.h"
#include "Mint/FitAmplitude.h"
#include "Mint/DalitzHistoSet.h"

#include "Mint/Minimisable.h"
#include "Mint/Minimiser.h"

#include "Mint/SignalGenerator.h"
#include "Mint/FastAmplitudeIntegrator.h"

#include "Mint/CoherenceFactor.h"

#include <ctime>

#include <iostream>

using namespace std;
using namespace MINT;

class FracLL : public Minimisable{
  FastAmplitudeIntegrator* _integ;
public:
  FracLL(FastAmplitudeIntegrator* integ) : _integ(integ){
    _integ->getVal();//makes sure we are initialises.
  }
  double getVal(){
    return _integ->getFractionChi2();
  }
};

int testFracFit(){
  time_t startTime = time(0);
  FitAmplitude::AutogenerateFitFile();

  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pat(EventPattern);

  cout << " making Amps" << endl;
  FitAmpSum Amps(pat);
  cout << " making AmpsBar" << endl;
  
  SignalGenerator sg(&Amps);
  sg.setWeighted();
  //    SignalGenerator sgbar(AmpsBar.get());
  NamedParameter<double> IntegPrecision("IntegPrecision", 1.e-3);
  FastAmplitudeIntegrator integ(pat, &Amps, &sg, gRandom, IntegPrecision);
  cout << "integrator value: " << integ.getVal() << endl;
  cout << "now doing the fit" << endl;
  FracLL f(&integ);
  Minimiser mini(&f);
  mini.doFit();
  integ.doFinalStats();
  cout << " done fit - now plotting" << endl;
  DalitzHistoSet histos = integ.histoSet();
  histos.save("histos.root");
  histos.draw("histoPlots_");
  cout << "total time for this " << difftime(time(0), startTime)/60 << " min"
       << endl;
  return 0;
}


int main(){

  return testFracFit();

}
//
