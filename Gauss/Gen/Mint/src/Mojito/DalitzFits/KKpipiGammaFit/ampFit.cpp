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

    bool dbThis=false;
    if(dbThis){
      cout << "AmpRatioB created with " 
	   << _rB << ", " << _delta << ", " << _gamma
	   << ", " << _Bcharge << ", " << DCSFactor << endl;
      cout << " will return with those values: " << ComplexVal() << endl;
    }
  }
  
  complex<double> ComplexVal(){
    std::complex<double> result = 
      polar((double) _rB, (double)(_delta - _gamma * (double) _Bcharge));

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
  std::string _integratorEventFile; // the events

  std::string _integratorOutputFile; // be default same as input
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
	   , const std::string& integInputFiles="sgIntegrator" // comma separated list of saved "fast" integrations.
	   , const std::string& integEventFile="IntegEventFile.root" // the events generated in the course of the integration
	   , const std::string& integOutputFile="" // if you want the integration to be saved in a different file than integInputFile
	   )
    : DalitzPdfBaseFastInteg(events, 0, amps.get(), precision)
    , _localRnd(0)
    , _sgGen(0)
    , _fileGen(0)
    , _chosenGen(0)
    , _integratorSource("IntegratorSource", (std::string) "new", (char*) 0)
    , _integratorEventFile(integEventFile)
    , _integratorOutputFile(integOutputFile)
 {
    cout << " AmpsPdf with integ method " << method << endl;

    setIntegratorFileName(integInputFiles);

    if("" == integOutputFile) _integratorOutputFile=integInputFiles;
    
    bool nonFlat = ("efficient" == method);
    bool generateNew = ((string)_integratorSource == (string)"new");
    if(nonFlat){
      cout << "AmpsPdf uses nonFlat integration." << endl;
      if(generateNew){
	_sgGen =  new SignalGenerator(_amps);
	_sgGen->setWeighted();
	_sgGen->setSaveEvents(_integratorEventFile);
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
	_fileGen   = new FromFileGenerator(_integratorEventFile, _sgGen);
	_chosenGen = _fileGen;
      }
      this->setEventGenerator(_chosenGen);
    }else{
      cout << "AmpsPdf uses flat integration." << endl;
    }
  }

  IFastAmplitudeIntegrable* getAmpSum(){ return _amps;}

  ~AmpsPdf(){
    saveIntegrator(_integratorOutputFile);
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

  NamedParameter<string> BpIntegFileName("BpIntegFileName", (std::string) "BpIntegrator");
  NamedParameter<string> BmIntegFileName("BmIntegFileName", (std::string) "BmIntegrator");

  NamedParameter<string> BpIntegEventFile("BpIntegEventFile", (std::string) "BpInteg.root");
  NamedParameter<string> BmIntegEventFile("BmIntegEventFile", (std::string) "BmInteg.root");

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pat(EventPattern.getVector());

  FitParameter rB("rB");
  FitParameter delta("delta");
  FitParameter gamma("gamma");

  counted_ptr<AmpRatioB> xiBplus(new AmpRatioB(rB, delta, gamma, 1));
  counted_ptr<AmpRatioB> xiBminus(new AmpRatioB(rB, delta, gamma, -1));

  counted_ptr<FitAmpSum> BpAmps = makeGammaFitAmpSum(pat, xiBplus);
  counted_ptr<FitAmpSum> BmAmps = makeGammaFitAmpSum(pat, xiBminus);


  cout << " got event pattern: " << pat << endl;
  DalitzEventList BpEventList, BmEventList;

  cout << "generating Bp events" << endl;
  SignalGenerator sgP(BpAmps.get());
  FromFileGenerator ffgP(BpSignalFileName, &sgP);
  ffgP.FillEventList(BpEventList, Nevents);
  cout << "done Bp." << endl;
  cout << "generating B- events:" << endl;
  SignalGenerator sgM(BmAmps.get());
  FromFileGenerator ffgM(BmSignalFileName, &sgM);
  ffgM.FillEventList(BmEventList, Nevents);
  cout << "done B-" << endl;

  DalitzHistoSet datHP = BpEventList.histoSet();
  datHP.save("plotsFromBpEventList.root");
  DalitzHistoSet datHM = BmEventList.histoSet();
  datHM.save("plotsFromBmEventList.root");


  AmpsPdf BpPdf(BpAmps, &BpEventList, integPrecision, integMethod
		, BpIntegFileName, BpIntegEventFile);
  Neg2LL<IDalitzEvent> fcnP(&BpPdf, &BpEventList);

  AmpsPdf BmPdf(BmAmps, &BmEventList, integPrecision, integMethod
		, BmIntegFileName, BmIntegEventFile);
  Neg2LL<IDalitzEvent> fcnM(&BmPdf, &BmEventList);

  Neg2LLSum fcn(&fcnP, &fcnM);
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
  
  datHP.draw("dataPlotsBp_");
  fitHP.draw("fitPlotsBp_");
  datHP.drawWithFit(fitHP, "datFitBp_");
  
  datHM.draw("dataPlotsBm_");
  fitHM.draw("fitPlotsBm_");
  datHM.drawWithFit(fitHM, "datFitBm_");
  
  cout << " ampFit done. Took " << (time(0) - startTime)/60 
       << " min. Returning 0." << endl;
  return 0;
}


int main(){

  return ampFit();

}
//
