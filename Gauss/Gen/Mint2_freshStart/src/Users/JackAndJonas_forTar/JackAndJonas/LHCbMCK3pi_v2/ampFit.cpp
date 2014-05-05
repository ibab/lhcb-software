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
#include "readPhilipsNtuple.h"

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

#include "Mint/SumOfFitAmpLists.h"

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
    if(nonFlat){
      cout << "AmpsPdf uses nonFlat integration." << endl;
      if(false){//generateNew){
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

  DiskResidentEventList* getIntegrationEvents(){
    if(0 != _fileGen){
      return &(_fileGen->getEventList());
    }else{
      return 0;
    }
  }
  
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
  
  NamedParameter<string> InputFilename("InputFilename"
				       , (std::string) "data/mctruth_k3pi_withfourmom.root");
  NamedParameter<string> NtupleName("NtupleName"
				    , (std::string) "GenK3PiTuple/GenK3PiTuple");

  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());
  
  NamedParameter<int> doNormCheck("doNormCheck", 0);
  
  cout << " got event pattern: " << pdg << endl;
  
  MinuitParameterSet fasSet;
  
  counted_ptr<FitAmpSum> fasKstarRho(new FitAmpSum((DalitzEventPattern)EventPattern, &fasSet, "KstarRho_"));
  counted_ptr<FitAmpSum> fasKstarOmega(new FitAmpSum((DalitzEventPattern)EventPattern, &fasSet, "KstarOmega_"));
  counted_ptr<FitAmpSum> fasNonResRho(new FitAmpSum((DalitzEventPattern)EventPattern, &fasSet, "NonResRho_"));
  counted_ptr<FitAmpSum> fasNonResKstar(new FitAmpSum((DalitzEventPattern)EventPattern, &fasSet, "NonResKstar_"));
  counted_ptr<FitAmpIncoherentSum> fasI(new FitAmpIncoherentSum((DalitzEventPattern)EventPattern, &fasSet));
  
  SumOfFitAmpLists fas(fasKstarRho, fasKstarOmega, fasNonResRho, fasNonResKstar, fasI);

  
  // fas.setEfficiency(EffPtr); (don't want efficiency correction for CLEO-c - done through MC)
  
  int maxEvents= (int) Nevents;
  int minY = 3;
  int maxY = 4;
  int minP =    0*GeV;
  int maxP = 1000*GeV;
  cout << "doing " << maxEvents << " signal events - reading ntuple, now:" << endl;
  readPhilipsNtuple ntpReader((std::string) InputFilename
			      , (std::string) NtupleName
			      , -1
			      );

  DalitzEventList eventList;
  int counter=0;
  for(unsigned int i=0; i < ntpReader.numEvents() && (counter < maxEvents || maxEvents < 0); i++){
    counted_ptr<DalitzEvent> evtPtr = ntpReader.readEntry(i);
    if(0 == evtPtr) continue;
    //if(ntpReader.mcylab() < minY || ntpReader.mcylab() > maxY) continue;
    if(i < 20) cout << ntpReader.mcplab() << endl;
    if(ntpReader.plab() < minP || ntpReader.plab() > maxP) continue;
    eventList.Add(*evtPtr);
    ++counter;
  }
  cout << " I've got " << eventList.size() << " data events." << endl;
  //  randomise24(eventList);
  eventList.save("EventsRandmised24.root");
  
  
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
  // from here
  
  //amps.closeIntegratorFile();

  DalitzHistoSet datH = eventList.histoSet();
  datH.save("plotsFromEventList.root");
  DalitzHistoSet fitH = amps.histoSet(); 
  fitH.save("plotsFromIntegrator.root");
  
  datH.draw("dataPlots_");
  fitH.draw("fitPlots_");
  datH.drawWithFit(fitH, "datFit_");
  


  DiskResidentEventList* chi2Events;
  if(0 != (amps.getIntegrationEvents()) && amps.getIntegrationEvents()->size() > 0){
    chi2Events = amps.getIntegrationEvents();
  }else{
    chi2Events = new DiskResidentEventList("SignalIntegrationEvents.root");
  }
  cout << "about to copy list lists" << endl;


  bool beQuick=false;

  DalitzEventList fewChi2Events;
  DalitzEventList fewEvents;
  if(beQuick){
    for(unsigned int i=0; i < 2000 && i < chi2Events->size(); i++){
      DalitzEvent evt(chi2Events->getREvent(i));
      fewChi2Events.Add(evt);
    }
    cout << " copied chi2 events, now doing data" << endl;
    for(unsigned int i=0; i < 2000 && i < eventList.size(); i++){
      DalitzEvent evt(eventList[i]);
      fewEvents.Add(evt);
    }
    cout << "copied lists" << endl;
    cout << "data: " << fewEvents.size() << " events."
	 << " and MC: " << chi2Events->size() << endl;
  }
  bool doChi = true;
  if (doChi){
    cout << "Doing Chi^2." << endl;
    cout << "... using " << chi2Events->size() << "events" << endl;

    Chi2Binning chi2;
    if(beQuick){
      cout << "... using only " << fewChi2Events.size() << " events for chi2" << endl;
      chi2.createBinning(&fewEvents, 50, 200);
      cout << " done binning, now feeding events and amps" << endl;
      chi2.setEventsAndPdf(&fewEvents, &fewChi2Events, &amps);
    }else{
      cout << "... using " << chi2Events->size() << " events for chi2" << endl;
      chi2.createBinning(&eventList, 100, 1000);
      cout << " done binning, now feeding events and amps" << endl;
      chi2.setEventsAndPdf(&eventList, chi2Events, &amps);
    }
    cout << "done that, now printing and calculating chi2" << endl;
    chi2.print();
    cout << "done that, re-doing chi2" << endl;
    cout << "New chi2: " << chi2.getChi2_perBin() << endl;
    cout << " done that, now getting histograms " << endl;
    DalitzHistoStackSet chi2hdata = chi2.getDataHistoStack();
    cout << "got data histos... "<< endl;
    DalitzHistoStackSet chi2hmc = chi2.getMCHistoStack();
    cout << " also got mc histos, now drawing" << endl;
    chi2hdata.draw("chi2DataHisto");
    cout << "drew data histo "<< endl;
    chi2hmc.draw("chi2MCHisto");
    cout << "also drew MC histos" << endl;
    chi2.drawChi2Distribution();

  }

  cout << " Took " << (time(0) - startTime)/60. 
       << " min. Returning 0." << endl;
  

  return 0;
}


int main(){

  return ampFit();

}
//
