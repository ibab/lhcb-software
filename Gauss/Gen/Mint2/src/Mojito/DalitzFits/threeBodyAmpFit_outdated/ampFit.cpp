// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Neg2LL.h"
#include "Mint/Neg2LLSum.h"
#include "Mint/DalitzEventList.h"

#include "Mint/CLHEPPhysicalConstants.h"


#include "Mint/PdfBase.h"
#include "Mint/DalitzPdfBase.h"
#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpSum.h"

#include "Mint/PlotSet.h"
#include "Mint/DalitzEvent.h"


#include "Mint/cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"

#include <iostream>

using namespace std;

class AmpsPdf
  : public DalitzPdfBase
{
protected:
  FitAmpSum _amps;
public:
  double un_normalised_noPs(){
    complex<double> cval = _amps.getVal();
    return cval.real()*cval.real() + cval.imag()*cval.imag();
  }

  AmpsPdf(IDalitzEventAccess* events=0) 
    : DalitzPdfBase(events)
    , _amps(this)
  {
  }
  
};

#include "TRandom2.h"
#include <ctime>

int ampFit(){
  time_t startTime = time(0);

  TRandom2 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed(0);
  gRandom = &ranLux;

  FitAmplitude::AutogenerateFitFile();

  NamedParameter<string> InputFileName("InputFileName", (std::string) "");
  bool generateNew = (std::string) InputFileName == "";

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  doScan("doScan", 0);
   
  NamedParameter<int> EventPattern("Event Pattern", 421, 310, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());
  
  cout << " got event pattern: " << pdg << endl;
  DalitzEventList eventList;
  cout << "now about ot generate " << Nevents << " events" << endl;
  //  eventList.generatePhaseSpaceEvents(Nevents, pdg, &ranLux);
  //  eventList.Start();

  if(! generateNew){
    eventList.fromFile(InputFileName);
  }
  AmpsPdf amps(&eventList);

  if(generateNew){
    eventList.generateEvents(Nevents, pdg, &amps, &ranLux);
  }

  PlotSet psp= eventList.makeAllPlots("AmpPlots");
  psp.save("AmpPlots.root");

  Neg2LL<IDalitzEvent> fcn(&amps, &eventList);

  Minimiser mini(&fcn);
  mini.doFit();
  mini.printResultVsInput();

  if(doScan){
    Double_t arglist[100] = {0};
    Int_t ierflg=0;
    
    TFile fscan("scanGamma.root", "RECREATE");
    for(int i=0; i<3; i++){
      arglist[0]=i; arglist[1]=100; arglist[2]=-100.; arglist[3]=100;
      mini.mnexcm("SCAN", arglist, 1, ierflg);
      //TCanvas *scanCan1 = new TCanvas("ScanGamma","ScanGamma");
      cout << "did scan, made canvas " << endl;
      // scanCan1->cd();
      TGraph *gr1 = (TGraph*)mini.GetPlot();
      if(0 == gr1){
	cout << " didn't get plot " << endl;
      }else{
	cout << "got plot!" << endl;
	//  gr1->Draw("al");
	//cout << "drew plot" << endl;
	fscan.cd();
	gr1->Write();
      }
    }
    fscan.Close();
  }

  cout << " saving list: " << endl;
  eventList.save();

  cout << " ampFit done. Took " << (time(0) - startTime)/60 
       << " min. Returning 0." << endl;
  return 0;
}


int main(){

  return ampFit();

}
//
