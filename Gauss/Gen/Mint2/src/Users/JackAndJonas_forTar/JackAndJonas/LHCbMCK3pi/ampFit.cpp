// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Neg2LL.h"
#include "Mint/Neg2LLSum.h"
#include "Mint/DalitzEventList.h"
#include "Mint/NamedDecayTreeList.h"
#include "Mint/DecayTree.h"
#include "Mint/DiskResidentEventList.h"

#include "Mint/CLHEPPhysicalConstants.h"


#include "Mint/PdfBase.h"
#include "Mint/DalitzPdfBase.h"
#include "Mint/DalitzPdfBaseFastInteg.h"
#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpSum.h"
#include "Mint/FitAmpIncoherentSum.h"

#include "Mint/DalitzEvent.h"

#include "Mint/AmpRatios.h"

#include "Mint/IEventGenerator.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/DalitzBoxSet.h"

#include "Mint/SignalGenerator.h"
#include "Mint/FromFileGenerator.h"

#include "Mint/DalitzSumPdf.h"
#include "Mint/cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TNtupleD.h"

#include "TRandom2.h"
#include "TRandom3.h"

#include <ctime>

#include <iostream>

#include "Mint/DalitzPdfNormChecker.h"
#include "Mint/IFastAmplitudeIntegrable.h"
#include "Mint/DalitzPdfSaveInteg.h"


#include "Mint/Chi2Binning.h"
#include "Mint/FitAmpIncoherentSum.h"

#include "Mint/FitAmpList.h"
#include "Mint/IGetRealEvent.h"


#include "readGenNtp.h"

using namespace std;
using namespace MINT;


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
  
  AmpsPdf(IFastAmplitudeIntegrable* amps
	  , IDalitzEventList* events=0
	  , double precision=1.e-4
	  , std::string method="efficient"
	  , std::string fname =  "SignalIntegrationEvents.root", bool genMoreEvents = true
	  )
    : DalitzPdfBaseFastInteg(events, 0, amps, precision)
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

void randomise24(DalitzEventList& eventList){
  for(unsigned int i=0; i < eventList.size(); i++){
    if(gRandom->Rndm() > 0.5){
      TLorentzVector p2 = eventList[i].p(2);
      eventList[i].p(2)=eventList[i].p(4);
      eventList[i].p(4)=p2;    
    }
  }
}

int ampFit(){
  time_t startTime = time(0);
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
  NamedParameter<int> doScan("doScan",0);
  NamedParameter<int>  Nexperiments("Nexperiments", 10);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
  
  
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());
  
  NamedParameter<int> doNormCheck("doNormCheck", 0);
  
  cout << " got event pattern: " << pdg << endl;
  
  MinuitParameterSet fasSet;
  
  //  FitAmpSum fas((DalitzEventPattern)EventPattern, &fasSet);
  FitAmpIncoherentSum fas((DalitzEventPattern)EventPattern, &fasSet);

  // fas.setEfficiency(EffPtr); (don't want efficiency correction for CLEO-c - done through MC)
  
  
  time_t t0 = time(0);
  
  DalitzEventList eventList;
  
  /*
  cout << "reading events from file " << InputFileName << endl;
  eventList.fromFile(InputFileName);
  randomise24(eventList);
  eventList.save("EventsRandmised24.root");
  */
  readGenNtp ntupleReader(InputFileName);
  ntupleReader.readit(&eventList);
  cout << " I've got " << eventList.size() << " events." << endl;
  

  MinuitParameterSet fitMPS;
  AmpsPdf amps(&fas, &eventList, integPrecision, integMethod, (std::string) IntegratorEventFile); // Lauren
  amps.setIntegratorFileName("sgIntegrator");			// Lauren
  
  Neg2LL<IDalitzEvent> fcn(&amps, &eventList, &fasSet);
  Minimiser mini(&fcn);
  mini.doFit();
  mini.printResultVsInput();
  
  cout << "Saving each amplitude in a histogram." << endl;
  amps.saveEachAmpsHistograms("realDataHistos");
  amps.saveIntegrator("sgIntegrator");				//Lauren
  amps.saveEachAmpsHistograms("EachAmp");
  
  amps.doFinalStats(&mini);
  MinuitParameterSet::getDefaultSet()->printVariable();

  DalitzHistoSet datH = eventList.histoSet();
  datH.save("plotsFromEventList.root");
  DalitzHistoSet fitH = amps.histoSet(); 
  fitH.save("plotsFromIntegrator.root");
  
  datH.draw("dataPlots_");
  fitH.draw("fitPlots_");
  datH.drawWithFit(fitH, "datFit_");
  

  // from here
  DiskResidentEventList chi2Events("SignalIntegrationEvents.root");
  bool doChi = false;
  if (doChi){
    cout << "Doing Chi^2." << endl;
    Chi2Binning chi2;
    chi2.createBinning(&eventList, 100, 1000);
    chi2.setEventsAndPdf(&eventList, &chi2Events, &amps);
    cout << "New chi2: " << chi2.getChi2_perBin() << endl;
  }
  // until here
 

  cout << " Took " << (time(0) - startTime)/60. 
       << " min. Returning 0." << endl;
  

  return 0;
}


int main(){

  return ampFit();

}
//
