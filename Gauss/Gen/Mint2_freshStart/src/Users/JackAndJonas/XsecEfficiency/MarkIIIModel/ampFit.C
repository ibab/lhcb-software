// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "FitParameter.h"
#include "NamedParameter.h"
#include "DalitzEventList.h"
#include "NamedDecayTreeList.h"
#include "DecayTree.h"
#include "DiskResidentEventList.h"

#include "CLHEPPhysicalConstants.h"


#include "PdfBase.h"
#include "DalitzPdfBase.h"
#include "DalitzPdfBaseFastInteg.h"
#include "FitAmplitude.h"
#include "FitAmpSum.h"
#include "FitAmpIncoherentSum.h"

#include "DalitzEvent.h"

#include "AmpRatios.h"

#include "IEventGenerator.h"
#include "DalitzBWBoxSet.h"
#include "DalitzBoxSet.h"

#include "SignalGenerator.h"
#include "FromFileGenerator.h"

#include "DalitzSumPdf.h"
#include "cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TNtupleD.h"

#include "TRandom2.h"
#include "TRandom3.h"

#include <ctime>

#include <iostream>

#include "DalitzPdfNormChecker.h"
#include "IFastAmplitudeIntegrable.h"
#include "DalitzPdfSaveInteg.h"


#include "Chi2Binning.h"
#include "FitAmpIncoherentSum.h"

#include "FitAmpList.h"
#include "IGetRealEvent.h"


#include "readPhilipsNtuple.h"

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

int ampFit(){
  time_t startTime = time(0);
  TRandom3 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed((int)RandomSeed);
  gRandom = &ranLux;
  
  FitAmplitude::AutogenerateFitFile();
  
  NamedParameter<string> InputFilenameBefore("InputFilenameBefore"
					     , (std::string) "data/mctruth_k3pi_withfourmom.root");
  NamedParameter<string> NtupleNameBefore("NtupleNameBefore"
					  , (std::string) "mctruth_k3pi");

  NamedParameter<string> InputFilenameAfter("InputFilenameAfter"
					    , (std::string) "data/mc_k3pi_withfourmom.root");
  NamedParameter<string> NtupleNameAfter("NtupleNameAfter"
					  , (std::string) "mc_k3pi");
  
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
  
  time_t t0 = time(0);
  
  // fill event lists:
  DiskResidentEventList eventListBefore("EventListBeforeSelection.root", "RECREATE");
  DiskResidentEventList eventListAfter("EventListAfterSelection.root", "RECREATE");

  readPhilipsNtuple ntpReaderBefore( eventListBefore
				     , (std::string) InputFilenameBefore
				     , (std::string) NtupleNameBefore
				     );
  cout << " I've got " << eventListBefore.size() << " before selection." << endl;
  DalitzHistoSet datHBefore = eventListBefore.histoSet();
  datHBefore.save("plotsFromEventListBefore.root");
  datHBefore.draw("dat_before");

  readPhilipsNtuple ntpReaderAfter( eventListAfter
				    , (std::string) InputFilenameAfter
				    , (std::string) NtupleNameAfter
				    );
  cout << " I've got " << eventListAfter.size() << " selected events." << endl;
  DalitzHistoSet datHAfter = eventListAfter.histoSet();
  datHAfter.save("plotsFromEventListAfter.root");
  datHAfter.draw("dat_after");

  // make PDF's
  MinuitParameterSet LHCbParameters, MarkIIIParameters;

  FitAmpIncoherentSum LHCbAmps(pdg, (const char*) "Inco_LHCbResult.txt", &LHCbParameters);
  FitAmpSum MarkIIIAmps(pdg, (const char*) "markIII.txt", &MarkIIIParameters);
  
  cout << "printing MarKIII amplitudes: " << endl;
  MarkIIIAmps.printNonZero();

  cout << "printing LHCb amplitudes: " << endl;
  LHCbAmps.printNonZero();

  AmpsPdf LHCbPdf(&LHCbAmps, &eventListBefore, integPrecision, integMethod
		  , IntegratorEventFileLHCb);
  AmpsPdf MarkIIIPdf(&MarkIIIAmps, &eventListBefore, integPrecision, integMethod
		     , IntegratorEventFileMarkIII);


  // re-weight events before selection
  DiskResidentEventList reweightedEventListBefore("ReweightedEventListBeforeSelection.root", "RECREATE");
  DiskResidentEventList reweightedEventListAfter("ReweightedEventListAfterSelection.root", "RECREATE");


  double totalWeightBefore=0;
  double totalWeightSquaredBefore=0;
  eventListBefore.Start();
  cout << "starting loop " << endl;
  int counter=0;
  while(eventListBefore.Next()){
    counter++;
    double wM = MarkIIIPdf.getVal();
    double wL = LHCbPdf.getVal();
    double w = wM/wL;
    DalitzEvent evt(eventListBefore.getEvent());
    evt.setWeight(w);
    reweightedEventListBefore.Add(evt);
    totalWeightBefore += w;
    totalWeightSquaredBefore += w*w;
  }
  reweightedEventListBefore.save();
  DalitzHistoSet datHWeightedBefore = reweightedEventListBefore.weightedHistoSet();
  datHWeightedBefore.draw("datweighted_before");
  datHBefore.drawWithFit(datHWeightedBefore, "before_McDots_ReweightedLine");

  // re-weight events after selection
  double totalWeightAfter=0;
  double totalWeightSquaredAfter=0;
  LHCbPdf.setEventRecord(&eventListAfter);
  MarkIIIPdf.setEventRecord(&eventListAfter);

  eventListAfter.Start();
  cout << "starting \"after\" loop " << endl;
  while(eventListAfter.Next()){
    counter++;
    double wM = MarkIIIPdf.getVal();
    double wL = LHCbPdf.getVal();
    double w = wM/wL;
    DalitzEvent evt(eventListAfter.getEvent());
    evt.setWeight(w);
    reweightedEventListAfter.Add(evt);
    totalWeightAfter += w;
    totalWeightSquaredAfter += w*w;
  }
  reweightedEventListAfter.save();
  DalitzHistoSet datHWeightedAfter = reweightedEventListAfter.weightedHistoSet();
  datHWeightedAfter.draw("datweighted_after");
  datHAfter.drawWithFit(datHWeightedAfter, "after_McDots_ReweightedLine");

  double dN_after = eventListAfter.size();
  double dN_before = eventListBefore.size();

  double meanWB = totalWeightBefore/dN_before;
  double meanW2B = totalWeightSquaredBefore/dN_before;

  double meanWA = totalWeightAfter/dN_after;
  double meanW2A = totalWeightSquaredAfter/dN_after;

  double sigmaMeanB = sqrt(meanW2B - meanWB*meanWB);
  double sigmaMeanA = sqrt(meanW2A - meanWA*meanWA);
  
  double sigmaWeightedBefore = sigmaMeanB*sqrt(dN_before);
  double sigmaWeightedAfter =  sigmaMeanA*sqrt(dN_after);

  cout << "total weight before " << totalWeightBefore << " +/- " << sigmaWeightedBefore << endl;
  cout << "total weight after " << totalWeightAfter << " +/- " <<  sigmaWeightedAfter << endl;


  double eff_original =  dN_after/dN_before;
  double sigma_eff_original = sqrt(dN_after/(dN_after*dN_after) 
				   + dN_before/(dN_before*dN_before)) * eff_original;

  double eff_reweighted = totalWeightAfter/totalWeightBefore ;
  double sigma_eff_weighted = 
    sqrt(sigmaWeightedAfter*sigmaWeightedAfter/(totalWeightAfter*totalWeightAfter) 
	 + sigmaWeightedBefore*sigmaWeightedBefore/(totalWeightBefore*totalWeightBefore))
    * eff_reweighted;

  cout << "efficiency w/o reweighting: " << eff_original  << " +/- " << sigma_eff_original << endl;
  cout << "efficiency w/ reweighting: " << eff_reweighted << " +/- " << sigma_eff_weighted << endl;
  cout << "ratio of the reweighted/original = " << eff_reweighted/eff_original << endl;

  datHWeightedBefore.drawWithFitNorm(datHWeightedAfter, "noEffDots_effLines_Weighted");
  datHBefore.drawWithFitNorm(datHAfter, "noEffDots_effLines_original");

  LHCbPdf.saveIntegrator("LHCbIntegrator");
  MarkIIIPdf.saveIntegrator("MarkIIIIntegrator");

  return 0;
}


int main(){

  return ampFit();

}
//
