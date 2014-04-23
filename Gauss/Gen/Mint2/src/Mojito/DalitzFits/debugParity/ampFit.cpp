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
  NamedParameter<int>  doFit("doFit", 1);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);

     
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());
  
  cout << " got event pattern: " << pdg << endl;
  DalitzEventList eventList;
  DalitzEventList eventListCPcon;
  DalitzEventList eventListCPcon2;

  FitAmpSum DtoF(pdg);
  FitAmpSum DbartoFbar(pdg.makeCPConjugate());
  counted_ptr<FitAmpSum> DbartoFbar2(DtoF.GetCPConjugateSameFitParameters());

  SignalGenerator sg(&DtoF, gRandom);
  SignalGenerator sgCPcon(&DbartoFbar, gRandom);

  SignalGenerator sgCPcon2(DbartoFbar2.get(), gRandom);
  
  int N=10;
  sg.setWeighted();
  sgCPcon.setWeighted();
  
  gRandom->SetSeed(1);
  sg.FillEventList(eventList, N);
  gRandom->SetSeed(1);
  sgCPcon.FillEventList(eventListCPcon, N);
  gRandom->SetSeed(1);
  sgCPcon.FillEventList(eventListCPcon2, N);
  
  for(int i=0; i < N; i++){
    cout << "D->f        " << eventList[i]  << endl;
    cout << "Dbar->fbar  " << eventListCPcon[i] << endl;
    cout << "Dbar->fbar2 " << eventListCPcon2[i] << endl;
    DtoF.setEvent(&(eventList[i]));
    DbartoFbar.setEvent(&(eventListCPcon[i]));
    DbartoFbar2->setEvent(&(eventListCPcon2[i]));


    cout << " A     = " << DtoF.getVal()
	 << " = " << abs(DtoF.getVal())
	 << " exp( i " << arg(DtoF.getVal()) << " )"
	 << "\n Abar  = " << DbartoFbar.getVal()
	 << " = " << abs(DbartoFbar.getVal())
	 << " exp( i " << arg(DbartoFbar.getVal()) << " )"
	 << "\n Abar2 = " << DbartoFbar2->getVal()
	 << " = " << abs(DbartoFbar2->getVal())
	 << " exp( i " << arg(DbartoFbar2->getVal()) << " )"
	 << endl;
    cout << "----------------------------------------------" << endl;
    
    cout << "AMPLITUDE LISTS"
	 << "\n DtoF: **************** \n"
	 << endl;
    DtoF.printNonZeroWithValue(cout);
    cout << "\n DtoFbar: **************** \n"
	 << endl;
    DbartoFbar.printNonZeroWithValue(cout);
    cout << "\n DtoFbar2: **************** \n"
	 << endl;
    DbartoFbar2->printNonZeroWithValue(cout);
    cout << "\n *******************************" << endl;
  }

  return 0;
}

int main(){

  return ampFit();

}
//
