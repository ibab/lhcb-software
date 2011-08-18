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

#include "Mint/Mojito/DalitzEvents/DalitzEvent.h"

#include "Mint/Mojito/Ratios/AmpRatios.h"

#include "Mint/Mint/Events/IEventGenerator.h"
#include "Mint/Mojito/BreitWignerMC/DalitzBWBoxSet.h"
#include "Mint/Mojito/BetterMC/DalitzBoxSet.h"

#include "Mint/Mojito/DalitzEventGeneration/SignalGenerator.h"
#include "Mint/Mojito/DalitzEventGeneration/FromFileGenerator.h"

#include "Mint/Mint/Utils/cexp.h"


#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"

#include "TRandom2.h"
#include "TRandom3.h"
#include <ctime>

#include <iostream>

#include "Mint/Mojito/DalitzFits/DalitzPdfNormChecker.h"

#include "Mint/Mojito/DecayTrees/NamedDecayTreeList.h"
#include "Mint/Mojito/DecayTrees/DecayTree.h"

#include "Mint/Mojito/DalitzFits/DalitzPdfSaveInteg.h"

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

     
  NamedParameter<int> EventPattern("Event Pattern", 421, 310, 211, -211, 111);
  DalitzEventPattern pdg(EventPattern.getVector());
  
  cout << " got event pattern: " << pdg << endl;
  DalitzEventList eventList;

  cout << "The omega" << endl;
  const ParticlePropertiesList *pList = ParticlePropertiesList::getMe();
  pList->get(223)->print();
  cout << "as it should be?" << endl;

  bool doWeightTest=false;
  if(doWeightTest){
    SignalGenerator sgTest(pdg);
    sgTest.setWeighted();
    DalitzEventList testList;
    sgTest.FillEventList(testList, 100000);
    
    DalitzHistoSet testEvents  = testList.histoSet();
    DalitzHistoSet testWeights = testList.weightedHistoSet();
    testEvents.draw("testEvtsPlots_");
    testWeights.draw("testWeightsPlots_");
    testWeights.drawWithFit(testEvents, "eventsAndWeightsPlots_");
    testList.save("testList.root");
    testWeights /= testEvents;
    testWeights.draw("weightEventRatioPlots_");
    
    //  testEvents.drawWithFit(testWeights, "weightsAndEventsPlots_");
    
    cout << "done all test plots" << endl;
  }

  if(! generateNew){
    cout << "reading events from file " << InputFileName << endl;
    eventList.fromFile(InputFileName);
    cout << " I've got " << eventList.size() << " events." << endl;
  }

  if(generateNew){
    SignalGenerator sg(pdg);
    sg.FillEventList(eventList, Nevents);
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

  Neg2LL<IDalitzEvent> fcn(&amps, &eventList, &fitMPS);

  /*
  DalitzPdfNormChecker nc(&amps, pdg);
  nc.checkNorm();
  */

  Minimiser mini(&fcn);
  mini.doFit();
  mini.printResultVsInput();
  
  DalitzHistoSet fitH = amps.histoSet(); 
  fitH.save("plotsFromIntegrator.root");
  amps.saveEachAmpsHistograms("singleAmpHistos");

  datH.draw("dataPlots_");
  fitH.draw("fitPlots_");
  datH.drawWithFit(fitH, "datFit_");
  

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
