// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
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
#include "Mint/SumOfFitAmpLists.h"

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

#include "Mint/Utils.h"


#include "readPhilipsNtuple.h"
#include "EventCounter.h"
#include "WeightedCount.h"

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

int justReweight(){

  time_t t0 = time(0);

  TRandom3 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed((int)RandomSeed);
  gRandom = &ranLux;
  
  FitAmplitude::AutogenerateFitFile();
  
  NamedParameter<string> InputFilenameBefore("InputFilenameBefore"
					     , (std::string) "data/mctruth_k3pi_withfourmom.root");
  NamedParameter<string> NtupleNameBefore("NtupleNameBefore"
					  , (std::string) "mctruth_k3pi");

  
  NamedParameter<string> IntegratorEventFileLHCb("IntegratorEventFileLHCb"
					     , (std::string) "IntegrationEventsLHCb.root"
					     , (char*) 0);
  NamedParameter<string> IntegratorEventFileMarkIII("IntegratorEventFileMarkIII"
					     , (std::string) "IntegrationEventsMarkIII.root"
					     , (char*) 0);
  NamedParameter<string> IntegratorInputFileLHCb("IntegratorInputFileLHCb"
					     , (std::string) "sgIntegratorLHCb"
					     , (char*) 0);
  NamedParameter<string> IntegratorInputFileMarkIII("IntegratorInputFileMarkIII"
					     , (std::string) "sgIntegratorMarkIII"
					     , (char*) 0);

  NamedParameter<int> doScan("doScan",0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
  
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  NamedParameter<std::string> WeightBranchName("WeightBranchName"
					  , (std::string)"w");
  NamedParameter<std::string> LHCbFileName("LHCbFileName"
					   , (std::string)"LHCb_semi.txt");
  NamedParameter<std::string> ModelFileBaseName("ModelFileBaseName"
					    , (std::string)"Kpipipi_MarkIII_betterResults_pf");
  NamedParameter<int> NumWeightBranches("NumWeightBranches", 11);

  DalitzEventList dummyList;
  dummyList.generatePhaseSpaceEvents(10, pdg);

  
  cout << " got event pattern: " << pdg << endl;
  
  int maxEvents = -1000;

  readPhilipsNtuple ntpReaderBefore((std::string) InputFilenameBefore
				    , (std::string) NtupleNameBefore
				    , maxEvents
				    , (std::string) WeightBranchName
				    , NumWeightBranches
				     );

  MinuitParameterSet LHCbParameters;

  counted_ptr<FitAmpSum> fasKstarRho(new FitAmpSum((DalitzEventPattern)EventPattern
						   , ((std::string) LHCbFileName).c_str()
						   , &LHCbParameters, "KstarRho_"));
  counted_ptr<FitAmpSum> fasKstarOmega(new FitAmpSum((DalitzEventPattern)EventPattern
						     , ((std::string) LHCbFileName).c_str()
						     , &LHCbParameters, "KstarOmega_"));
  counted_ptr<FitAmpSum> fasNonResRho(new FitAmpSum((DalitzEventPattern)EventPattern
						    , ((std::string) LHCbFileName).c_str()
						    , &LHCbParameters, "NonResRho_"));
  counted_ptr<FitAmpSum> fasNonResKstar(new FitAmpSum((DalitzEventPattern)EventPattern
						      , ((std::string) LHCbFileName).c_str()
						      , &LHCbParameters, "NonResKstar_"));
  counted_ptr<FitAmpIncoherentSum> fasI(new FitAmpIncoherentSum((DalitzEventPattern)EventPattern
								, ((std::string) LHCbFileName).c_str()
								, &LHCbParameters));
  
  SumOfFitAmpLists LHCbAmps(fasKstarRho, fasKstarOmega, fasNonResRho, fasNonResKstar, fasI);


  AmpsPdf LHCbPdf(&LHCbAmps, &dummyList, integPrecision, integMethod
		  , IntegratorEventFileLHCb);
  LHCbPdf.setIntegratorFileName("LHCbIntegrator");


  std::vector<IGetDalitzEvent*> ModelList(NumWeightBranches, 0);

  for(int i=0; i < NumWeightBranches; i++){
    cout << "========== Reading model " << i << " ========== " << endl;
    std::string fname_postfix="";
    if(0 != i) fname_postfix = "_rnd_" + anythingToString(i);
    fname_postfix += ".txt";

    MinuitParameterSet* MarkIIIParametersPtr = new MinuitParameterSet;
    FitAmpSum* MarkIIIAmps = new FitAmpSum(pdg
					  , ((std::string) ModelFileBaseName + fname_postfix).c_str()
					  , MarkIIIParametersPtr);
    
    AmpsPdf* MarkIIIPdf = new AmpsPdf(MarkIIIAmps, &dummyList, integPrecision, integMethod
				     , IntegratorEventFileMarkIII);
    MarkIIIPdf->setIntegratorFileName("MarkIIIIntegrator");
    
    // both, using the amplitudes and the pdfs works. Only differnce: PDFs are 
    // normalised. Should not matter for efficiency ratios, but will give nicer numbers.
    // and mighte useful elsewhere

    ModelList[i] = MarkIIIPdf;
  }
  
  WeightedCount cbefore("beforeCuts_"
			, &LHCbPdf, ModelList, &ntpReaderBefore);
  cbefore.doCount();
  //cbefore.drawAndSave();
  DalitzHistoSet noWeightsNoCutsH   = cbefore.getUnweightedHistos();
  DalitzHistoSet withWeightsNoCutsH = cbefore.getReweightedHistos();
  
  //  LHCbPdf.doFinalStats();
  //  MarkIIIPdf.doFinalStats();

  noWeightsNoCutsH.drawWithFit(withWeightsNoCutsH, "noWeightsDots_WITHWeightsLine_noCuts");

  //  LHCbPdf.saveIntegrator("LHCbIntegrator");
  //  MarkIIIPdf.saveIntegrator("MarkIIIIntegrator");

  ntpReaderBefore.save();

  cout << "saved ntpReaders" << endl;

  cout << "Done. This took " << (time(0) - t0)/60 << " min." << endl;
  return 0;
}


int main(){

  return justReweight();

}
//
