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

#include "Mint/PlotSet.h"
#include "Mint/DalitzEvent.h"

#include "Mint/AmpRatios.h"

#include "Mint/IEventGenerator.h"
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

class AmpsPdf
  : public DalitzPdfBaseFastInteg
{
protected:
  IEventGenerator<IDalitzEvent>* _boxes;
public:
  double un_normalised_noPs(){
    complex<double> cval = _amps->getVal();
    return cval.real()*cval.real() + cval.imag()*cval.imag();
  }

  AmpsPdf(IDalitzEventList* events=0
	  , double precision=1.e-4
	  , std::string method="efficient"
	  , MinuitParameterSet* mps=0
	  )
    : DalitzPdfBaseFastInteg(events, 0, mps, precision)
    , _boxes(0)
 {
    cout << " AmpsPdf with integ method " << method << endl;
    bool nonFlat = "efficient" == method;
    bool BWBoxes = true;
    if(nonFlat){
      events->Start();
      events->Next();
      DalitzEventPattern pat(events->getEvent()->eventPattern());
      cout << " pat " << pat << endl;
      cout << "AmpsPdf uses nonFlat integration." << endl;
      if(BWBoxes){
	DalitzBWBoxSet* b = new DalitzBWBoxSet(_amps->makeBWBoxes());
	//	b->setUnWeightPs();
	_boxes = b;
      }else{
	_boxes = new DalitzBoxSet(_amps->makeBoxes(3));
      }
      this->setEventGenerator(_boxes);
    }else{
      cout << "AmpsPdf uses flat integration." << endl;
    }
  }

  FitAmpSum& getAmpSum(){ return *_amps;}

  ~AmpsPdf(){
    if(0 != _boxes) delete _boxes;
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
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
   
  NamedParameter<std::string> NtpName("NtpName"
				      , (std::string)"pull.root");
   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());

  bool noPhaseSpace = false;

  DalitzEventList dummyEventList;
  dummyEventList.generatePhaseSpaceEvents(1, pdg);  
  MinuitParameterSet fasSet;
  FitAmpSum fas(&dummyEventList, &fasSet);

  TNtupleD* testNtp=0;
  fasSet.fillNtp(testNtp);
  fasSet.fillNtp(testNtp);
  fasSet.fillNtp(testNtp);
  TFile ftest("test.root", "RECREATE");
  ftest.cd();
  testNtp->Write();
  ftest.Close();

  TNtupleD* ntp=0;

  for(int j_ex = 0; j_ex < Nexperiments; j_ex++){
    time_t t0 = time(0);
    // production:
    DalitzEventList eventList;
    DalitzBWBoxSet boxes(fas.makeBWBoxes(pdg));
    
    for(int i=0; i< Nevents; i++){
      int printEvery  = 1000;
      if(i < 1000) printEvery=100;
      bool printout = ( i%printEvery == 0 || i < 100);
      if(printout){
	cout << " made event number " << i << endl;
	cout << "   current list size " << eventList.size() << endl;
	double dt = difftime(time(0), t0);
	cout << "     this took " << dt << "s" << endl;
	if(dt > 0) cout << "     or " << eventList.size()/dt << "evt/s" << endl;
      }
      counted_ptr<DalitzEvent> evt(boxes.makeEventForOwner());
      if(printout) cout << "tried one event " << evt << endl;
      if(evt){
	bool largeWeight = (false && evt->getWeight() > 1.e-7) && (! noPhaseSpace);
	fas.setEvent(&(*evt));
	
	if(largeWeight){
	  cout << "rediculous ";
	}
	if(largeWeight || printout){
	  cout << " weight = " << evt->getWeight()
	       << " boxes.genValueNoPs " << boxes.genValueNoPs(*evt)
	       << " fas.RealVal() " << fas.RealVal()
	       << "." << endl;
	  cout << " largest Amp ";
	  fas.printLargestAmp();
	  cout << endl;
	}
	fas.resetEventRecord();
	
	if(i < 100 || i%1000==0 ){
	  cout << " adding event with weight " << evt->getWeight() << endl;
	}
	fas.resetEventRecord();
	eventList.Add(*evt);
      }
    }
    
    // fit:
    MinuitParameterSet fitMPS;
    AmpsPdf amps(&eventList, integPrecision, integMethod, &fitMPS);
    Neg2LL<IDalitzEvent> fcn(&amps, &eventList, &fitMPS);

    cout << "fitMPS pointer = " << &fitMPS << endl;
    
    Minimiser mini(&fcn);
    mini.doFit();
    mini.printResultVsInput();
    fitMPS.fillNtp(ntp);

    DalitzHistoSet fitH = amps.histoSet(); 
    fitH.save("plotsFromIntegrator.root");
    
    DalitzHistoSet datH = eventList.histoSet();
    datH.save("plotsFromEventList.root");

    datH.draw("dataPlots_" + anythingToString(j_ex) + "_");
    fitH.draw("fitPlots_"  + anythingToString(j_ex) + "_");

    datH.drawWithFit(fitH, "datFit_" + anythingToString(j_ex) + "_");

    TFile f(((std::string)NtpName).c_str(), "RECREATE");
    f.cd();
    ntp->Write();
    f.Close();

    TFile fs( (((std::string)NtpName) + "_save").c_str(), "RECREATE");
    fs.cd();
    ntp->Write();
    fs.Close();
    

    
    cout << " ampFit done. Took " << (time(0) - startTime)/60 
	 << " min. Returning 0." << endl;
  }

  return 0;
}


int main(){

  return ampFit();

}
//
