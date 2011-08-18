// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include "Mint/Mint/FitParameter/FitParameter.h"
#include "Mint/Mint/NamedParameter/NamedParameter.h"
#include "Mint/Mint/Utils/CLHEPPhysicalConstants.h"

#include "Mint/Mojito/FitAmplitude/FitAmplitude.h"
#include "Mint/Mojito/FitAmplitude/FitAmpSum.h"

#include "Mint/Mojito/DalitzEvents/DalitzEvent.h"

#include "Mint/Mojito/Ratios/AmpRatios.h"

#include "Mint/Mint/Utils/cexp.h"


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

  DalitzEventList eventList;
  cout << "now about ot generate " << 5 << " events" << endl;
  eventList.generatePhaseSpaceEvents(5, pdg);
  eventList.Start();

  FitAmpSum amps(&eventList);
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
