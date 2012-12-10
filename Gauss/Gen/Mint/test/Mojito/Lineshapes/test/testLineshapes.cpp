// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
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

int compareDefaultWithCLEO2012(){
  time_t startTime = time(0);
  FitAmplitude::AutogenerateFitFile();
  NamedParameter<string> PlotName("PlotName", (std::string) "histo");
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pat(EventPattern);

  cout << " making Amps for event patern " << pat << endl;
  FitAmpSum Amps(pat, "", "NoCLEO2012");
  FitAmpSum CLEO2012_Amps(pat, "", "OnlyCLEO2012");
  
  SignalGenerator sg(&Amps);
  sg.setWeighted();

  SignalGenerator CLEO2012_sg(&CLEO2012_Amps);
  CLEO2012_sg.setWeighted();

  NamedParameter<double> IntegPrecision("IntegPrecision", 1.e-3);
  FastAmplitudeIntegrator integ(pat, &Amps, &sg
				, gRandom, IntegPrecision);
  DalitzHistoSet histos = integ.histoSet();
  histos.save((std::string) PlotName + ".root");
  histos.draw((std::string) PlotName +  "_");

  FastAmplitudeIntegrator CLEO2012_integ(pat, &CLEO2012_Amps, &CLEO2012_sg
					 , gRandom, IntegPrecision);
  DalitzHistoSet CLEO2012_histos = CLEO2012_integ.histoSet();
  CLEO2012_histos.save("CLEO2012_" + (std::string) PlotName + ".root");
  CLEO2012_histos.draw("CLEO2012_" + (std::string) PlotName +  "_");
  CLEO2012_histos.drawWithFit(histos
			      , "CLEO2012_compare" 
			      + (std::string) PlotName 
			      +  "_");

  cout << "total time for this " << difftime(time(0), startTime)/60 << " min"
       << endl;
  return 0;
}


int main(){

  return compareDefaultWithCLEO2012();

}
//
