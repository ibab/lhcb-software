// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include <iostream>
#include <ctime>

#include "TError.h"

#include "Mint/DalitzEvent.h"
#include "Mint/DalitzEventList.h"

#include "Mint/DalitzBWArea.h"
#include "Mint/DalitzBWBox.h"
#include "Mint/MappedDalitzBWArea.h"
#include "Mint/DalitzCoordinate.h"

#include "Mint/FitAmpSum.h"
#include "Mint/PlotSet.h"
#include "Mint/DalitzPdfBase.h"

#include "Mint/Utils.h"

#include "Mint/NamedParameter.h"

#include "Mint/Amplitude.h"
#include "Mint/ReturnWeight.h"

#include "TRandom3.h"

using namespace std;

class AmpsPdf
  : public DalitzPdfBase
{
protected:
  FitAmpSum _amps;
public:
  double un_normalised(){
    complex<double> cval = _amps.getVal();
    return cval.real()*cval.real() + cval.imag()*cval.imag();
  }

  AmpsPdf(IDalitzEventAccess* events=0) 
    : DalitzPdfBase(events)
    , _amps(this)
  {
  }
  
};

int testBWBoxesWithAmpSum(){
  time_t t0 = time(0);

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<int> RandomSeed("RandomSeed", 0);

  TRandom3 rnd((int)RandomSeed);
  gRandom = &rnd;

  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  DalitzEventList eventList;

  DalitzEventList dummyEventList;
  //  dummyEventList.generatePhaseSpaceEvents(Nevents, pdg);
  dummyEventList.generatePhaseSpaceEvents(1, pdg);


  FitAmpSum fas(&dummyEventList);

  DalitzBWBoxSet boxes(fas.makeBWBoxes(pdg));

  
  bool noPhaseSpace = false;
  for(int i=0; i< Nevents; i++){
    int printEvery  = 1000;
    if(i < 1000) printEvery=100;
    bool printout = ( i%printEvery == 0 || i < 100);
    if(printout){
      cout << " made event number " << i << endl;
      cout << "   current list size " << eventList.size() << endl;
      double dt = difftime(time(0), t0);
      cout << "     this took " << dt << "s" << endl;
      if(dt > 0) cout << "     or " << eventList.size()/dt << "evt/s" << endl;
    }
    counted_ptr<DalitzEvent> evt(boxes.makeEventForOwner());
    if(printout) cout << "tried one event " << evt << endl;
    if(evt){
      bool largeWeight = (false && evt->getWeight() > 1.e-7) && (! noPhaseSpace);
      fas.setEvent(&(*evt));

      if(largeWeight){
	cout << "rediculous ";
      }
      if(largeWeight || printout){
	cout << " weight = " << evt->getWeight()
	     << " boxes.genValueNoPs " << boxes.genValueNoPs(*evt)
	     << " fas.RealVal() " << fas.RealVal()
	     << "." << endl;
	cout << " largest Amp ";
	fas.printLargestAmp();
	cout << endl;
      }
      fas.resetEventRecord();

      if(i < 100 || i%1000==0 ){
	cout << " adding event with weight " << evt->getWeight() << endl;
      }
      fas.resetEventRecord();
      eventList.Add(*evt);
    }
  }
  ReturnWeight w(&eventList);

  //eventList.throwAwayData(&w);

  for(unsigned int i=0; i< dummyEventList.size(); i++){
    //    dummyEventList[i].setWeight(boxes.genValue(dummyEventList[i]));
    dummyEventList[i].setWeight(dummyEventList[i].phaseSpace());
  }


  PlotSet psp= eventList.makeAllPlots("AmpPlots");
  psp.save("AmpPlots.root");
  PlotSet psp2= dummyEventList.makeAllPlots("AmpPlotsWeight");
  psp2.save("AmpPlotsWeight.root");

  std::string fname = "eventList" 
    + anythingToString(Nevents/1000) +"k_seed" 
    + anythingToString((int)RandomSeed) + ".root";

  eventList.save(fname);
  cout << " the filename is " << fname << " and the format"
       << " is that of a root ntuple." << endl;
  double dt = difftime(time(0), t0);
  cout << " Done. This took " << dt/60.0 << "min" << endl;
  if(dt > 0) cout << "  - or  " << eventList.size()/dt << "evt/s" << endl;
  return 0;
}



int main(){
  //  return testBoxes();

  return testBWBoxesWithAmpSum();
}

//
