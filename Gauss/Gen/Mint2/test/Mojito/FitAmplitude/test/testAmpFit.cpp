// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Neg2LL.h"
#include "Mint/EventList.h"

#include "Mint/PdfBase.h"

#include "TRandom.h"

#include <iostream>

#include "Mint/FitComplex.h"
#include "Mint/FitAmplitude.h"

#include "Mint/DalitzEventList.h"
#include "Mint/DalitzEventPattern.h"

#include "TH1D.h"
#include "TFile.h"

#include "Mint/FitAmpSum.h"

#include "Mint/CLHEPSystemOfUnits.h"

#include "Mint/AllPossibleSij.h"
#include "Mint/PlotSet.h"

using namespace std;
using namespace MINT;

class TimePdf : public PdfBase<double>{
  FitParameter tau;
public:
  TimePdf() : tau("tau"){}
  double getVal(double& t){
    return exp(-t/tau)/tau;
  }
};


int toyFit(){

  NamedParameter<int> N("N");
  EventList<double> times;
  NamedParameter<double> generatedTime("generatedTime");

  for(int i = 0; i < N; i++){
    times.Add(gRandom->Exp(generatedTime));
  }

  TimePdf myTimePdf;

  Neg2LL fcn(myTimePdf, times);

  Minimiser mini(&fcn);
  mini.doFit();


  cout << "Done. Fitted " << N << " events"
       << " generated with mean lifetime "
       << generatedTime
       << ". Result above."
       << endl;

  return 0;
}

int testComplex(){
  counted_ptr<FitComplex> fc(FitComplexMaker("test"));
  cout << fc->getVal() << endl;
  fc->print();

  return 0;
}
int testFitAmplitude(){
  //  DalitzEventPattern pdg(421, 321, -321, 211, -211);

  NamedParameter<int> Nevents("Nevents", 10);
  NamedParameter<int> EventPattern("Event Pattern", 421, 321, -321, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());
  
  cout << " using the following event pattern (mother + final state daughters): "
       << pdg << endl;

  AllPossibleSij three(3);
  three.print();
  cout << endl;

  AllPossibleSij four(4);
  four.print();
  cout << endl;



  FitAmplitude::AutogenerateFitFile();


  DalitzEventList eventList;
  eventList.generatePhaseSpaceEvents(Nevents, pdg);

  //  DalitzEvent* fullEvent = (DalitzEvent*) eventList.currentEvent();
  DalitzEvent fullEvent(pdg, (TRandom*)0);

  //  double GeV2 = GeV*GeV;
  //  int bins = 100;

  FitAmplitude fa("D0->phi(1020)0(->K+,K-),rho(770)0(->pi+,pi-)");

  cout << fa << endl;

  FitAmpSum fas(pdg);

  std::vector<int> sijIndices;
  sijIndices.push_back(1);
  sijIndices.push_back(2);

  //  TH1D* Ha = eventList.makePlot(sijIndices,  fa.name().c_str(), &fa);
  //TH1D* Has = eventList.makePlot(sijIndices,  "sum" , &fas);

  /*
  cout << " making all plots: " << endl;
  PlotSet ps= eventList.makeAllPlots("all_sum", &fas);
  ps.save("plotList.root");
  cout << "made all plots " << endl;

  TFile f("histos.root", "RECREATE");
  f.cd();
  //  Ha->Write();
  Has->Write();
  f.Close();
  cout << "all OK" << endl;
  
  */
  cout << " generated a lot of events, now am about to throw them away"
       << endl;

  PlotSet ps= eventList.makeAllPlots("all_sum", &fas);
  ps.save("plotList.root");

  cout << "made all plots " << endl;

  for(unsigned int i=0; i < eventList.size(); i++){
    double a = fa.Prob(eventList[i]);
    double as = fas.Prob(eventList[i]);
    if(i < 20 || 0 == i%(Nevents/10)){
      cout << i << ") " 
	   << eventList[i].phaseSpace() 
	   << endl;
      eventList[i].print();
      cout << " fa.getVal() = " << a << endl;
      cout << " fas.getVal() = " << as << endl;
      cout << endl;
    }
  }

  eventList.save();
 
  cout << "saved list as ntuple, now reading it back" << endl;
  DalitzEventList readBackList;
  bool readStatus = readBackList.fromFile();
  cout << " OK, seem to have survived that. readStatus = " << readStatus
       << endl;
  cout << " now looping and comparing events " << endl;

  for(unsigned int i=0; i< readBackList.size() && i < 5; i++){
    cout << " original: " << eventList[i] << endl;
    cout << " the copy: " << readBackList[i] << endl;
    cout << " ---------------------------------- "
	 << endl;
  }

  
  return 0;

}
int main(){
  return testFitAmplitude();
  //return testComplex();
  //  return toyFit();
}
//
