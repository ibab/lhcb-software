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

#include "Mint/NamedDecayTreeList.h"
#include "Mint/DecayTree.h"

#include "Mint/DalitzPdfSaveInteg.h"

using namespace std;
using namespace MINT;

bool addDecayTrees(){
  NamedDecayTreeList* list=NamedDecayTreeList::getMe();
  DecayTree* dk(0);
  
  //laurens decay NEW KSwide
  dk= new DecayTree(421);
  dk->addDgtr(321, -321);
  dk->addDgtr(888310)->addDgtr(211,-211);
  list->add(*dk, "BgSpinZero");
  //  list->add(*dk);
  delete dk;
  
  //laurens decay NEW KSwidePhi NB  no proper spin factor, temp 3 body spin factor inserteds
  dk= new DecayTree(421);
  dk->addDgtr(888310)->addDgtr(211,-211);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  //dk->addDgtr(888310)->addDgtr(211,-211);
  //dk->getVal().setL(0);
  //  list->add(AmpInitialiser(*dk, "BgSpinZero"));
  list->add(*dk, "BgSpinZero");
  delete dk;
  
  dk= new DecayTree(421);  //lauren Kswide a0(1450)
  dk->addDgtr( 10111)->addDgtr( 321, -321);
  dk->addDgtr(888310)->addDgtr(211,-211);
  //dk->getVal().setL(0);
  list->add(*dk, "BgSpinZero");
  delete dk;
  
  dk= new DecayTree(421);  //lauren  a0(1450)+->K+KsWide
  dk->addDgtr( -321, 10211)->addDgtr( 321, 888310)->addDgtr(211, -211);
  //dk->getVal().setL(0);
  list->add(*dk, "BgSpinZero");
  delete dk;
  
  dk= new DecayTree(421);  //lauren  a0(980)+->K+KsWide
  dk->addDgtr(  9000211)->addDgtr( 321, 888310)->addDgtr(211, -211);
  dk->addDgtr( -321);
  //dk->getVal().setL(0);
  list->add(*dk, "BgSpinZero");
  delete dk;
  
  dk= new DecayTree(421);  //lauren  a0(1260)+->K+KsWide
  dk->addDgtr(  20213)->addDgtr( 321, 888310)->addDgtr(211, -211);
  dk->addDgtr( -321);
  //dk->getVal().setL(0);
  list->add(*dk, "BgSpinZero");
  delete dk;
  
  dk= new DecayTree(421);  //lauren Kswide a0(980)
  dk->addDgtr( 9000111)->addDgtr( 321, -321);
  dk->addDgtr(888310)->addDgtr(211,-211);
  //dk->getVal().setL(0);
  list->add(*dk, "BgSpinZero");
  delete dk;
  
  dk= new DecayTree(421); //lauren Kswide f0(1370)
  dk->addDgtr( 30221)->addDgtr( 321, -321);
  dk->addDgtr(888310)->addDgtr(211,-211);
  //dk->getVal().setL(0);
  list->add(*dk, "BgSpinZero");
  delete dk;
  
  return true;
//=======
}

int ampFit(){
  time_t startTime = time(0);

  TRandom3 ranLux;
  NamedParameter<int> RandomSeed("RandomSeed", 0);
  ranLux.SetSeed((int)RandomSeed);
  gRandom = &ranLux;

  addDecayTrees();
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
  
  cout << " got event pattern: " << pdg << endl;
  DalitzEventList eventList;


  bool doIntegTest=false;
  if(doIntegTest){
    int Ncheck=1000000;
    FitAmpSum fas(pdg);
    DalitzBWBoxSet boxes(fas.makeBWBoxes());
    boxes.am_I_generating_what_I_think_I_am_generating(Ncheck);
    boxes.compareGenerationMethodsForFullPDF(Ncheck);
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
  datH.draw("dataPlots_");

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

  fitH.draw("fitPlots_");
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
