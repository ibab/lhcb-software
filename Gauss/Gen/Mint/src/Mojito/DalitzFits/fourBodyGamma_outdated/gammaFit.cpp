// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
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

class Bplus
  : public DalitzPdfBase
{
protected:
  FitParameter& _gamma;
  FitParameter& _rB;
  FitParameter& _delta;
  FitAmpSum _amps;
  FitAmpSum _CPamps;

public:

  double un_normalised_noPs(){
    //    bool dbthis=false;
    complex<double> i(0,1);
    double deg = 180.0/pi;

    complex<double> cval = _amps.getVal() 
      + _rB * cexp(i*(_gamma + _delta)/deg) * _CPamps.getVal();

    return cval.real()*cval.real() + cval.imag()*cval.imag();

  }

  Bplus(FitParameter& gamma, FitParameter& rB, FitParameter& delta
	, IDalitzEventAccess* events=0) 
    : DalitzPdfBase(events)
    , _gamma(gamma), _rB(rB), _delta(delta)
    , _amps(this), _CPamps(this)
  {
    cout << " that's gamma " << _gamma << endl;
    cout << " that's rb    " << _rB << endl;
    cout << " that's delta " << _delta << endl;
    _CPamps.CPConjugateSameFitParameters();
  }
  
};

class Bminus : public Bplus{
public:
  double un_normalised_noPs(){
    //    bool dbthis=false;
    complex<double> i(0,1);
    double deg = 180.0/pi;

    complex<double> cval = _amps.getVal() 
      + ((double)_rB) * cexp(i*(_gamma - _delta)/deg) * _CPamps.getVal();

    return cval.real()*cval.real() + cval.imag()*cval.imag();

  }

  
  Bminus(FitParameter& gamma, FitParameter& rB, FitParameter& delta
	, IDalitzEventAccess* events=0)
    : Bplus(gamma, rB, delta, events){}

};

#include "TRandom2.h"

int gammaFit(){

  TRandom2 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed(0);
  gRandom = &ranLux;

  FitAmplitude::AutogenerateFitFile();

  NamedParameter<int> Nevents("Nevents", 10000);
  NamedParameter<int> NeventsFinal("NeventsFinal",  1300);
  NamedParameter<int> doScan("doScan", 0);
   
  NamedParameter<int> EventPattern("Event Pattern", 421, 310, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  cout << " got event pattern: " << pdg << endl;
  DalitzEventList BpList;
  DalitzEventList BmList;
  cout << "now about ot generate " << Nevents << " events" << endl;
  BpList.generatePhaseSpaceEvents(Nevents, pdg, &ranLux);
  BmList.generatePhaseSpaceEvents(Nevents, pdg, &ranLux);
  BpList.Start();
  BmList.Start();

  FitParameter gamma("gamma");
  FitParameter rB("rB");
  FitParameter delta("delta");

  Bplus  Bp(gamma, rB, delta, &BpList);
  Bminus Bm(gamma, rB, delta, &BmList);

  BpList.throwAwayData(&Bp, NeventsFinal);
  BmList.throwAwayData(&Bm, NeventsFinal);

  PlotSet psp= BpList.makeAllPlots("BpPlots");
  psp.save("BpPlots.root");
  PlotSet psm= BmList.makeAllPlots("BmPlots");
  psm.save("BmPlots.root");

  Neg2LL<IDalitzEvent> logBp(&Bp, &BpList);
  Neg2LL<IDalitzEvent> logBm(&Bm, &BmList);

  Neg2LLSum fcn(&logBp, &logBm);

  Minimiser mini(&fcn);
  mini.doFit();
  

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

  return 0;
}


int main(){

  return gammaFit();

}
//
