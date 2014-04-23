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
#include "Mint/SignalGenerator.h"
#include "Mint/FromFileGenerator.h"

#include "Mint/DalitzBWBoxSet.h"
#include "Mint/DalitzBoxSet.h"

#include "Mint/MinuitParameterSet.h"

#include "Mint/cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TNtupleD.h"

#include "TRandom2.h"
#include "TRandom3.h"

#include "AsciiEventReader.h"
#include "Mint/Chi2Binning.h"

#include <ctime>

#include <iostream>

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
  NamedParameter<std::string> _integratorFileName;
public:
  double un_normalised_noPs(){
    if(0 == getEvent()) return 0;
    double ampSq =  _amps->RealVal();
    return ampSq;// * getEvent()->phaseSpace();
  }

  AmpsPdf(IDalitzEventList* events=0
	  , double precision=1.e-4
	  , std::string method="efficient"
	  , MinuitParameterSet* mps=0
	  )
    : DalitzPdfBaseFastInteg(events, 0, mps, precision)
    , _localRnd(0)
    , _sgGen(0)
    , _fileGen(0)
    , _chosenGen(0)
    , _integratorSource("IntegratorSource", (std::string) "new", (char*) 0)
    , _integratorFileName("IntegratorFileName"
			  , (std::string) "SignalIntegrationEvents.root"
			  , (char*) 0)
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

  // ====== get NamedParameters (can be done at any place in the code, 
  // ====== grouped here in the beginning for tidyness)
  NamedParameter<int> RandomSeed("RandomSeed", 0);

  NamedParameter<string> SgAsciiFilename("SgAsciiFilename"
					 , (std::string) "Data/All_KsPP_MC.txt");

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  Nexperiments("Nexperiments", 10);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<int>  makePlots("MakePlots", 0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
      
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());
  //============================================================

  // some initialisation
  TRandom3 ranLux;
  ranLux.SetSeed((int)RandomSeed);
  gRandom = &ranLux;
  FitAmplitude::AutogenerateFitFile();

  // read in events:
  AsciiEventReader asciiReader(SgAsciiFilename);
  counted_ptr<DalitzEventList> evtSgListPtr = asciiReader.getEvtList();

  // counted_ptr<DalitzEventList> tester(new DalitzEventList);
  //  tester->fromFile("SignalIntegrationEventsKspipi.root");
  //evtSgListPtr = tester;

  if(0 == evtSgListPtr){
    cout << "ampFit.C: evtSgListPtr is zero - bailing out" << endl;
    return 1;
  }
  DalitzHistoSet datSgH = evtSgListPtr->histoSet();
  datSgH.draw("dataPlotsSg_");

  // The fit itself
  MinuitParameterSet fitMPS;
  AmpsPdf ampsSg(evtSgListPtr.get(), integPrecision, integMethod, &fitMPS);
  Neg2LL<IDalitzEvent> fcn(&ampsSg, evtSgListPtr.get(), &fitMPS);
  
  ampsSg.setIntegratorFileName("sgIntegrator");

  Minimiser mini(&fcn);
  //  mini.doFit();
  //mini.printResultVsInput();

  DiskResidentEventList dL("chi2Events.root");
  SignalGenerator sgGen(ampsSg.getAmpSum());
  for(int i=0; i < 10000; i++){
    dL.Add(sgGen.newEvent().get());
  }
  
  Chi2Binning chi2;
  cout << "about to create binning" << endl;
  chi2.createBinning(evtSgListPtr.get());
  cout << " done that, now setting pdf " << endl;
  chi2.setEventsAndPdf(evtSgListPtr.get()
                       , &dL
                       , &ampsSg);
  cout << " so far so good - now what's the chi2?" << endl;
  cout << "chi2: " << chi2.getChi2_perBin() << endl;
  

  //ampsSg.saveIntegrator("sgIntegrator");

  ampsSg.doFinalStats(&mini);

  DalitzHistoSet fitSgH = ampsSg.histoSet() ;
  fitSgH.draw("fitPlotsSg_");
  datSgH.drawWithFit(fitSgH, "datFitSg_");

  cout << "====================================================+===========\n"
       << "   Done. This took " << (time(0) - startTime)/60.0 << " min.\n "
       << "================================================================" 
       << endl;

  return 0;
}


int main(){

  return ampFit();

}
//
