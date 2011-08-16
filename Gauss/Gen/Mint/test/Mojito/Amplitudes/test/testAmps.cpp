// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include <iostream>
#include "Mint/NamedParameter.h"
#include "Mint/FitParameter.h"

#include "Mint/IDalitzEventList.h"
#include "Mint/DalitzEventList.h"
#include "Mint/DalitzEventPattern.h"

#include "Mint/ParticlePropertiesList.h"

#include "Mint/DecayTree.h"

#include "Mint/Amplitude.h"

#include "Mint/SpinFactorMaker.h"

#include "Mint/EventList.h"

#include "Mint/DecayTreeComparisons.h"

using namespace std;
using namespace MINT;

int testAmps(){

  DalitzEventPattern pdg(421, 321, -321, 211, -211);
  int Nevents=5;

  cout << "making two decay trees for testing: " << endl;
  DecayTree dt1(421);
  dt1.addDgtr(313)->addDgtr(321, -211);
  dt1.addDgtr(-321, 211);
  sortBySVPAT(dt1);

  DecayTree dt2(421);
  dt2.addDgtr(-321, 10323)->addDgtr(321, 113)->addDgtr(211,-211);

  cout << "here they are\n" << dt1 << "\n\n" << dt2 << "\n" << endl;
  cout << "Now making an event list:" << endl;
  DalitzEventList eventList;
  eventList.generatePhaseSpaceEvents(Nevents, pdg);
  cout << "Got " << eventList.size() << " events" << endl;

  cout << "\nSpin factor checks: PrintAllSpinFactors()" << endl;
  PrintAllSpinFactors();
  cout << endl;

  cout << "\n\n====================================\n" << endl;
  cout << " Finding Spin Factors for Trees\n" << endl;
  cout << "For this tree:\n" << dt2
       << "\n I am now making a SpinFactor"
       << endl;

  Amplitude amp0(dt2, &eventList);

  ISpinFactor* sf2 = SpinFactorMaker(dt2, &amp0);
  cout << " Made SpinFactor. This is normally done"
       << " automatically by the Amplitude Class."
       << endl;
  cout << "\n\n====================================\n" << endl;
  cout << " Finding Amplitudes (with SpinFactors) for Trees\n" << endl;
  cout << "Now I'm going to make an Amplitude for each tree." << endl;
  Amplitude amp(dt1, &eventList);
  cout << " amp 1:\n" << amp << endl;
  Amplitude amp2(dt2, &eventList);
  cout << " amp 2:\n" << amp2 << endl;
  cout << "\n\n====================================\n" << endl;

  cout << "Amplitudes and Events:\n" << endl;
  cout << "\nThese are the events I generated, and"
       << "\n what the Amplitude class makes of them" <<endl;

  eventList.Start();
  int counter=0;
  do{
    cout << " ----- Event number " << counter << "------" << endl;
    if(0 != sf2){
      cout << "spin factor sf2 (from amplitude amp2)" 
	   << amp2.spinFactor()->getVal() << endl;
    }
    cout << " phase space: (from event) "
	 << eventList.currentEvent()->phaseSpace() 
	 << endl;
    cout << "ask the amplitude amp1 to print the event" <<endl;
    amp.getEvent()->print();
    cout << " evaluating amp2: "
	 << " amp2.getVal() = " << amp2.getVal() << endl;
    cout << "\n\n\n" << endl;
    if(eventList.currentEvent()->phaseSpace() > 5.e-18){
      eventList.Delete(); // this is to check that delete doesn't destroy
    }                     // loop-pointers etc.
    counter++;
  }while(eventList.Next());
  cout << "\n\n=============================\n\n" << endl;
  cout << "Printed all events, and also deleted a few to make"
       << " sure that Delete() works without upsetting"
       << " the loop. Check what's left:"
       << endl;
  cout << "\n\n Loop  again: " << endl;
  counter=0;
  eventList.Start();
  while(eventList.Next()){
    cout << " ----- Event number " << counter << "------" << endl;
    cout << counter++ << ") " 
	 << eventList.currentEvent()->phaseSpace() 
	 << endl;
    cout << "amp to print event:" << endl;
    amp.getEvent()->print();
  }

  return 0;
}

int main(){
  return testAmps();
}
//
