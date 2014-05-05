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

  TRandom3 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed((int)RandomSeed);
  gRandom = &ranLux;

  FitAmplitude::AutogenerateFitFile();

  NamedParameter<string> InputFileName("InputFileName", (std::string) "");

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  Nexperiments("Nexperiments", 10);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<int>  makePlots("MakePlots", 0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
   
  NamedParameter<std::string> NtpName("NtpName"
				      , (std::string)"pull.root");
   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  MinuitParameterSet fasSet;
  FitAmpSum fas((DalitzEventPattern)EventPattern, &fasSet);

  TFile* paraFile = new TFile("pull.root", "RECREATE");
  TNtupleD* ntp=0;

  for(int j_ex = 0; j_ex < Nexperiments; j_ex++){
    time_t t0 = time(0);

    // production:
    DalitzEventList eventList;
    SignalGenerator sg(pdg);
    sg.FillEventList(eventList, Nevents);
   
    // fit:
    MinuitParameterSet fitMPS;
    AmpsPdf amps(&eventList, integPrecision, integMethod, &fitMPS);
    Neg2LL<IDalitzEvent> fcn(&amps, &eventList, &fitMPS);
    Minimiser mini(&fcn);
    mini.doFit();
    mini.printResultVsInput();
    fitMPS.fillNtp(paraFile, ntp); // store fit result

    // make plots:
    if(makePlots){
      DalitzHistoSet fitH = amps.histoSet(); 
      fitH.save("plotsFromIntegrator.root");
      
      DalitzHistoSet datH = eventList.histoSet();
      datH.save("plotsFromEventList.root");
      
      datH.draw("dataPlots_" + anythingToString(j_ex) + "_");
      fitH.draw("fitPlots_"  + anythingToString(j_ex) + "_");
      
      datH.drawWithFit(fitH, "datFit_" + anythingToString(j_ex) + "_");
    }

    ntp->AutoSave();
    cout << "==============================================" << endl;
    cout << " Done " << j_ex
	 << " th experiment. This one took " << (time(0) - t0)/60.0 
	 << " min.\n Total time since start " << (time(0) - startTime)/60.0
	 << " min." << endl;
    cout << "==============================================" << endl;
  }

  cout << "==============================================" << endl;
  cout << "ampPull done. " << (int) Nexperiments
       << " experiments took:" << (time(0) - startTime)/60.0
       << " min. This is an average of "
       << (Nexperiments<= 0 ? -9999 : ((time(0) - startTime)/60.0)/Nexperiments)
       << " min per experiment."
       << "\nReturning 0." << endl;
  cout << "==============================================" << endl;
  paraFile->Close();
  delete paraFile;
  return 0;
}


int main(){

  return ampFit();

}
//
