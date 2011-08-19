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
#include "Mint/DalitzPdfSaveInteg.h"


#include "Mint/Chi2Binning.h"

using namespace std;
using namespace MINT;


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
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);

  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  NamedParameter<int> doNormCheck("doNormCheck", 0);

  cout << " got event pattern: " << pdg << endl;


  DalitzEventList eventList;

  if(! generateNew){
    cout << "reading events from file " << InputFileName << endl;
    eventList.fromFile(InputFileName);
    cout << " I've got " << eventList.size() << " events." << endl;
  }

  if(generateNew){
    cout << "Generating " << Nevents << " signal events." << endl;
    SignalGenerator sg(pdg);
    sg.FillEventList(eventList, Nevents);
    eventList.save("K3pi.root");
  }
  
  DalitzHistoSet datH = eventList.histoSet();
  datH.save("plotsFromEventList.root");
  MinuitParameterSet fitMPS;
  DalitzPdfSaveInteg amps(&eventList, integPrecision
			  , (std::string) IntegratorInputFile
			  , (std::string) IntegratorEventFile
			  , "topUp"
			  , &fitMPS
			  );

  //amps.setIntegratorFileName("sgIntegrator");

  Neg2LL<IDalitzEvent> fcn(&amps, &eventList, &fitMPS);

  if((int) doNormCheck){
    DalitzPdfNormChecker nc(&amps, pdg);
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
  
  //AmpRatios rats(pdg);

  //rats.getRatios(amps.getAmpSum());

  //  cout << " saving list: " << endl;
  //  eventList.save();

  */
  cout << " ampFit done. Took " << (time(0) - startTime)/60. 
       << " min. Returning 0." << endl;
  return 0;
}


int main(){

  return ampFit();

}
//
