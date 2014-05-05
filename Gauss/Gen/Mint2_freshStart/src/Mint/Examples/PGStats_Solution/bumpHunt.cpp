#include "Mint/PdfBase.h"
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/EventList.h"
#include "Mint/Neg2LL.h"
#include "Mint/Minimiser.h"

#include <iostream>
#include <cstdio>
#include <fstream>

#include "TNtupleD.h"
#include "TH1D.h"
#include "TFile.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TF1.h"

using namespace MINT;
using namespace std;

enum EVT_TYPE {B=1, BBAR=-1, NOT_ASSIGNED=0};
ostream& operator<<(ostream& os, EVT_TYPE ty){
  if(B == ty) os << "B";
  else if(BBAR == ty) os << "BBAR";
  else os << "NOT_ASSIGNED";
  return os;
}

class OscillationEvent{
  double _time;
  EVT_TYPE _type;
public:
  OscillationEvent(double ti=-1, EVT_TYPE ty=NOT_ASSIGNED) 
  : _time(ti), _type(ty){}
  OscillationEvent(const OscillationEvent& other)
    : _time(other.time()), _type(other.type()){}
  double time() const{return _time;}
  EVT_TYPE type() const{return _type;}
  void print(ostream& os) const{
    os << type() << "\t" << time();}
};
ostream& operator<<(ostream& os, OscillationEvent& evt){
  evt.print(os);
  return os;
}

double asymmetry(double A, double dm, double t){
  return A*sin(dm*t);
}

class OscillationPdf : public PdfBase<OscillationEvent>{
  FitParameter* _A;
  double _dm;
public:
  OscillationPdf(FitParameter* A_in
		 , double dm_in) 
    : _A(A_in)
    , _dm(dm_in){}

  double getVal(){
    OscillationEvent* evtPtr = getEvent();
    if(! evtPtr){
      cout << "OscillationPdf: Can't get event!!"
           << endl;
      return -9999;
    }
    double t    = evtPtr->time();
    EVT_TYPE type = evtPtr->type();
    return ( 1.0 + type*getAsy(t) ) /2.0;
  }
  double getAsy(double t){
    double Aval = (0 == _A ? 0 : (double) *_A);
    return asymmetry(Aval, _dm, t);
  }
};

class DataMaker{

  double _N, _generatedA, _decayTime, _dm;
  EventList<OscillationEvent> _events;
  double _tmax;

 public:
  DataMaker(double N, double genA, double decTime, double dm)
    : _N(N)
    , _generatedA(genA)
    , _decayTime(decTime)
    , _dm(dm)
    {}
  void makeData(){
    _tmax = -9999;
    _events.clear();
    for(int i = 0; i < _N; i++){
      double t = gRandom->Exp(_decayTime);
      double p_of_B = ( 1.0 + B*asymmetry(_generatedA, _dm, t) ) /2.0;
      EVT_TYPE type = B;
      if(gRandom->Rndm() > p_of_B) type = BBAR;
      _events.push_back(OscillationEvent(t, type));
      if(t > _tmax || 0 == i) _tmax = t;
    }
  }
  void saveData(){
    filebuf fb;
    fb.open("data.txt",ios::out);
    ostream os(&fb);
    TFile* fn = new TFile("data.root", "RECREATE");
    TNtupleD* ntp = new TNtupleD("OscillationData"
				 , "OscillationData"
				 , "type:time");
    TH1D* hist = new TH1D("OscillatonHisto", "OscillationHisto"
			  , 10, 0, _tmax*1.01);
    hist->Sumw2();
    for(unsigned int i = 0; i < _events.size(); i++){
      os << _events[i] << endl;
      ntp->Fill(_events[i].type(), _events[i].time());
      hist->Fill(_events[i].time(), (int) (_events[i].type()));
    }
    ntp->Write();
    hist->Write();
    fb.close();
    fn->Close();
  }
  EventList<OscillationEvent>* events(){return &_events;}
  double getMax()const{return _tmax;}

  TH1D* getHist(){
    TH1D* hist     = new TH1D("OscillationHisto", "OscillationHisto"
			      , 10, 0, _tmax*1.01);
    TH1D* hist_sum = new TH1D("OscillationHisto", "OscillationHisto"
			      , 10, 0, _tmax*1.01);
    hist->SetXTitle("time (ps)");
    hist->SetYTitle("Asymmetry");
    hist->Sumw2();
    //hist_diff->Sumw2();
    hist_sum->Sumw2();
    for(unsigned int i = 0; i < _events.size(); i++){
      hist_sum->Fill(_events[i].time());
      hist->Fill(_events[i].time(), (int) (_events[i].type()));
    }
    hist->Divide(hist_sum);
    return hist;
  }
};

class GarbageMaker{

  double _N, _generatedA, _decayTime, _dm;
  EventList<OscillationEvent> _events;
  double _tmax;

 public:
  GarbageMaker(double N, double genA, double decTime, double dm)
    : _N(N)
    , _generatedA(genA)
    , _decayTime(decTime)
    , _dm(dm)
    {}
  void makeData(){
    _tmax = -9999;
    _events.clear();
    for(int i = 0; i < _N; i++){
      int k = (int) (gRandom->Rndm()*10.0);
      double t = k*(TMath::Pi()/2.0)/_dm;
      EVT_TYPE type = B;
      _events.push_back(OscillationEvent(t, type));
      if(t > _tmax || 0 == i) _tmax = t;
    }
  }
  void saveData(){
    filebuf fb;
    fb.open("garbage.txt",ios::out);
    ostream os(&fb);
    TFile* fn = new TFile("garbage.root", "RECREATE");
    TNtupleD* ntp = new TNtupleD("OscillationData"
				 , "OscillationData"
				 , "type:time");
    TH1D* hist = new TH1D("OscillatonHisto", "OscillationHisto"
			  , 100, 0, _tmax*1.01);
    TH1D* hist_sum = new TH1D("OscillatonHisto", "OscillationHisto"
			  , 100, 0, _tmax*1.01);
    hist->Sumw2();
    for(unsigned int i = 0; i < _events.size(); i++){
      os << _events[i] << endl;
      ntp->Fill(_events[i].type(), _events[i].time());
      hist->Fill(_events[i].time(), (int) (_events[i].type()));
      hist_sum->Fill(_events[i].time());
    }
    hist->Divide(hist_sum);
    ntp->Write();
    hist->Write();
    fb.close();
    fn->Close();
  }
  EventList<OscillationEvent>* events(){return &_events;}
  double getMax()const{
    return _tmax;
  }
  TH1D* getHist(){
    TH1D* hist     = new TH1D("GarbageHisto", "GarbageHisto"
			      , 100, 0, _tmax*1.01);
    TH1D* hist_sum = new TH1D("GarbageHisto", "GarbageHisto"
			      , 100, 0, _tmax*1.01);
    hist->SetXTitle("time (ps)");
    hist->SetYTitle("Asymmetry");
    hist->Sumw2();
    //hist_diff->Sumw2();
    hist_sum->Sumw2();
    for(unsigned int i = 0; i < _events.size(); i++){
      hist_sum->Fill(_events[i].time());
      hist->Fill(_events[i].time(), (int) (_events[i].type()));
    }
    hist->Divide(hist_sum);
    return hist;
  }
};
OscillationPdf* globalOscillationPdf=0;
int globalScale=-1;
Double_t wrapPdf(Double_t *x, Double_t* ){
  return globalScale * globalOscillationPdf->getAsy(x[0]);
}


int main(){
  NamedParameter<int> randomSeed("RandomSeed");
  gRandom->SetSeed((int)randomSeed);
  NamedParameter<int> N("N");
  NamedParameter<int> numExperiments("NumExperiments");
  NamedParameter<double> generatedA("GeneratedA");
  NamedParameter<double> decayTime("DecayTime");
  FitParameter A("A");
  NamedParameter<double> dm("dm");

  DataMaker data(N, generatedA, decayTime, dm);
  data.makeData();
  data.saveData();
  TH1D* fitH = data.getHist();
  TH1D* fitzeroH = data.getHist();

  OscillationPdf myOscilPdf(&A, dm);
  Neg2LL<OscillationEvent> fcn(&myOscilPdf, data.events());

  Minimiser mini(&fcn);
  mini.doFit();
  double fitResult = A;
  double fitError = A.err();
  double fitResultsig = fitResult/fitError;
  double fcnValue = fcn.getVal();

  globalOscillationPdf = &myOscilPdf;
  double binwidth = fitH->GetBinWidth(1);
  globalScale = 1;//N * binwidth;
  TF1* func = new TF1("oscilPdf", wrapPdf, 0, data.getMax());
  fitH->GetListOfFunctions()->Add(func);
  TCanvas* can = new TCanvas;
  fitH->SetMinimum(-2);
  fitH->Draw();
  can->Print("fitH.pdf");

  // Goodness of Fit testing:
  TH1D* h = data.getHist();
  double chi2=0;
  int nbins=0;
  for(int i=1; i < h->GetNbinsX(); i++){
    double f = asymmetry(fitResult, dm, h->GetBinCenter(i));
    double n = h->GetBinContent(i);
    double sigma = h->GetBinError(i);
    if(0.0 == sigma) continue;
    double df = ((f-n)/sigma);
    chi2 += df*df;
    nbins++;
  }
  int ndof = nbins - 1;
  cout << "chi2 / ndof = " << chi2 << " / " << ndof 
       << " = " << chi2/ndof << endl;
  double chi2Prob = TMath::Prob(chi2, ndof);
  cout << "chi2 prob: " << chi2Prob << endl;
  
  // garbage test
  cout << "++++++++++ Garbage test +++++++++++" << endl;
  GarbageMaker garb(N, generatedA, decayTime, dm);
  garb.makeData();
  garb.saveData();
  TH1D* garbfitH = garb.getHist();
  TH1D* garbfitzeroH = garb.getHist();

  Neg2LL<OscillationEvent> gfcn(&myOscilPdf, garb.events());

  Minimiser gmini(&gfcn);
  gmini.doFit();

  double garbfitResult = A;
  double garbfitError = A.err();
  double garbfitResultsig = garbfitResult/garbfitError;
  double garbfcnValue = gfcn.getVal();

  // Goodness of Fit testing:
  TH1D* garbh = garb.getHist();
  double garbchi2=0;
  int gnbins=0;
  for(int i=1; i < garbh->GetNbinsX(); i++){
    double f = asymmetry(garbfitResult, dm, garbh->GetBinCenter(i));
    double n = garbh->GetBinContent(i);
    double sigma = garbh->GetBinError(i);
    if(0.0 == sigma) continue;
    double df = ((f-n)/sigma);
    garbchi2 += df*df;
    gnbins++;
  }
  int gndof = gnbins - 1;
  cout << "garbchi2 / ndof = " << garbchi2 << " / " << gndof 
       << " = " << garbchi2/gndof << endl;
  double garbchi2Prob = TMath::Prob(garbchi2, gndof);
  cout << "garbchi2 prob: " << garbchi2Prob << endl;

  cout << "+++++++++ done Garbage test +++++++ " << endl;


  globalOscillationPdf = &myOscilPdf;
  binwidth = garbfitH->GetBinWidth(1);
  globalScale = 1;//N * binwidth;
  
  TF1* garbfunc = new TF1("oscilPdf", wrapPdf, 0, garb.getMax());
  garbfitH->GetListOfFunctions()->Add(garbfunc);
  TCanvas* can2 = new TCanvas;
  garbfitH->SetMinimum(-garbfitResult*1.1);
  garbfitH->Draw();
  can2->Print("garbfitH.pdf");

  cout << "Compare FCN: Fit: " << fcnValue << " garbage: " << garbfcnValue
       << endl;

  // now let's do hypothesis testing...
  cout << "relative to null: " << fitResultsig << " effect" << endl;
  cout << "should be p = " << 1.0 -TMath::Freq(fitResultsig) << endl;

  OscillationPdf myNullPdf(0, dm);
  Neg2LL<OscillationEvent> fcnNull(&myNullPdf, data.events());
  
  double logOfLLratio = (fcnValue - fcnNull.getVal());
  cout << "log of LL ratio: " << logOfLLratio << endl;

  cout << "LL ratio probability (Gauss)  " 
       << 2*(1.0 -TMath::Freq(sqrt(fabs(logOfLLratio)))) << endl;
  cout << "LL ratio probability (Chi2)  " 
       << TMath::Prob(-logOfLLratio, 1) << endl;
  
  
  nbins=0;
  double null_chi2=0;
  for(int i=1; i < h->GetNbinsX(); i++){
    double f = 0;
    double n = h->GetBinContent(i);
    double sigma = h->GetBinError(i);
    if(0.0 == sigma) continue;
    double df = ((f-n)/sigma);
    null_chi2 += df*df;
    nbins++;
  }
  ndof = nbins - 1;
  cout << "null - hypothesis chi2 / ndof = " << null_chi2 << " / " << ndof 
       << " = " << null_chi2/ndof << endl;
  double null_chi2Prob = TMath::Prob(null_chi2, ndof);
  cout << "null chi2 prob: " << null_chi2Prob << endl;


  DataMaker newData(N, 0, decayTime, dm);
  int ll_ratio_above=0;
  int bigA=0;
  int bigAsig=0;
  TH1D ll_ratioH("ll_ratioH", "ll_ratioH"
		    , 400, 0, 10);
  ll_ratioH.SetXTitle("liklihood ratio");
  binwidth = ll_ratioH.GetBinWidth(1);
  char ytitle[100];
  sprintf(ytitle, "Number of experiments per %f", binwidth);
  ll_ratioH.SetYTitle(ytitle);

  TH1D null_Avalues("null_Avalues", "null_Avalues"
		    , 400, generatedA - 10*A.err(), generatedA + 10*A.err());
  null_Avalues.SetXTitle("Fit results for S");
  binwidth = null_Avalues.GetBinWidth(1);
  sprintf(ytitle, "Number of experiments per %f", binwidth);
  null_Avalues.SetYTitle(ytitle);

  TH1D null_Asigvalues("null_Asigvalues", "null_Asigvalues"
		    , 400, -10, 10);
  null_Asigvalues.SetXTitle("Results for S/sigma");
  binwidth = null_Asigvalues.GetBinWidth(1);
  sprintf(ytitle, "Number of experiments per %f", binwidth);
  null_Asigvalues.SetYTitle(ytitle);

  for(int i = 1; i < numExperiments; i++){
    A.resetToInit();
    newData.makeData();
    Neg2LL<OscillationEvent> newfcn(&myOscilPdf, newData.events());
    Minimiser toymini(&newfcn);
    toymini.setPrintLevel(0);
    toymini.doFit();
    
    OscillationPdf myNullPdf(0, dm);
    Neg2LL<OscillationEvent> fcnNull(&myNullPdf, newData.events());
    
    double newLogOfLLratio = (newfcn.getVal() - fcnNull.getVal());
    cout << "new log of LL ratio: " << newLogOfLLratio << endl;
    
    ll_ratioH.Fill(-newLogOfLLratio);
    if(newLogOfLLratio < logOfLLratio) ll_ratio_above++;

    null_Avalues.Fill(A);
    if(fabs(A) > fabs(fitResult)) bigA++;

    double Asig = A / A.err();
    null_Asigvalues.Fill(Asig);
    if(fabs(Asig) > fabs(fitResultsig)) bigAsig++;
  }
  double ll_ratio_pvalue = ((double) ll_ratio_above)/((double) numExperiments);
  double ll_ratio_p_error = 
    sqrt(numExperiments*ll_ratio_pvalue*(1.0-ll_ratio_pvalue))/numExperiments;
  cout << "ll_ratio p value = " 
       << ll_ratio_pvalue << " +/- " << ll_ratio_p_error << endl;

  double A_pvalue = ((double) bigA)/((double) numExperiments);
  double A_p_error = 
    sqrt(numExperiments*A_pvalue*(1.0-A_pvalue))/numExperiments;
  cout << "A p value = " << A_pvalue << " +/- " << A_p_error << endl;

  double Asig_pvalue = ((double) bigAsig)/((double) numExperiments);
  double Asig_p_error = 
    sqrt(numExperiments*Asig_pvalue*(1.0-Asig_pvalue))/numExperiments;
  cout << "Asig p value = " << Asig_pvalue << " +/- " << Asig_p_error << endl;

  TFile* f = new TFile("qualityH.root", "RECREATE");
  ll_ratioH.Write();
  null_Avalues.Write();
  null_Asigvalues.Write();
  f->Close();

  TCanvas* tc2 = new TCanvas("p-values");
  ll_ratioH.Draw();
  tc2->Print("LL_ratios.pdf");

  null_Avalues.Draw();
  tc2->Print("Amp_values.pdf");

  null_Asigvalues.Draw();
  tc2->Print("Amp_significances.pdf");


  cout << "====================" << endl;
  cout << "Result summary" << endl;
  cout << " input value: " << generatedA << endl;
  cout << " fit result: " << fitResult << " +/- " <<  fitError << endl;
  cout << "chi2 *fit* probability: " << chi2Prob << endl;
  cout << " ------------------------ " << endl;
  cout << "chi2 null probability: " << null_chi2Prob << endl;
  cout << "deviation of A probability " 
       << 2*(1.0 - TMath::Freq(fitResult/fitError)) << endl;
  cout << "log(LL ratio) " <<  logOfLLratio << endl;
  cout << "LL ratio probability (Gauss)" 
       << 2*(1.0 -TMath::Freq(sqrt(fabs(logOfLLratio)))) << endl;
  cout << "LL ratio probability (Chi2)" 
       << TMath::Prob(-logOfLLratio, 1) << endl;
  cout << " ------------------------- " << endl;
  cout << " magnitude of A p value: " 
       << A_pvalue  <<  " +/- " << A_p_error << endl;
  cout << "Asig p value = " 
       << Asig_pvalue << " +/- " << Asig_p_error << endl;
  cout << " ll-ratio p value: " 
       << ll_ratio_pvalue  << " +/- " << ll_ratio_p_error  << endl;
  cout << "=====================" << endl;
  cout << "done." << endl;

  // now do ll prob

  // NULL-hypothesis testing


  /*

  OscillationPdf myNullPdf(0, dm);
  Neg2LL<OscillationEvent> fcnNull(&myNullPdf, data.events());

  double logOfLLratio = -0.5* (fcn.getVal() - fcnNull.getVal());
  cout << "log of LL ratio: " << logOfLLratio << endl;

  */

  return 0;
}
