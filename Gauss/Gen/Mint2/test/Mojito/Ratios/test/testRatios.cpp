// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/CLHEPPhysicalConstants.h"

#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpSum.h"

#include "Mint/DalitzEvent.h"

#include "Mint/AmpRatios.h"

#include "Mint/cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"

#include <iostream>

using namespace std;
using namespace MINT;

#include "TRandom2.h"
#include <ctime>

int ampRatiosTest(){
  time_t startTime = time(0);

  TRandom2 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed(0);
  gRandom = &ranLux;

  FitAmplitude::AutogenerateFitFile();

   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());


  FitAmpSum amps(pdg);
  // IMPORTANT: figure out why amps(pdg) leads to crash in AmpRatios

  AmpRatios rats(pdg);

  rats.getRatios(amps);
  
  cout << "this took " << (time(0) - startTime) << " s" << endl;
  return 0;
}


int main(){

  return ampRatiosTest();

}
//
