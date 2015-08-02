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
#include "Mint/DalitzPdfBaseFlexiFastInteg.h"


#include "Mint/Chi2Binning.h"

using namespace std;
using namespace MINT;


class AmpsPdf
  : public DalitzPdfBaseFlexiFastInteg
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
    double ampSq =  _amps->RealVal(evt);
    return ampSq;// * getEvent()->phaseSpace();
  }
  
  AmpsPdf(const DalitzEventPattern& pat
	  // , IFastAmplitudeIntegrable* amps
	  , MinuitParameterSet* mps
	  , double precision=1.e-4
	  , std::string method="efficient"
	  , std::string fname =  "SignalIntegrationEvents.root", bool genMoreEvents = true
	  )
    : DalitzPdfBaseFlexiFastInteg(pat, 0, mps, precision)
    , _localRnd(0)
    , _sgGen(0)
    , _fileGen(0)
    , _chosenGen(0)
    , _integratorSource("IntegratorSource", (std::string) "new", (char*) 0)
    , _integratorFileName(fname)
  {
    cout << " AmpsPdf with integ method " << method << endl;
    bool nonFlat = "efficient" == method;
    bool generateNew = ((string)_integratorSource == (string)"new");
    if(nonFlat){
      cout << "AmpsPdf uses nonFlat integration." << endl;
      if(generateNew){
	_sgGen =  new SignalGenerator(pat);
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
	_sgGen =  new SignalGenerator(pat, _localRnd);
	_sgGen->setWeighted();
	_sgGen->dontSaveEvents();// saving events is done by FromFileGenerator
	if(genMoreEvents) _fileGen   = new FromFileGenerator(_integratorFileName, _sgGen);
	else{
	  _fileGen = new FromFileGenerator(_integratorFileName, 0);
	  cout << "not going to generate any more events" << endl;
	}
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
  MinuitParameterSet fitMPS;

  cout << "pset pointer in default " << MinuitParameterSet::getDefaultSet() << endl;
  cout << "pset pointer in ampsFit " << &fitMPS << endl;

  cout << "sizes " << MinuitParameterSet::getDefaultSet()->size()
       << ", " << fitMPS.size() << endl;


  TRandom3 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed((int)RandomSeed);
  gRandom = &ranLux;

  FitAmplitude::AutogenerateFitFile();

  NamedParameter<string> InputFileName("InputFileName", (std::string) "");
  bool generateNew = (std::string) InputFileName == "";

  NamedParameter<string> IntegratorEventFile("IntegratorEventFile"
					     , (std::string) "SignalIntegrationEvents.root"
					     , (char*) 0);
  NamedParameter<string> IntegratorInputFile("IntegratorInputFile"
					     , (std::string) "sgIntegrator"
					     , (char*) 0);
  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<std::string> integMethod("IntegMethod", (std::string) "efficient");
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);

  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pat(EventPattern.getVector());

  NamedParameter<int> doNormCheck("doNormCheck", 0);

  cout << " got event pattern: " << pat << endl;


  DalitzEventList eventList;

  cout << "1 pset pointer in default " << MinuitParameterSet::getDefaultSet() << endl;
  cout << "1 pset pointer in ampsFit " << &fitMPS << endl;

  cout << "1 sizes " << MinuitParameterSet::getDefaultSet()->size()
       << ", " << fitMPS.size() << endl;

  if(! generateNew){
    cout << "reading events from file " << InputFileName << endl;
    eventList.fromFile(InputFileName);
    cout << " I've got " << eventList.size() << " events." << endl;
  }

  if(generateNew){
    cout << "Generating " << Nevents << " signal events." << endl;
    SignalGenerator sg(pat);
    sg.FillEventList(eventList, Nevents);
    eventList.save("K3pi.root");
  }
  
  DalitzHistoSet datH = eventList.histoSet();
  datH.save("plotsFromEventList.root");


  cout << "2 pset pointer in default " << MinuitParameterSet::getDefaultSet() << endl;
  cout << "2 pset pointer in ampsFit " << &fitMPS << endl;

  cout << "2 sizes " << MinuitParameterSet::getDefaultSet()->size()
       << ", " << fitMPS.size() << endl;

  AmpsPdf amps(pat, 0// &fitMPS
	       , integPrecision
	       , integMethod
	       , (std::string) IntegratorEventFile); 
  

  cout << "3 pset pointer in default " << MinuitParameterSet::getDefaultSet() << endl;
  cout << "3 pset pointer in ampsFit " << &fitMPS << endl;

  cout << "3 sizes " << MinuitParameterSet::getDefaultSet()->size()
       << ", " << fitMPS.size() << endl;


  //amps.setIntegratorFileName("sgIntegrator");

  Neg2LL fcn(amps, eventList);//, &fitMPS);

  if((int) doNormCheck){
    DalitzPdfNormChecker nc(&amps, pat);
    nc.checkNorm();
  }

  
  Minimiser mini(&fcn);
  mini.doFit();
  mini.printResultVsInput();
  amps.saveEachAmpsHistograms("singleAmpHistos");

  //amps.saveIntegrator("sgIntegrator");
  amps.doFinalStats(&mini);

  /*
  DalitzEventList integratorEvents;
  integratorEvents.fromFile(IntegratorFileName);

  Chi2Binning chi2;
  chi2.createBinning(&eventList);
  chi2.setEventsAndPdf(&eventList
		       , &integratorEvents
		       , &amps
		       , amps.getAmpSum()
		       );
  cout << "chi2: " << chi2.getChi2_perBin() << endl;
  
  DalitzHistoSet fitH = amps.histoSet(); 
  fitH.save("plotsFromIntegrator.root");
  
  datH.draw("dataPlots_");
  fitH.draw("fitPlots_");
  datH.drawWithFit(fitH, "datFit_");
  
  //AmpRatios rats(pat);

  //rats.getRatios(amps.getAmpSum());

  //  cout << " saving list: " << endl;
  //  eventList.save();

  */

  DalitzHistoSet fitH = amps.histoSet(); 
  fitH.save("plotsFromIntegrator.root");
  
  datH.draw("dataPlots_");
  fitH.draw("fitPlots_");
  datH.drawWithFit(fitH, "datFit_");

  cout << " ampFit done. Took " << (time(0) - startTime)/60. 
       << " min. Returning 0." << endl;
  return 0;
}


int main(){

  return ampFit();

}
//
