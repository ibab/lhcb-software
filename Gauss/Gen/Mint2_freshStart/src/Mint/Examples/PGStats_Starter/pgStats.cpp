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
  // this constructor generates data:
  DataMaker(double N, double genA, double decTime, double dm)
    : _N(N)
    , _generatedA(genA)
    , _decayTime(decTime)
    , _dm(dm)
    {
      makeData();
    }

  // this one reads them:
  DataMaker(char* filename){
    readData(filename);
  }

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
   void readData(char* fname){
    filebuf fb;
    fb.open(fname,ios::in);
    istream is(&fb);
    std::string BorBBar;
    double t;
    while(is >> BorBBar >> t){
      EVT_TYPE type = (BorBBar == "B"? B : BBAR);
      _events.push_back(OscillationEvent(t, type));
    }
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
  NamedParameter<int> randomSeed("RandomSeed");
  gRandom->SetSeed((int)randomSeed);
  NamedParameter<int> N("N");
  NamedParameter<int> numExperiments("NumExperiments");
  NamedParameter<double> generatedA("GeneratedA");
  NamedParameter<double> decayTime("DecayTime");
  FitParameter A("A");
  NamedParameter<double> dm("dm");

  DataMaker data("data.txt");
  OscillationPdf myOscilPdf(&A, dm);
  Neg2LL<OscillationEvent> fcn(&myOscilPdf, data.events());

  Minimiser mini(&fcn);
  mini.doFit();
  double fitResult = A;
  double fitError = A.err();
  double fitResultsig = fitResult/fitError;
  double fcnValue = fcn.getVal(); // = -2log(likelihood)

  cout << "================" << endl;
  cout << "Result summary" << endl;
  cout << " fit result: " << fitResult << " +/- " <<  fitError << endl;

  /*
  cout << "chi2 *fit* probability: " << chi2Prob << endl;
  cout << " ------------------------ " << endl;
  cout << "chi2 null probability: " << null_chi2Prob << endl;
  cout << "deviation of A probability " 
       << 2*(1.0 - TMath::Freq(fitResult/fitError)) << endl;
  cout << "LL ratio probability (Chi2-prob, 1 extra parameter)" 
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
  */


  return 0;
}
