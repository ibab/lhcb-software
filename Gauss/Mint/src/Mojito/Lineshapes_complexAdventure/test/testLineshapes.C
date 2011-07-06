// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "FitParameter.h"
#include "NamedParameter.h"
#include "DalitzEventPattern.h"
#include "FitAmpSum.h"
#include "FitAmplitude.h"
#include "DalitzHistoSet.h"

#include "Minimisable.h"
#include "Minimiser.h"

#include "SignalGenerator.h"
#include "FastAmplitudeIntegrator.h"

#include "CoherenceFactor.h"

#include <ctime>

#include <iostream>
#include <string>

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
  NamedParameter<string> PlotName("PlotName", (std::string) "histo");
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pat(EventPattern);

  cout << " making Amps for event patern " << pat << endl;
  FitAmpSum Amps(pat);
  cout << " making AmpsBar" << endl;
  
  SignalGenerator sg(&Amps);
  sg.setWeighted();

  NamedParameter<double> IntegPrecision("IntegPrecision", 1.e-3);
  FastAmplitudeIntegrator integ(pat, &Amps, &sg, gRandom, IntegPrecision);
  DalitzHistoSet histos = integ.histoSet();
  histos.save((std::string) PlotName + ".root");
  histos.draw((std::string) PlotName +  "_");
  cout << "total time for this " << difftime(time(0), startTime)/60 << " min"
       << endl;
  return 0;
}


int main(){

  return testFracFit();

}
//
