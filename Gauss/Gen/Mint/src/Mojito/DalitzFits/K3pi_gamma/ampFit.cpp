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
#include "TH2F.h"

#include "TRandom2.h"
#include "TRandom3.h"
#include <ctime>

#include <iostream>

#include "Mint/DalitzPdfNormChecker.h"

#include "Mint/IReturnComplex.h"
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
	    , int Bcharge, double DCSFactor=1.0)
    : _rB(rB),  _delta(delta), _gamma(gamma), _Bcharge(Bcharge), _DCSFactor(DCSFactor) {

    bool dbThis=true;
    if(dbThis){
      cout << "AmpRatioB created with " 
	   << _rB << ", " << _delta << ", " << _gamma
	   << ", " << _Bcharge << ", " << _DCSFactor << endl;
      cout << " will return with those values: " << ComplexVal() << endl;
    }
  }
  
  complex<double> ComplexVal(){
    std::complex<double> result = 
      polar((double) _rB, (double)(_delta + _gamma * (double) _Bcharge));

    return result/_DCSFactor;
  }
};



counted_ptr<FitAmpSum> makeGammaFitAmpSum(DalitzEventPattern pat
					  , const counted_ptr<AmpRatioB>& ar){
  counted_ptr<FitAmpSum> DSum( new FitAmpSum(pat) );
  counted_ptr<FitAmpSum> D0barSum(0);
  if(pat.selfConjugateFS()){
    cout << " self-conjugate final state " << endl;
    D0barSum = (counted_ptr<FitAmpSum>) DSum->GetCPConjugateSameFitParameters();
  }else{
    DalitzEventPattern patBar(pat);
    patBar[0].antiThis();
    cout << " not self-conjugate final state " << endl;
    D0barSum = counted_ptr<FitAmpSum>(new FitAmpSum(patBar));
  }
  DSum->multiply( (counted_ptr<IReturnComplex>) ar);
  cout << "multiplying Dbar amplitudes with: " << ar->ComplexVal() << endl;
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
  double un_normalised_noPs(IDalitzEvent& evt){
    return _amps->RealVal(evt);
  }

  AmpsPdf( const DalitzEventPattern& pat, const counted_ptr<FitAmpSum>& amps
	   , double precision=1.e-4
	   , const std::string& method="efficient"
	   , const std::string& integFileName="IntegFile.root"
	   )
    : DalitzPdfBaseFastInteg(0, amps.get(), precision)
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
	_sgGen =  new SignalGenerator(pat, _amps);
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
	_sgGen =  new SignalGenerator(pat, _amps, _localRnd);
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
  NamedParameter<int> RandomSeed("RandomSeed",0);
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


  NamedParameter<double> DCSFactor("DCSFactor", 0.225*0.225);
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pat(EventPattern.getVector());

  FitAmplitude::AutogenerateFitFile("AllAmpsForThisPattern.txt", pat);


  FitParameter rB("rB");
  FitParameter delta("delta");
  FitParameter gamma("gamma");

  //  return 0;
  
  counted_ptr<AmpRatioB> xiBplus(new AmpRatioB(rB, delta, gamma, 1, DCSFactor));
  counted_ptr<AmpRatioB> xiBminus(new AmpRatioB(rB, delta, gamma, -1, DCSFactor));

  counted_ptr<FitAmpSum> BpAmps = makeGammaFitAmpSum(pat, xiBplus);
  counted_ptr<FitAmpSum> BmAmps = makeGammaFitAmpSum(pat, xiBminus);

  cout << " got event pattern: " << pat << endl;
  DalitzEventList BpEventList, BmEventList;
  
  /*
  cout << "generating Bp events" << endl;
  SignalGenerator sgP(BpAmps.get());
  FromFileGenerator ffgP(BpSignalFileName, &sgP);
  ffgP.FillEventList(BpEventList, Nevents);
  cout << "done Bp." << endl;
  */

  SignalGenerator sgM(pat, BmAmps.get());
  sgM.FillEventList(BmEventList, Nevents);

//    BpEventList.fromFile("BpSg.root");

  //  return 0;
  
  /*
  cout << "generating B- events:" << endl;
  SignalGenerator sgM(BmAmps.get());
  FromFileGenerator ffgM(BmSignalFileName, &sgM);
  ffgM.FillEventList(BmEventList, Nevents);
  cout << "done B-" << endl;
  */
  SignalGenerator sgP(pat, BpAmps.get());
  sgP.FillEventList(BpEventList, Nevents);

  DalitzHistoSet datHP = BpEventList.histoSet();
  datHP.save("plotsFromBpEventList.root");
  DalitzHistoSet datHM = BmEventList.histoSet();
  datHM.save("plotsFromBmEventList.root");

  AmpsPdf BpPdf(pat, BpAmps, integPrecision, integMethod
      		, BpIntegFileName);
  
  BpPdf.setIntegratorFileName("spIntegrator_test");

  Neg2LL fcnP(BpPdf, BpEventList);
  
  AmpsPdf BmPdf(pat, BmAmps, integPrecision, integMethod
  		, BmIntegFileName);
  
  BmPdf.setIntegratorFileName("smIntegrator_test");
  
  Neg2LL fcnM(BmPdf, BmEventList);

  Neg2LLSum fcn(&fcnP, &fcnM);

  bool do2DScan=true;
  if(do2DScan){
    int scanBins=50;
    double scanMin=0, scanMax=2*pi;
    TFile* scanFile = new TFile("scan.root", "RECREATE");
    TH2D* scanHisto = new TH2D("scan", "scan", scanBins, scanMin, scanMax, scanBins, scanMin, scanMax);
    
    double scanMinLL=-9999;
    for(int i=0; i < scanBins; i++){
      double gamma_value = scanMin + ((double)i+0.5)*(scanMax - scanMin)/((double)scanBins);
      gamma.setCurrentFitVal(gamma_value);
      for(int j=0; j < scanBins; j++){
	double delta_value = scanMin + ((double)j+0.5)*(scanMax-scanMin)/((double)scanBins);
	delta.setCurrentFitVal(delta_value);

	fcn.parametersChanged();
	double v = fcn.getVal();
	
	if( (i==0 && j==0) || v < scanMinLL) scanMinLL=v;
	
	scanHisto->Fill(gamma, delta, v);
      }
    }
    for(int i=0; i < scanBins; i++){
      double gamma_value = ((double)i+0.5)*2*pi/((double)scanBins);
      for(int j=0; j < scanBins; j++){
	double delta_value = ((double)j+0.5)*2*pi/((double)scanBins);
	scanHisto->Fill(gamma_value, delta_value, -scanMinLL);
      }
    }
    scanFile->cd();
    scanHisto->Write();
    scanFile->Close();
    
    gamma.resetToInit();
    delta.resetToInit();
  }
  //  return 0;


  /*
  DalitzPdfNormChecker nc(&BpPdf, pat);
  nc.checkNorm();
  */
  cout << "starting fit" << endl;
  Minimiser mini(&fcn);
  mini.doFit();
  mini.printResultVsInput();
  cout << "done fit" << endl;
  
  
  DalitzHistoSet fitHP = BpPdf.histoSet(); 
  fitHP.save("plotsFromBpIntegrator.root");
  DalitzHistoSet fitHM = BmPdf.histoSet(); 
  fitHM.save("plotsFromBmIntegrator.root");
  
  datHP.draw("dataPlotsBp_", "", "png");
  fitHP.draw("fitPlotsBp_", "", "png");
  datHP.drawWithFit(fitHP, "datFitBp_", "png");
  
  
  datHM.draw("dataPlotsBm_", "", "png");
  fitHM.draw("fitPlotsBm_", "", "png");
  datHM.drawWithFit(fitHM, "datFitBm_", "png");
  
  cout << "about to save integrator files" << endl;
  BpPdf.saveIntegrator("spIntegrator_test");
  cout << "saved B+ integrator" << endl;

  BmPdf.saveIntegrator("smIntegrator_test");
  cout << "saved B- integrator." << endl;

  cout << " ampFit done. Took " << (time(0) - startTime)/60 
       << " min. Returning 0." << endl;

  return 0;
}


int main(){

  return ampFit();

}
//
