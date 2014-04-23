#include "Mint/PdfBase.h"
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/EventList.h"
#include "Mint/Neg2LL.h"
#include "Mint/Minimiser.h"

#include <iostream>
#include <fstream>

#include "TNtupleD.h"
#include "TH1D.h"
#include "TFile.h"
#include "TMath.h"
#include "TF1.h"
#include "TCanvas.h"

#include <algorithm>

using namespace MINT;
using namespace std;

enum EVT_TYPE {B=1, BBAR=-1, NOT_ASSIGNED=0};
ostream& operator<<(ostream& os, EVT_TYPE ty){
  if(B == ty) os << "B";
  else if(BBAR == ty) os << "BBAR";
  else os << "NOT_ASSIGNED";
  return os;
}

class MassEvent{
  double _mass;
public:
  MassEvent(double ma=-1)
    : _mass(ma) {}
  MassEvent(const MassEvent& other)
    : _mass(other.mass()) {}

  double mass() const{return _mass;}
  void print(ostream& os) const{
    os << mass();
  }
  bool operator<(const MassEvent& other) const{
    return mass() < other.mass();
  }
  bool operator>(const MassEvent& other) const{
    return mass() > other.mass();
  }
  bool operator==(const MassEvent& other) const{
    return mass() == other.mass();
  }
};
ostream& operator<<(ostream& os, const MassEvent& evt){
  evt.print(os);
  return os;
}

class MassPdf : public PdfBase<MassEvent>{
  FitParameter* _signalFrac;
  FitParameter* _mean;
  FitParameter* _sigma;
  double _min, _max;
  double _sqrt2pi;

  double signalPdf(double m){
    double d = (m - (*_mean))/(*_sigma);
    return 1./(_sqrt2pi * (*_sigma)) * exp(-0.5*d*d);
  }
  double backgPdf(double m){
    if(m > _max || m < _min) return 0;
    return 1.0/(_max - _min);
  }
  double comulSignal(double m){
    double d = (m - (*_mean))/(*_sigma);
    return TMath::Freq(d);
  }
  double comulBackg(double m){
    if(m < _min) return 0;
    if(m > _max) return 1;
    return (m - _min)/(_max - _min);
  }
public:
  MassPdf(FitParameter* sf
	  , FitParameter* mean, FitParameter* sigma
	  , double mi, double ma) 
    : _signalFrac(sf)
    , _mean(mean), _sigma(sigma)
    , _min(mi), _max(ma){
    _sqrt2pi = sqrt(2.0*TMath::Pi());
  }

  double getVal(){
    double m = getEvent()->mass();
    return getVal(m);
  }
  double getVal(double m){
    double sf = (0 == _signalFrac ? 0 : ((double) *_signalFrac));
    return sf * signalPdf(m) + (1.0 - sf)*backgPdf(m);
  }
  double comul(double m){
    double sf = (0 == _signalFrac ? 0 : ((double) *_signalFrac));
    return sf * comulSignal(m) + (1.0 - sf)*comulBackg(m);
  }
};

class DataMaker{
  
  double _N, _sf, _mean, _sigma, _min, _max;
  EventList<MassEvent> _events;
public:
  DataMaker(double N, double sf, double mean, double sigma
	    , double mi, double ma
	    ) 
    : _N(N), _sf(sf), _mean(mean), _sigma(sigma), _min(mi), _max(ma)
    {}
  void makeData(){
    _events.clear();
    for(int i = 0; i < _N; i++){
      double m;
      if(gRandom->Rndm() < _sf){
	m = gRandom->Gaus(_mean, _sigma);
      }else{
	m = gRandom->Rndm()*(_max - _min) + _min;
      }
      _events.push_back(MassEvent(m));
    }
  }
  void saveData(){
    filebuf fb;
    fb.open("mass_data.txt",ios::out);
    ostream os(&fb);
    TFile* fn = new TFile("mass_data.root", "RECREATE");
    TH1D* hist = new TH1D("MassHisto", "MassHisto"
			  , 100, _min, _max);
    hist->Sumw2();
    for(unsigned int i = 0; i < _events.size(); i++){
      os << _events[i] << endl;
      hist->Fill(_events[i].mass());
    }
    hist->Write();
    fb.close();
    fn->Close();
  }
  EventList<MassEvent>* events(){return &_events;}

  TH1D* getHist(){
    TH1D* hist = new TH1D("MassHisto", "MassHisto"
			  , 40, _min, _max);
    hist->Sumw2();
    for(unsigned int i = 0; i < _events.size(); i++){
      hist->Fill(_events[i].mass());
    }
    return hist;
  }
};

MassPdf* globalMassPdf=0;
int globalScale=-1;
Double_t wrapPdf(Double_t *x, Double_t* ){
  return globalScale * globalMassPdf->getVal(x[0]);
}

int main(){
  NamedParameter<int> randomSeed("RandomSeed");
  gRandom->SetSeed((int)randomSeed);
  NamedParameter<int> N("N");
  NamedParameter<int> numExperiments("NumExperiments");
  NamedParameter<double> generated_SF("Generated_SF");
  FitParameter mean("Mean"), sigma("Sigma");
  NamedParameter<double> min("Min"), max("Max");
  FitParameter SF("SF");

  DataMaker data(N, generated_SF, (double) mean, (double) sigma
		 , (double) min, (double) max);
  data.makeData();
  data.saveData();
  TH1D* fitH = data.getHist();
  TH1D* fitzeroH = data.getHist();

  MassPdf myMassPdf(&SF, &mean, &sigma
		    , (double) min, (double) max);
  Neg2LL<MassEvent> fcn(&myMassPdf, data.events());

  Minimiser mini(&fcn);
  mini.doFit();
  double fitResult = SF;
  double fitError = SF.err();
  double fitResultsig = fitResult/fitError;
  double fcnValue = fcn.getVal();

  globalMassPdf = &myMassPdf;
  double binwidth = fitH->GetBinWidth(1);
  globalScale = N * binwidth;
  TF1* func = new TF1("massPdf", wrapPdf, (double) min, (double) max);
  fitH->GetListOfFunctions()->Add(func);
  TCanvas* can = new TCanvas;
  fitH->Draw();
  can->Print("fitH.pdf");
  
  // now let's do hypothesis testing...
  cout << "relative to null: " << fitResultsig << " effect" << endl;
  cout << "should be p = " << 1.0 -TMath::Freq(fitResultsig) << endl;

  MassPdf myNullPdf(0, &mean, &sigma
		    , (double) min, (double) max);
  globalMassPdf = &myNullPdf;
  globalScale = N * binwidth;
  TF1* func2 = new TF1("nullPdf", wrapPdf, (double) min, (double) max);
  fitzeroH->GetListOfFunctions()->Add(func2);
  fitzeroH->Draw();
  can->Print("fitzeroH.pdf");

  double chi2=0;
  double zero_chi2=0;
  int nbins=0;
  for(int i=1; i < fitzeroH->GetNbinsX(); i++){
    double m = fitzeroH->GetBinCenter(i);
    double dm = fitzeroH->GetBinWidth(i);
    double f = N * myMassPdf.getVal(m) * dm;
    double zero_f = N * myNullPdf.getVal(m) * dm;
    double n = fitzeroH->GetBinContent(i);
    double sigma = fitzeroH->GetBinError(i);
    if(0.0 == sigma) continue;
    double df = ((f-n)/sigma);
    chi2 += df*df;
    double zero_df = ((zero_f-n)/sigma);
    zero_chi2 += zero_df*zero_df;
    nbins++;
  }
  int ndof = nbins - 1;
  cout << "fit chi2 / ndof = " << chi2 << " / " << ndof 
       << " = " << chi2/ndof << endl;
  double chi2Prob = TMath::Prob(chi2, ndof);
  cout << "fit chi2 prob: " << chi2Prob << endl;

  cout << "zero_chi2 / ndof = " << zero_chi2 << " / " << ndof 
       << " = " << zero_chi2/ndof << endl;
  double zero_chi2Prob = TMath::Prob(zero_chi2, ndof);
  cout << "zero_chi2 prob: " << zero_chi2Prob << endl;

  double zero_maxD = -999;
  double maxD = -999;
  EventList<MassEvent> sortedList(*data.events());
  sort(sortedList.begin(), sortedList.end());
  double num = sortedList.size();
  for(unsigned int i=0; i < sortedList.size(); i++){
    double m = sortedList[i].mass();
    double F = myMassPdf.comul(m);
    double zeroF = myNullPdf.comul(m);
    double cd = ((double) i)/num;
    double D = fabs(F-cd);
    double zeroD = fabs(zeroF-cd);
    if(D > maxD) maxD=D;
    if(zeroD > zero_maxD) zero_maxD=D;
  }
  cout << "Kolmogorov D = " << maxD << endl;
  double KolmoZ = sqrt(num)*maxD;
  cout << "Kolmogorov sqrt(N)*D "  << KolmoZ << endl;
  cout << "Kolmogorov Prob " << TMath::KolmogorovProb(KolmoZ) << endl;

  cout << "zero Kolmogorov D = " << zero_maxD << endl;
  double zero_KolmoZ = sqrt(num)*zero_maxD;
  cout << "zero Kolmogorov sqrt(N)*D "  << zero_KolmoZ << endl;
  cout << "zero Kolmogorov Prob " << TMath::KolmogorovProb(zero_KolmoZ) << endl;

  Neg2LL<MassEvent> fcnNull(&myNullPdf, data.events());
  
  double logOfLLratio = (fcnValue - fcnNull.getVal());
  cout << "log of LL ratio: " << logOfLLratio << endl;

  cout << "LL ratio probability (Gauss)  " 
       << 2*(1.0 -TMath::Freq(sqrt(fabs(logOfLLratio)))) << endl;
  cout << "LL ratio probability (Chi2)  " 
       << TMath::Prob(-logOfLLratio, 1) << endl;
  
  

  DataMaker newData(N, 0, mean, sigma, min, max);
  int ll_ratio_above=0;
  int bigA=0;
  int bigAsig=0;
  TH1D ll_ratioH("ll_ratioH", "ll_ratioH"
		    , 100, 0, 10);
  TH1D null_Avalues("null_Avalues", "null_Avalues"
		    , 400, generated_SF - 10*SF.err(), generated_SF + 10*SF.err());
  TH1D null_Asigvalues("null_Asigvalues", "null_Asigvalues"
		    , 400, -10, 10);
  for(int i = 1; i < numExperiments; i++){
    SF.resetToInit();
    newData.makeData();
    Neg2LL<MassEvent> newfcn(&myMassPdf, newData.events());
    Minimiser toymini(&newfcn);
    toymini.setPrintLevel(0);
    toymini.doFit();
    
    MassPdf myNullPdf(0, &mean, &sigma
		      , (double) min, (double) max);
    Neg2LL<MassEvent> fcnNull(&myNullPdf, newData.events());
    
    double newLogOfLLratio = (newfcn.getVal() - fcnNull.getVal());
    cout << "new log of LL ratio: " << newLogOfLLratio << endl;
    
    ll_ratioH.Fill(-newLogOfLLratio);
    if(newLogOfLLratio < logOfLLratio) ll_ratio_above++;

    null_Avalues.Fill(SF);
    if(SF > fitResult) bigA++;

    double Asig = SF / SF.err();
    null_Asigvalues.Fill(Asig);
    if(Asig > fitResultsig) bigAsig++;
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
  ll_ratioH.Draw();
  can->Print("ll_ratio.pdf");

  null_Avalues.Write();
  null_Avalues.Draw();
  can->Print("null_Avalues.pdf");

  fitH->Write();
  f->Close();

  cout << "====================" << endl;
  cout << "Result summary" << endl;
  cout << " input value: " << generated_SF << endl;
  cout << " fit result: " << fitResult << " +/- " <<  fitError << endl;
  cout << " fit quality: " << endl;
  cout << "fit chi2 / ndof = " << chi2 << " / " << ndof 
       << " = " << chi2/ndof << endl;
  cout << "fit chi2 prob: " << chi2Prob << endl;
  cout << "Kolmogorov D = " << maxD << endl;
  cout << "Kolmogorov sqrt(N)*D "  << KolmoZ << endl;
  cout << "Kolmogorov Prob " << TMath::KolmogorovProb(KolmoZ) << endl;
  cout << " ------------------------ " << endl;
  cout <<  "zero_chi2 / ndof = " << zero_chi2 << " / " << ndof 
       << " = " << zero_chi2/ndof << endl;
  cout << "zero_chi2 prob: " << zero_chi2Prob << endl;
  cout << " - " << endl;
  cout << "zero Kolmogorov D = " << zero_maxD << endl;
  cout << "zero Kolmogorov sqrt(N)*D "  << zero_KolmoZ << endl;
  cout << "zero Kolmogorov Prob " << TMath::KolmogorovProb(zero_KolmoZ) << endl;
  cout << " - " << endl;
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

  MassPdf myNullPdf(0, dm);
  Neg2LL<MassEvent> fcnNull(&myNullPdf, data.events());

  double logOfLLratio = -0.5* (fcn.getVal() - fcnNull.getVal());
  cout << "log of LL ratio: " << logOfLLratio << endl;

  */

  return 0;
}
