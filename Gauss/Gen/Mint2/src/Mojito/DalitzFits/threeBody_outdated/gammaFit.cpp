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
#include "Mint/DalitzPdfBaseFastInteg.h"
#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpSum.h"

#include "Mint/DalitzHistoSet.h"
#include "Mint/DalitzEvent.h"


#include "Mint/cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"

#include <iostream>

using namespace std;

class Bplus
  : public DalitzPdfBaseFastInteg
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
	, IDalitzEventList* events=0) 
    : DalitzPdfBaseFastInteg(events)
    , _gamma(gamma), _rB(rB), _delta(delta)
    , _amps(this), _CPamps(this)
  {
    cout << " that's gamma " << _gamma << endl;
    cout << " that's rb    " << _rB << endl;
    cout << " that's delta " << _delta << endl;
    _CPamps.CPConjugateSameFitParameters();
  }
  DalitzBWBoxSet makeBWBoxes(const DalitzEventPattern& pdg){
    return _amps.makeBWBoxes(pdg);
  }
};

class Bminus : public Bplus{
public:
  double un_normalised(){
    //    bool dbthis=false;
    complex<double> i(0,1);
    double deg = 180.0/pi;

    complex<double> cval = _amps.getVal() 
      + ((double)_rB) * cexp(i*(_gamma - _delta)/deg) * _CPamps.getVal();

    return cval.real()*cval.real() + cval.imag()*cval.imag();

  }

  
  Bminus(FitParameter& gamma, FitParameter& rB, FitParameter& delta
	, IDalitzEventList* events=0)
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
  
  FitParameter gamma("gamma");
  FitParameter rB("rB");
  FitParameter delta("delta");

  Bplus  Bp(gamma, rB, delta, &BpList);
  Bminus Bm(gamma, rB, delta, &BmList);

  DalitzBWBoxSet pBox(Bp.makeBWBoxes(pdg));
  DalitzBWBoxSet mBox(Bm.makeBWBoxes(pdg));
  
  time_t t0 = time(0);

  for(int i=0; i< Nevents; i++){
    int printEvery  = 1000;
    if(i < 1000) printEvery=100;
    bool printout = ( i%printEvery == 0 || i < 100);
    if(printout){
      cout << " made Bp event number " << i << endl;
      cout << "   current list size " << BpList.size() << endl;
      double dt = difftime(time(0), t0);
      cout << "     this took " << dt << "s" << endl;
      if(dt > 0) cout << "     or " << BpList.size()/dt << "evt/s" << endl;
    }
    counted_ptr<DalitzEvent> evt(pBox.makeEventForOwner());
    if(printout) cout << "tried one event " << evt << endl;
    if(evt){
      BpList.Add(*evt);
    }
  }
  for(int i=0; i< Nevents; i++){
    int printEvery  = 1000;
    if(i < 1000) printEvery=100;
    bool printout = ( i%printEvery == 0 || i < 100);
    if(printout){
      cout << " made Bm event number " << i << endl;
      cout << "   current list size " << BmList.size() << endl;
      double dt = difftime(time(0), t0);
      cout << "     this took " << dt << "s" << endl;
      if(dt > 0) cout << "     or " << BmList.size()/dt << "evt/s" << endl;
    }
    counted_ptr<DalitzEvent> evt(mBox.makeEventForOwner());
    if(printout) cout << "tried one event " << evt << endl;
    if(evt){
      BmList.Add(*evt);
    }
  }
  
  BpList.Start();
  BmList.Start();

  DalitzHistoSet psp= BpList.histoSet();
  psp.save("BpPlots.root");
  DalitzHistoSet psm= BmList.histoSet();
  psm.save("BmPlots.root");

  Neg2LL<IDalitzEvent> logBp(&Bp, &BpList);
  Neg2LL<IDalitzEvent> logBm(&Bm, &BmList);

  Neg2LLSum fcn(&logBp, &logBm);

  Minimiser mini(&fcn);
  mini.doFit();

  DalitzHistoSet fit_p = Bp.histoSet();
  DalitzHistoSet fit_m = Bm.histoSet();
  
  psp.drawWithFit(fit_p, "plusWithFit_");
  psm.drawWithFit(fit_m, "minusWithFit_");

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
