// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/Mint/FitParameter/FitParameter.h"
#include "Mint/Mint/NamedParameter/NamedParameter.h"
#include "Mint/Mint/Minimiser/Minimiser.h"
#include "Mint/Mint/PdfAndLogL/Neg2LL.h"
#include "Mint/Mint/PdfAndLogL/Neg2LLSum.h"
#include "Mint/Mojito/DalitzEvents/DalitzEventList.h"

#include "Mint/Mint/Utils/CLHEPPhysicalConstants.h"


#include "Mint/Mint/PdfAndLogL/PdfBase.h"
#include "Mint/Mojito/DalitzFits/DalitzPdfBase.h"
#include "Mint/Mojito/DalitzFits/DalitzPdfBaseFastInteg.h"
#include "Mint/Mojito/FitAmplitude/FitAmplitude.h"
#include "Mint/Mojito/FitAmplitude/FitAmpSum.h"
#include "Mint/Mojito/FitAmplitude/FitAmpIncoherentSum.h"

#include "Mint/Mojito/DalitzEvents/DalitzEvent.h"

#include "Mint/Mojito/Ratios/AmpRatios.h"

#include "Mint/Mint/Events/IEventGenerator.h"
#include "Mint/Mojito/BreitWignerMC/DalitzBWBoxSet.h"
#include "Mint/Mojito/BetterMC/DalitzBoxSet.h"

#include "Mint/Mojito/DalitzEventGeneration/SignalGenerator.h"
#include "Mint/Mojito/DalitzEventGeneration/FromFileGenerator.h"

#include "Mint/Mojito/DalitzFits/DalitzSumPdf.h"

#include "Mint/Mint/Utils/cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"

#include "TRandom2.h"
#include "TRandom3.h"
#include <ctime>

#include <iostream>

#include "Mint/Mojito/DalitzFits/DalitzPdfNormChecker.h"
#include "Mint/Mojito/DalitzIntegrator/IFastAmplitudeIntegrable.h"

using namespace std;
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
	  , std::string fname =  "SignalIntegrationEvents.root"
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
  bool generateNew = (std::string) InputFileName == "";

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());
  
  cout << " got event pattern: " << pdg << endl;
  DalitzEventList eventList;

  if(! generateNew){
    cout << "reading events from file " << InputFileName << endl;
    eventList.fromFile(InputFileName);
    cout << " I've got " << eventList.size() << " events." << endl;
  }

  MinuitParameterSet fitMPS;
  FitParameter sgFrac("SignalFraction");
  FitAmpSum sg(pdg, &fitMPS);
  FitAmpIncoherentSum bg(pdg, &fitMPS);

  if(generateNew){
    SignalGenerator sggen(&sg);
    sggen.FillEventList(eventList, Nevents * sgFrac);
    SignalGenerator bggen(&bg);
    bggen.FillEventList(eventList, Nevents*(1.0-sgFrac));
  }
  
  DalitzHistoSet datH = eventList.histoSet();
  datH.save("plotsFromEventList.root");

  NamedParameter<std::string> SgIntegratorFname("SgIntegratorFname");
  NamedParameter<std::string> PeakyBgIntegratorFname("PeakyBgIntegratorFname");

  AmpsPdf amps(&sg, &eventList, integPrecision, integMethod, SgIntegratorFname);
  AmpsPdf ampsBg(&bg, &eventList, integPrecision, integMethod, PeakyBgIntegratorFname);

  DalitzSumPdf sgPlusBg(&eventList, sgFrac, amps, ampsBg);

  Neg2LL<IDalitzEvent> fcn(&sgPlusBg, &eventList, &fitMPS);

  NamedParameter<int> checkIntegration("CheckIntegration");
  if(checkIntegration){
    cout << " checking background norm" << endl;
    DalitzPdfNormChecker nc(&sgPlusBg, pdg);
    nc.checkNorm();
  }

  Minimiser mini(&fcn);
  mini.doFit();
  mini.printResultVsInput();
  
  DalitzHistoSet fitSgH = amps.histoSet() * sgFrac;
  fitSgH.save("plotsFromIntegrator_sg.root");
  DalitzHistoSet fitBgH = ampsBg.histoSet() * (1.0 - sgFrac);
  fitBgH.save("plotsFromIntegrator_bg.root");

  DalitzHistoSet fitH(fitSgH + fitBgH);

  datH.draw("dataPlots_");
  fitH.draw("fitPlots_");
  fitSgH.draw("fitPlots_");
  fitBgH.draw("fitPlots_");
  datH.drawWithFit(fitH, "datFit_");
  

  if(doScan){
    Double_t arglist[100] = {0};
    Int_t ierflg=0;
    
    TFile fscan("scanGamma.root", "RECREATE");
    for(int i=0; i<3; i++){
      arglist[0]=i; arglist[1]=100; arglist[2]=-100.; arglist[3]=100;
      mini.mnexcm("SCAN", arglist, 1, ierflg);
      //TCanvas *scanCan1 = new TCanvas("ScanGamma","ScanGamma");
      cout << "did scan, made canvas " << endl;
      // scanCan1->cd();
      TGraph *gr1 = (TGraph*)mini.GetPlot();
      if(0 == gr1){
	cout << " didn't get plot " << endl;
      }else{
	cout << "got plot!" << endl;
	//  gr1->Draw("al");
	//cout << "drew plot" << endl;
	fscan.cd();
	gr1->Write();
      }
    }
    fscan.Close();
  }

  AmpRatios rats(pdg);

  //  rats.getRatios(amps.getAmpSum());

  //  cout << " saving list: " << endl;
  //  eventList.save();

  cout << " ampFit done. Took " << (time(0) - startTime)/60 
       << " min. Returning 0." << endl;
  return 0;
}


int main(){

  return ampFit();

}
//
