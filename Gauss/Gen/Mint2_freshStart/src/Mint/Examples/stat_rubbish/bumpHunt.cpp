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
    double Aval = (0 == _A ? 0 : (double) *_A);
    return ( 1.0 + type*asymmetry(Aval, _dm, t) ) /2.0;
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
			  , 100, 0, _tmax*1.01);
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

  TH1D* getHist(){
    TH1D* hist = new TH1D("OscillatonHisto", "OscillationHisto"
			  , 100, 0, _tmax*1.01);
    hist->Sumw2();
    for(unsigned int i = 0; i < _events.size(); i++){
      hist->Fill(_events[i].time(), (int) (_events[i].type()));
    }
    return hist;
  }
};

int main(){
  NamedParameter<int> N("N");
  NamedParameter<int> numExperiments("NumExperiments");
  NamedParameter<double> generatedA("GeneratedA");
  NamedParameter<double> decayTime("DecayTime");
  FitParameter A("A");
  NamedParameter<double> dm("dm");

  DataMaker data(N, generatedA, decayTime, dm);
  data.makeData();
  data.saveData();
  
  OscillationPdf myOscilPdf(&A, dm);
  Neg2LL<OscillationEvent> fcn(&myOscilPdf, data.events());

  Minimiser mini(&fcn);
  mini.doFit();
  double fitResult = A;
  double fitFCN = fcn.getVal();

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
  
  DataMaker newData(N, fitResult, decayTime, dm);
  int est_above=0;
  TH1D est_qualityH("est_qualityH", "est_qualityH"
		    , 400, fitFCN - 60, fitFCN + 60);
  for(int i = 1; i < numExperiments; i++){
    A.resetToInit();
    newData.makeData();
    Neg2LL<OscillationEvent> newfcn(&myOscilPdf, newData.events());
    Minimiser mini(&newfcn);
    mini.setPrintLevel(0);
    mini.doFit();
    double newFCN = newfcn.getVal();
    est_qualityH.Fill(newFCN);
    if(newFCN > fitFCN) est_above++;
  }
  double est_pvalue = ((double) est_above)/((double) numExperiments);
  cout << "est p value = " << est_pvalue << endl;

  int true_above = 0;
  TH1D true_qualityH("true_qualityH", "true_qualityH"
		    , 400, fitFCN - 60, fitFCN + 60);
  for(int i = 1; i < numExperiments; i++){
    A.resetToInit();
    data.makeData();
    Neg2LL<OscillationEvent> newfcn(&myOscilPdf, data.events());
    Minimiser mini(&newfcn);
    mini.setPrintLevel(0);
    mini.doFit();
    double newFCN = newfcn.getVal();
    true_qualityH.Fill(newFCN);
    if(newFCN > fitFCN) true_above++;
  }
  double true_pvalue = ((double) true_above)/((double) numExperiments);
  cout << "true p value = " << true_pvalue << endl;


  TFile* f = new TFile("qualityH.root", "RECREATE");
  est_qualityH.Write();
  true_qualityH.Write();
  f->Close();
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
