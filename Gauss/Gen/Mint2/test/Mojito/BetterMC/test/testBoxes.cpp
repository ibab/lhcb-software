// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include <iostream>
#include <ctime>

#include "TError.h"

#include "Mint/DalitzEvent.h"
#include "Mint/DalitzEventList.h"

#include "Mint/DalitzArea.h"
#include "Mint/DalitzBox.h"
#include "Mint/DalitzBoxSet_Method2.h"
#include "Mint/MappedDalitzArea.h"
#include "Mint/DalitzCoordinate.h"

#include "Mint/FitAmpSum.h"
#include "Mint/PlotSet.h"
#include "Mint/DalitzPdfBase.h"

#include "Mint/Utils.h"

#include "Mint/NamedParameter.h"

#include "TRandom3.h"


using namespace std;
using namespace MINT;

class AmpsPdf
  : public DalitzPdfBase
{
protected:
  FitAmpSum _amps;
public:
  double un_normalised_noPs(IDalitzEvent& evt){
    complex<double> cval = _amps.getVal(evt);
    return cval.real()*cval.real() + cval.imag()*cval.imag();
  }

  AmpsPdf(const DalitzEventPattern& pat) 
    : DalitzPdfBase()
    , _amps(pat)
  {
  }
  
};


int testBoxes(){

  // make list of particles:
  // mother, followed by final states:


  gErrorIgnoreLevel = 2000;

  DalitzEventPattern  pdg(421, 321, -321, 211, -211);

  int Nevents=100000;

  DalitzEventList eventList;
  DalitzArea area(pdg);

  gRandom = new TRandom3;

  for(int i=0; i< Nevents/2; i++){
    if(i%1000 == 0){
      cout << " trying event number " << i << endl;
      cout << " current list size " << eventList.size() << endl;
    }
    counted_ptr<DalitzEvent> evt = area.makeEventForOwner();
    if(evt){
      eventList.Add(*evt);
    }
  }

  /*
  DalitzCoordinate phi(1,2), rho(3,4);
  phi.setMinMax(1.0*GeV*GeV, 1.1*GeV*GeV);
  rho.setMinMax(0.4*GeV*GeV, 0.5*GeV*GeV);

  MappedDalitzArea marea(pdg, phi, rho);

  for(int i=0; i< Nevents; i++){
    if(i%1000 == 0){
      cout << "1 trying event number " << i << endl;
      cout << "1 current list size " << eventList.size() << endl;
    }
    if(marea.makeEvent()){
      eventList.Add(*(marea.getEvent()));
    }
  }
  */

  DalitzCoordinate Kstar1(1,4), Kstar2(2,3);
  Kstar1.setMinMax(0.9*GeV*GeV, 1*GeV*GeV);
  Kstar2.setMinMax(1*GeV*GeV, 1.1*GeV*GeV);

  MappedDalitzArea marea2(pdg, Kstar1, Kstar2);
  for(int i=0; i< Nevents; i++){
    if(i%1000 == 0){
      cout << "2 trying event number " << i << endl;
      cout << "2 current list size " << eventList.size() << endl;
    }
    counted_ptr<DalitzEvent> evt=marea2.makeEventForOwner();
    if(evt){
      eventList.Add(*evt);
    }
  }

  /*
  DalitzCoordinate Kstar3(1,4), Kstar4(2,3);
  Kstar3.setMinMax(0.9*GeV*GeV, 1.1*GeV*GeV);
  Kstar4.setMinMax(0.5*GeV*GeV, 0.7*GeV*GeV);

  MappedDalitzArea marea3(pdg, Kstar3, Kstar4);
  bool rmOverlap = true;
  for(int i=0; i< Nevents; i++){
    if(i%1000 == 0){
      cout << "2 trying event number " << i << endl;
      cout << "2 current list size " << eventList.size() << endl;
    }
    if(marea3.makeEvent()){
      DalitzEvent evt(*(marea3.getEvent()));
      if(rmOverlap && marea2.isInside(evt)) continue;
      eventList.Add(evt);
    }
  }
  */

  PlotSet psp= eventList.makeAllPlots("AmpPlots");
  psp.save("AmpPlots.root");
  std::string fname = "eventList.root";
  eventList.save(fname);
  cout << " the filename is " << fname << " and the format"
       << " is that of a root ntuple." << endl;
  return 0;
}

int testBoxGeneration(){
  time_t startTime = time(0);

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<int> RandomSeed("RandomSeed", 0);
   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  gRandom->SetSeed(RandomSeed);

  DalitzEventList eventList;
  eventList.generatePhaseSpaceEvents(1, pdg);

  //  AmpsPdf amps(&eventList);
  AmpsPdf amps(pdg);

  DalitzBox phaseSpaceBox(pdg, &amps);
  cout << " made box " << endl;

  /*
  phaseSpaceBox.getReady();
  PlotSet singleBoxPlots = 
      phaseSpaceBox.eventList().makeAllPlots("singleBoxPlots");
  singleBoxPlots.save("singleBoxPlots.root");
  */

  FitAmpSum Amps(pdg);
  
  double nSigma = 3;
  //  double maxWidth = 2*  2*1*GeV * (nSigma*75)*MeV; // in m^2
  double maxWidth = 2*1*GeV * (nSigma*75.0)*MeV; // in m^2
  // this corresponds to a width (in m, rather than m^2) of
  // nSigma * 75 MeV for a particle of 1GeV mass.
  //  DalitzBoxSet_Method2 boxes(Amps.makeBoxes(nSigma).split(2).splitIfWiderThan(maxWidth), pdg);
  //DalitzBoxSet_Method2 boxes(Amps.makeBoxes(nSigma).splitIfWiderThan(maxWidth), pdg);
  DalitzBoxSet_Method2 boxes(Amps.makeBoxes(pdg), pdg);

  cout << " made " << boxes.size() << " boxes " << endl;


  boxes.setPDF(&amps);
  cout << " set PDF" << endl;

  time_t generationStartTime = time(0);
  double setupTime = -9999.0;

  for(int i=0; i< Nevents; i++){
    if(i < 10 || 0 == i%100){
      double dt = difftime(time(0), generationStartTime);
      double evtsPerMinute=0;
      if(dt > 0.5) evtsPerMinute = (i/dt)*60;
      cout << "generating event number " << i
	   << "\n    after initial setup of " << setupTime/60 << " min"
	   << "\n    we've been now running for another " << dt/60 << " min"
	   << "\n    we're currently doing "  << evtsPerMinute
	   << " event per min" 
	   << endl;
    }
    counted_ptr<DalitzEvent> evtPtr = boxes.generateEventForOwner();
    if(0 != evtPtr) eventList.Add(*evtPtr);
    if(i==0) setupTime = difftime(time(0), startTime);
  }
  PlotSet psp= eventList.makeAllPlots("AmpPlots");
  std::string boxname = "BoxGeneration" 
    + anythingToString(Nevents/1000) +"k_seed" 
    + anythingToString((int)RandomSeed) + ".root";

  psp.save(boxname);
  std::string fname = "eventList" 
    + anythingToString(Nevents/1000) +"k_seed" 
    + anythingToString((int)RandomSeed) + ".root";
  eventList.save(fname);

  double deltaTFinal = difftime(time(0), startTime);
  std::cout << " this took " << deltaTFinal/60.0 << " min";
  if(deltaTFinal > 0){
    std::cout << " rate = " << (eventList.size()/deltaTFinal) << " evts/s"
	      << " or " << (eventList.size()/deltaTFinal) *60 << " evts/m"
	      << " or " << (eventList.size()/deltaTFinal) *60.*60./1000. 
	      << "k evts/h";
  }
  std::cout << std::endl;
  
  std::cout << "Done - and exit." << endl;

  return 0;
}
int main(){
  return testBoxGeneration();
  //return testBoxes();
}

//
