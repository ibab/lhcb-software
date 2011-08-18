// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "Mint/Mint/NamedParameter/NamedParameter.h"
#include "Mint/Mojito/DalitzEvents/DalitzEventList.h"
#include "Mint/Mojito/DalitzEvents/DalitzEventPattern.h"
#include "Mint/Mojito/FitAmplitude/FitAmplitude.h"
#include "Mint/Mojito/FitAmplitude/FitAmpSum.h"

#include "Mint/Mojito/DalitzIntegrator/DalitzMCIntegrator.h"

#include "Mint/Mojito/DalitzEvents/PlotSet.h"

#include <iostream>

using namespace std;
using namespace MINT;

int testIntegrator(){

  FitAmplitude::AutogenerateFitFile();

  NamedParameter<int> Nevents("Nevents", 1000);
  NamedParameter<int> EventPattern("Event Pattern", 421, 310, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  cout << " got event pattern: " << pdg << endl;
  DalitzEventList eventList;
  cout << "now about ot generate " << Nevents << " events" << endl;
  eventList.generatePhaseSpaceEvents(Nevents, pdg);
  eventList.Start();

  FitAmpSum fas(&eventList);

  DalitzMCIntegrator integrator(pdg, &fas);

  double integral = integrator.getVal();

  double sum=0;
  eventList.Start();
  while(eventList.Next()){
    sum += fas.RealVal();
  }
  cout << " sum/(Nevents * integral ) = " << sum/(Nevents * integral ) << endl;
  
  return 0;
}


int main(){
  return testIntegrator();
}

//
