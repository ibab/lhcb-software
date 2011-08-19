// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT

#include "DKsPiPiResonances.h"
#include "ComplexSum.h"

#include "RooGlobalFunc.h"
#include "RooRealVar.h"
#include "RooArgList.h"

#include "TRandom2.h"

#include "RooFitEventInterface.h"
#include "Mint/NamedParameter.h"
#include "Mint/DalitzEventList.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpSum.h"

#include "Mint/counted_ptr.h"

#include <iostream>
#include <complex>

using namespace RooFit;
using namespace std;
using namespace MINT;

int rooFitVsUs(){

  TRandom2 ranLux;

  FitAmplitude::AutogenerateFitFile();

  NamedParameter<int> Nevents("Nevents", 1000);
  NamedParameter<int> EventPattern("Event Pattern", 421, 310, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  cout << " got event pattern: " << pdg << endl;
  DalitzEventList eventList;
  cout << "now about to generate " << Nevents << " events" << endl;
  eventList.generatePhaseSpaceEvents(Nevents, pdg, &ranLux);
  eventList.Start();

  RooFitEventInterface ri(&eventList);

  RooArgList ResonanceList(makeResonance(ri.s12(), ri.s23(), ri.s13()));
  RooArgList ResonanceListCP(makeResonance(ri.s23(), ri.s12(), ri.s13()));

  DalitzEventList anotherList;
  anotherList.generatePhaseSpaceEvents(5, pdg, &ranLux);

  counted_ptr<FitAmpSum> fas(new FitAmpSum(&anotherList));

  fas->setEventRecord(&eventList);

  counted_ptr<FitAmpSum> fasCP = fas->GetCPConjugateSameFitParameters();
  fasCP->setEventRecord(&eventList);

  ComplexSum AmpSumRooFit("AmpSumRooFit", "AmpSumRooFit"
			  , ResonanceList);
  
  ComplexSum AmpSumRooFitCP("AmpSumRooFit", "AmpSumRooFit"
			    , ResonanceListCP);
  int counter=0;
  eventList.Start();
  while(eventList.Next() && counter < 10){
    //cout << "starting event " << counter << endl;
    //eventList.getEvent()->print();

    ri.setAll();
    complex<double> resRooFit = AmpSumRooFit.getCVal();
    complex<double> ourVal = fas->getVal();

    complex<double> resRooFitCP = AmpSumRooFitCP.getCVal();
    complex<double> ourValCP = fasCP->getVal();
    
    complex<double> diff   = ourVal - resRooFit;
    complex<double> diffCP = ourValCP - resRooFitCP;

    if(counter < 10 || counter%1000==0
       //|| abs(diff) > 1.e-10 
       || abs(diffCP) > 1.e-10 
       ){
      cout << "======================================================" << endl;
      cout << "for this event, " << counter << " we get:" << endl;
      
      cout << " RooFit's amplitude sum gives : " << resRooFit
	   << endl;      
      cout << " Our amplitudes give: " << ourVal<< endl;
      cout << " diff = " << diff << endl;
      
      cout << "-------------------------------------------" << endl;

      cout << " RooFit's CP amplitude sum gives : " << resRooFitCP
	   << endl;      
      cout << " Our CP amplitudes give: " << ourValCP << endl;
      cout << " diffCP = " << diffCP << endl;

    }
    counter++;
  }

  //     return 0;

  cout << "done with that, making plots, now" <<endl;
  bool throwAway=false;
  eventList.Start();
  if(throwAway){
    eventList.throwAwayData(fas.get());
    cout << " making all plots: " << endl;
    DalitzHistoSet ps= eventList.histoSet();
    ps.save("plotList.root");
   }else{
    PlotSet ps= eventList.makeAllPlots("all_sum", fas.get());
     ps.save("plotList.root");
   }
  cout << "made all plots " << endl;

  return 0;
}



int main(){

  return rooFitVsUs();

}


//
