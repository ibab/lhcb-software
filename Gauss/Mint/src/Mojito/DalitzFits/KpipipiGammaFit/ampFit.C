// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "FitParameter.h"
#include "NamedParameter.h"
#include "Minimiser.h"
#include "Neg2LL.h"
#include "Neg2LLSum.h"
#include "DalitzEventList.h"

#include "CLHEPPhysicalConstants.h"


#include "PdfBase.h"
#include "DalitzPdfBase.h"
#include "DalitzPdfBaseFastInteg.h"
#include "FitAmplitude.h"
#include "FitAmpSum.h"

#include "DalitzEvent.h"

#include "AmpRatios.h"

#include "IEventGenerator.h"
#include "DalitzBWBoxSet.h"
#include "DalitzBoxSet.h"

#include "SignalGenerator.h"
#include "FromFileGenerator.h"

#include "cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"

#include "TRandom2.h"
#include "TRandom3.h"
#include <ctime>

#include <iostream>

#include "DalitzPdfNormChecker.h"

#include "IReturnComplex.h"
#include <complex>

using namespace std;
using namespace MINT;


class AmpRatioB : virtual public IReturnComplex{
  FitParameter& _rB;
  FitParameter& _delta;
  FitParameter& _gamma;
  int _Bcharge;
  double _DCSFactor;
public:
  AmpRatioB(FitParameter& rB, FitParameter& delta, FitParameter& gamma
	    , int Bcharge, double DCSFactor=1)
    : _rB(rB), _delta(delta), _gamma(gamma), _Bcharge(Bcharge), _DCSFactor(DCSFactor) {}
  
  complex<double> ComplexVal(){
    //return (double)_rB;
    std::complex<double> result = 
      polar((double) _rB, (double)(_delta - _gamma * (double) _Bcharge));
    //result = _rB;
    //if(_Bcharge < 0){
    //  result = 1./result;
    //}
    //cout << "result " << result << endl;
    return result/_DCSFactor;
  }
};



counted_ptr<FitAmpSum> makeGammaFitAmpSum(DalitzEventPattern pat
					  , const counted_ptr<AmpRatioB>& ar){
  counted_ptr<FitAmpSum> DSum( new FitAmpSum(pat) );
  //return DSum;
  counted_ptr<FitAmpSum> D0barSum(0);
  if(pat.selfConjugateFS()){
    cout << "makeGammaFitAmpSum: self-conjugate final state (pat = " << pat << ")" << endl;
    D0barSum = (counted_ptr<FitAmpSum>) DSum->GetCPConjugateSameFitParameters();
  }else{
    cout << "makeGammaFitAmpSum: not self-con. final state (pat = " << pat << ")" << endl;
    DalitzEventPattern patBar(pat);
    patBar[0].antiThis();
    D0barSum = counted_ptr<FitAmpSum>(new FitAmpSum(patBar));
    //return D0barSum;
  }
  DSum->multiply( (counted_ptr<IReturnComplex>) ar);
  DSum->add(*D0barSum);
  return DSum;
}

class AmpsPdf
  : public DalitzPdfBaseFastInteg
{
protected:
  TRandom* _localRnd;
  SignalGenerator* _sgGen;
  FromFileGenerator* _fileGen;
  IEventGenerator<IDalitzEvent>* _chosenGen;
  NamedParameter<std::string> _integratorSource;
  std::string _integratorFileName;
public:
  double un_normalised_noPs(){
    if(0 == getEvent()) return 0;
    double ampSq =  _amps->RealVal();
    return ampSq;// * getEvent()->phaseSpace();
  }

  AmpsPdf( const counted_ptr<FitAmpSum>& amps
	   , IDalitzEventList* events=0
	   , double precision=1.e-4
	   , const std::string& method="efficient"
	   , const std::string& integFileName="IntegFile.root"
	   )
    : DalitzPdfBaseFastInteg(events, 0, amps.get(), precision)
    , _localRnd(0)
    , _sgGen(0)
    , _fileGen(0)
    , _chosenGen(0)
    , _integratorSource("IntegratorSource", (std::string) "new", (char*) 0)
    , _integratorFileName(integFileName)
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

  TRandom3 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed((int)RandomSeed);
  gRandom = &ranLux;

  FitAmplitude::AutogenerateFitFile();

  NamedParameter<string> BpSignalFileName("BpSignalFileName", (std::string) "BpSg.root");
  NamedParameter<string> BmSignalFileName("BmSignalFileName", (std::string) "BmSg.root");

  NamedParameter<string> BpIntegFileName("BpIntegFileName", (std::string) "BpInteg.root");
  NamedParameter<string> BmIntegFileName("BmIntegFileName", (std::string) "BmInteg.root");

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pat(EventPattern.getVector());

  NamedParameter<double> DCSFactor("DCSFactor", 0.225*0.225);

  FitParameter rB("rB");
  FitParameter delta("delta");
  FitParameter gamma("gamma");


  counted_ptr<AmpRatioB> xiBplus(new AmpRatioB(rB, delta, gamma, 1, DCSFactor));
  counted_ptr<AmpRatioB> xiBminus(new AmpRatioB(rB, delta, gamma, -1, DCSFactor));

  counted_ptr<FitAmpSum> BpAmps = makeGammaFitAmpSum(pat, xiBplus);
  counted_ptr<FitAmpSum> BmAmps = makeGammaFitAmpSum(pat, xiBminus);


  cout << " got event pattern: " << pat << endl;
  DalitzEventList BpEventList, BmEventList;

  cout << "generating Bp events" << endl;


  //FitAmpSum justBp(pat);
  //SignalGenerator sgP(&justBp);

  SignalGenerator sgP(BpAmps.get());
  sgP.FillEventList(BpEventList, Nevents);

  cout << "done Bp." << endl;
  cout << "generating B- events:" << endl;
  SignalGenerator sgM(BmAmps.get());
  sgM.FillEventList(BmEventList, Nevents);
  cout << "done B-" << endl;

  DalitzHistoSet datHP = BpEventList.histoSet();
  datHP.save("plotsFromBpEventList.root");
  //DalitzHistoSet datHM = BmEventList.histoSet();
  //datHM.save("plotsFromBmEventList.root");

  AmpsPdf BpPdf(BpAmps, &BpEventList, integPrecision, integMethod
		, BpIntegFileName);
  BpPdf.setIntegratorFileName("spIntegrator");

  Neg2LL<IDalitzEvent> fcnP(&BpPdf, &BpEventList);

  AmpsPdf BmPdf(BmAmps, &BmEventList, integPrecision, integMethod
		, BmIntegFileName);
  BmPdf.setIntegratorFileName("smIntegrator");

  Neg2LL<IDalitzEvent> fcnM(&BmPdf, &BmEventList);

  Neg2LLSum fcn(&fcnP, &fcnM);
  
  /*
  DalitzPdfNormChecker nc(&BpPdf, pat);
  nc.checkNorm();
  */

  cout << "starting fit" << endl;
  Minimiser mini(&fcn);
  //Minimiser mini(&fcnP);
  mini.doFit();
  mini.printResultVsInput();
  BpPdf.doFinalStats();
  cout << "done fit" << endl;

  BpPdf.saveIntegrator("spIntegrator");
  BmPdf.saveIntegrator("smIntegrator");

  bool do2DScan=true;

  if(do2DScan){
    cout << "now doing 2D scan:" << endl;
    int scanBins=50;
    double scanMin=0, scanMax=2*pi;
    TFile* scanFile = new TFile("scan.root", "RECREATE");
    TH2D* scanHisto = new TH2D("scan", "scan", scanBins, scanMin, scanMax, scanBins, scanMin, scanMax);
    
    double scanMinLL=-9999;
    for(int i=0; i < scanBins; i++){
      double gamma_value = ((double)i+0.5)*2*pi/((double)scanBins);
      gamma.setCurrentFitVal(gamma_value);
      for(int j=0; j < scanBins; j++){
	double delta_value = ((double)j+0.5)*2*pi/((double)scanBins);
	delta.setCurrentFitVal(delta_value);
	double v = fcn.getVal();
	
	if( (i==0 && j==0) || v < scanMinLL) scanMinLL=v;
	
	scanHisto->Fill(gamma, delta, v);
      }
    }
    for(int i=0; i < scanBins; i++){
      double gamma_value = ((double)i+0.5)*2*pi/((double)scanBins);
      for(int j=0; j < scanBins; j++){
	double delta_value = ((double)j+0.5)*2*pi/((double)scanBins);
	scanHisto->Fill(gamma_value, delta_value, -scanMinLL + 1.e-20);
      }
    }
    scanFile->cd();
    scanHisto->Write();
    scanFile->Close();
    
    cout << "done 2-D scan" << endl;
  }

  DalitzHistoSet fitHP = BpPdf.histoSet(); 
  fitHP.save("plotsFromBpIntegrator.root");
  //  DalitzHistoSet fitHM = BmPdf.histoSet(); 
  //fitHM.save("plotsFromBmIntegrator.root");
  
  datHP.draw("dataPlotsBp_");
  fitHP.draw("fitPlotsBp_");
  datHP.drawWithFit(fitHP, "datFitBp_");
  
  /*
  datHM.draw("dataPlotsBm_");
  fitHM.draw("fitPlotsBm_");
  datHM.drawWithFit(fitHM, "datFitBm_");
  */
  
  cout << " ampFit done. Took " << (time(0) - startTime)/60 
       << " min. Returning 0." << endl;
  return 0;
}


int main(){

  return ampFit();

}
//
