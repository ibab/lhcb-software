// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Neg2LL.h"
#include "Mint/Neg2LLSum.h"
#include "Mint/DalitzEventList.h"

#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/CLHEPSystemOfUnits.h"


#include "Mint/PdfBase.h"
#include "Mint/DalitzPdfBase.h"
#include "Mint/DalitzPdfBaseFastInteg.h"
#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpSum.h"

#include "Mint/DalitzEvent.h"

#include "Mint/AmpRatios.h"

#include "Mint/IEventGenerator.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/DalitzBoxSet.h"

#include "Mint/SignalGenerator.h"
#include "Mint/FromFileGenerator.h"

#include "Mint/cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"

#include "TRandom2.h"
#include "TRandom3.h"
#include <ctime>

#include <iostream>

#include "Mint/DalitzPdfNormChecker.h"

using namespace std;
using namespace MINT;


TH1F* get_h(TFile* f, const std::string& hname){
  TH1F* h=0;
  if(0 == f) return 0;
  cout << " got file ptr: " << f << endl;
  if(0 != f){
    h = (TH1F*) f->Get(hname.c_str());
    if(0 != h){
      cout << "got efficiency histogram " 
	   << hname << "." << endl;
    }else{
      cout << "ERROR didn't get efficiency histogram " 
	   << hname << "!!!" << endl;
    }
  }
  return h;
}


class mumsMomentum : virtual public IReturnReal{
  TH1F* _h;
public:
  mumsMomentum(const string& fname="DmomGeV.root"
	       , const string& hname="Dmom"){
    TFile* f = new TFile(fname.c_str());
    _h = get_h(f, hname.c_str());
  }
  
  virtual double RealVal(){
    return _h->GetRandom()*GeV;
  }
};


class effClass : virtual public IGetDalitzEvent, public DalitzEventAccess{
  TH1F* _hKFast, *_hKSlow, *_hPiFast, *_hPiSlow;
  double _pKFast, _pKSlow, _pPiFast, _pPiSlow;
public:
  effClass(const std::string& fname="Ratios.root"
	   , IDalitzEventAccess* daddy=0) 
  : DalitzEventAccess(daddy)
  , _hKFast(0), _hKSlow(0)
  , _hPiFast(0), _hPiSlow(0)
  , _pKFast(-9999), _pKSlow(-9999)
  , _pPiFast(-9999), _pPiSlow(-9999)
  {
    TFile* f = new TFile(fname.c_str());
    _hKFast   = get_h(f, "fast_kaonR");
    _hKSlow   = get_h(f, "slow_kaonR");
    _hPiFast  = get_h(f, "fast_pionR");
    _hPiSlow  = get_h(f, "slow_pionR");
  }
  void resetP(){
    _pKFast = _pKSlow = _pPiFast = _pPiSlow = -9999;
  }
  bool getP(){
    bool useCMFrame=true;

    if(0 == getEvent()) return false;
    DalitzEventPattern pat = getEvent()->eventPattern();
    counted_ptr<IDalitzEvent> evtPtr(0);
    if(useCMFrame){
      TVector3 p0(0,0,0);
      evtPtr = counted_ptr<IDalitzEvent>(new DalitzEvent(getEvent()));
      evtPtr->setMothers3Momentum(p0);
    }else{
      evtPtr =  counted_ptr<IDalitzEvent>(getEvent());
    }

    resetP();

    double pK[2]={-9999, -9999};
    double pPi[2]={-9999, -9999};
    int iK=0, ip=0;
    for(unsigned int i=1; i < pat.size(); i++){
      if(abs(pat[i]) == 321) pK[iK++]=evtPtr->p(i).Rho()/GeV;
      if(abs(pat[i]) == 211) pPi[ip++]=evtPtr->p(i).Rho()/GeV;
    }
    if(iK != 2 || ip !=2){
      cout << "ERROR could not match all indices. K: " 
	   << iK << ", Pi: " << ip << "." << endl;
      return false;
    }
    if(pK[0] > pK[1]){
      _pKFast = pK[0]; _pKSlow = pK[1];
    }else{
      _pKFast = pK[1]; _pKSlow = pK[0];
    }
    if(pPi[0] > pPi[1]){
      _pPiFast = pPi[0]; _pPiSlow = pPi[1];
    }else{
      _pPiFast = pPi[1]; _pPiSlow = pPi[0];
    }
    return true;
  }

  static double oneEff(TH1F* h, double p){
    bool dbThis=false;
    if(0 == h) return 1;
    if(dbThis)cout << "oneEff for " << h->GetName() << " and " << p << ":\n";
    double bw = (h->GetXaxis()->GetXmax() -  
		 h->GetXaxis()->GetXmin()
		 )/h->GetXaxis()->GetNbins();
    int bin = (int) ((p - h->GetXaxis()->GetXmin())/bw + 0.5);
    double returnVal =  h->GetBinContent(bin);
    if(dbThis)cout << "\t p= " << p << "\t bw = " << bw << " bin = " << bin 
		   << ", returnVal = " << returnVal << endl;
    return returnVal;
  }
  
  double eff(){
    bool dbThis=false;
    double prod=1;
    if(! getP()){
      cout << " couldn't get momenta - returning 0" << endl;
      return 0;
    }
    prod *= oneEff(_hKFast, _pKFast);
    prod *= oneEff(_hKSlow, _pKSlow);
    prod *= oneEff(_hPiFast, _pPiFast);
    prod *= oneEff(_hPiSlow, _pPiSlow);
    if(dbThis) cout << "RealVal(): returning eff = " << prod << endl;
    return prod;
  }  

  double simple(){
    if(0 == getEvent()) return 0;
    return getEvent()->s(1,3) > 0.7*GeV*GeV ? 1.0 : 0.1;
  }

  virtual double RealVal(){
    //    return simple();
    return eff();
  }
};

class AmpsPdf
  : public DalitzPdfBaseFastInteg
{
protected:
  TRandom* _localRnd;
  SignalGenerator* _sgGen;
  FromFileGenerator* _fileGen;
  IEventGenerator<IDalitzEvent>* _chosenGen;
  NamedParameter<std::string> _integratorSource;
  NamedParameter<std::string> _integratorFileName;
  counted_ptr<mumsMomentum> _mp3;
public:
  double un_normalised_noPs(){
    if(0 == getEvent()) return 0;
    double ampSq =  _amps->RealVal();
    return ampSq;// * getEvent()->phaseSpace();
  }

  AmpsPdf( FitAmpSum* amps
	  , IDalitzEventList* events=0
	  , double precision=1.e-4
	  , std::string method="efficient"
	  )
    : DalitzPdfBaseFastInteg(events, 0
			     , amps
			     , precision)
    , _localRnd(0)
    , _sgGen(0)
    , _fileGen(0)
    , _chosenGen(0)
    , _integratorSource("IntegratorSource", (std::string) "new", (char*) 0)
    , _integratorFileName("IntegratorFileName"
			  , (std::string) "SignalIntegrationEvents.root"
			  , (char*) 0)
    , _mp3(new mumsMomentum())
 {
    cout << " AmpsPdf with integ method " << method << endl;
    bool nonFlat = "efficient" == method;
    bool generateNew = ((string)_integratorSource == (string)"new");
    if(nonFlat){
      cout << "AmpsPdf uses nonFlat integration." << endl;
      if(generateNew){
	_sgGen =  new SignalGenerator(_amps);
	_sgGen->setWeighted();
	_sgGen->setSaveEvents(_integratorFileName);
	_chosenGen = _sgGen;
      }else{
	// here, SignalGenerator is used by FromFileGenerator, to fill
	// up missing events in case more are needed than found in the
	// file.  Since we don't know with which random seed the
	// events in the file were generated, we supply a random
	// number generator with randomised seed.
	_localRnd = new TRandom3(time(0));
	_sgGen =  new SignalGenerator(_amps, _localRnd);
	_sgGen->setWeighted();
	_sgGen->dontSaveEvents();// saving events is done by FromFileGenerator
	_fileGen   = new FromFileGenerator(_integratorFileName, _sgGen);
	_fileGen->setMothers3Momentum(_mp3);
	_chosenGen = _fileGen;
      }
      this->setEventGenerator(_chosenGen);
    }else{
      cout << "AmpsPdf uses flat integration." << endl;
    }
  }

  IFastAmplitudeIntegrable* getAmpSum(){ return _amps;}

  ~AmpsPdf(){
    if(0 != _fileGen)  delete _fileGen;
    if(0 != _sgGen)    delete _sgGen;
    if(0 != _localRnd) delete _localRnd;
  }
};

int ampFit(){
  time_t startTime = time(0);

  bool doFit=true;

  TRandom3 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed((int)RandomSeed);
  gRandom = &ranLux;

  FitAmplitude::AutogenerateFitFile();

  NamedParameter<string> InputFileName("InputFileName", (std::string) "");
  //  bool generateNew = (std::string) InputFileName == "";

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());
  
  cout << " got event pattern: " << pdg << endl;
  DalitzEventList eventListEff, eventList;

  MinuitParameterSet fitMPS;
  FitAmpSum fasEff(pdg, &fitMPS);
  fasEff.setEfficiency(counted_ptr<IGetDalitzEvent>(new effClass));
  SignalGenerator sgEff(&fasEff);
  sgEff.FillEventList(eventListEff, Nevents);

  /*
  FitAmpSum fas(pdg);
  SignalGenerator sg(&fas);
  sg.FillEventList(eventList, Nevents);
  */

  DalitzHistoSet datHEff = eventListEff.histoSet();
  DalitzHistoSet datH = eventList.histoSet();

  datH.drawWithFit(datHEff, "effHistos");
  (datHEff / datH).draw("effRatio");

  datH.save("plotsFromEventList.root");

  if(doFit){
    //  MinuitParameterSet fitMPS;
    AmpsPdf amps(&fasEff, &eventListEff, integPrecision, integMethod);
    Neg2LL<IDalitzEvent> fcn(&amps, &eventListEff, &fitMPS);
    
    //  DalitzPdfNormChecker nc(&amps, pdg);
    //  nc.checkNorm();
    
    Minimiser mini(&fcn);
    mini.doFit();
    mini.printResultVsInput();
    
    DalitzHistoSet fitH = amps.histoSet(); 
    fitH.save("plotsFromIntegrator.root");
    
    datH.draw("dataPlots_");
    fitH.draw("fitPlots_");
    datH.drawWithFit(fitH, "datFit_");

    AmpRatios rats(pdg);    
  }


  cout << " ampFit done. Took " << (time(0) - startTime)/60 
       << " min. Returning 0." << endl;
  return 0;
}


int main(){

  return ampFit();

}
//
