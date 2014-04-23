// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Neg2LL.h"
#include "Mint/Neg2LLSum.h"
#include "Mint/DalitzEventList.h"

#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/CLHEPSystemOfUnits.h"


#include "Mint/PdfBase.h"
#include "Mint/DalitzPdfBase.h"
#include "Mint/DalitzPdfBaseFastInteg.h"
#include "Mint/DalitzSumPdf.h"

#include "Mint/FitAmplitude.h"
#include "Mint/FitAmpSum.h"
#include "Mint/FitAmpIncoherentSum.h"

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

#include "KKpipiEfficiency.h"

using namespace std;
using namespace MINT;

class mumsMomentum : virtual public IReturnReal{
  TH1F* _h;
  TH1F* get_h(TFile* f, const std::string& hname){
    TH1F* h=0;
    if(0 == f) return 0;
    cout << " got file ptr: " << f << endl;
    if(0 != f){
      h = (TH1F*) f->Get(hname.c_str());
      if(0 != h){
	cout << "got momentum histogram " 
	     << hname << "." << endl;
      }else{
	cout << "ERROR didn't get momentum histogram " 
	     << hname << "!!!" << endl;
      }
    }
    return h;
  }
public:
  mumsMomentum(const string& fname="DmomGeV.root"
	       , const string& hname="Dmom"){
    TFile* f = new TFile(fname.c_str());
    _h = get_h(f, hname.c_str());
  }
  
  virtual double RealVal(){
    return _h->GetRandom()*GeV;
  }
};


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
  counted_ptr<mumsMomentum> _mp3;
public:
  double un_normalised_noPs(){
    if(0 == getEvent()) return 0;
    double ampSq =  _amps->RealVal();
    return ampSq;// * getEvent()->phaseSpace();
  }

  AmpsPdf( IFastAmplitudeIntegrable* amps//FitAmpSum* amps
	  , IDalitzEventList* events=0
	  , double precision=1.e-4
	  , std::string method="efficient"
	  , std::string int_fname =  "SignalIntegrationEvents.root"
	  )
    : DalitzPdfBaseFastInteg(events, 0
			     , amps
			     , precision)
    , _localRnd(0)
    , _sgGen(0)
    , _fileGen(0)
    , _chosenGen(0)
    , _integratorSource("IntegratorSource", (std::string) "new", (char*) 0)
    , _integratorFileName(int_fname)
    , _mp3(new mumsMomentum())
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
	_fileGen->setMothers3Momentum(_mp3);
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

  NamedParameter<string> SgBoxInputFileName("SgBoxInputFileName"
					    , (std::string) "");
  bool generateNewSgBox = (std::string) SgBoxInputFileName == "";

  NamedParameter<string> BgBoxInputFileName("BgBoxInputFileName"
					    , (std::string) "");
  bool generateNewBgBox = (std::string) BgBoxInputFileName == "";

  NamedParameter<int>  NSgBoxEvents("NSgBoxEvents", 10000);
  NamedParameter<int>  NBgBoxEvents("NBgBoxEvents", 10000);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pdg(EventPattern.getVector());
  NamedParameter<int> withEfficiency("WithEfficiency", 1);
  
  cout << " got event pattern: " << pdg << endl;
  DalitzEventList eventListSgBox; // signal region, contains sg + bg
  DalitzEventList eventListBgBox; // backg. region, contains bg only;

  cout << "With Efficiency? " << (withEfficiency ? "y" : "n") << endl;
  if(! generateNewSgBox){
    cout << "reading signal-box events from file "
	 << SgBoxInputFileName << endl;
    eventListSgBox.fromFile(SgBoxInputFileName);
    cout << " I've got " << eventListSgBox.size() << " events." << endl;
  }
  if(! generateNewBgBox){
    cout << "reading background-box events from file " 
	 << BgBoxInputFileName << endl;
    eventListBgBox.fromFile(BgBoxInputFileName);
    cout << " I've got " << eventListBgBox.size() << " events." << endl;
  }

  MinuitParameterSet fitMPS;
  FitParameter sgFrac("SignalFraction");
  FitAmpSum sgFas(pdg, &fitMPS);

  if(withEfficiency)sgFas.setEfficiency(counted_ptr<IGetDalitzEvent>(new KKpipiEfficiency));

  FitAmpIncoherentSum bgFas(pdg, &fitMPS);
  if(withEfficiency)bgFas.setEfficiency(counted_ptr<IGetDalitzEvent>(new KKpipiEfficiency));

  if(generateNewSgBox){
    SignalGenerator sggen(&sgFas);
    sggen.FillEventList(eventListSgBox, NSgBoxEvents * sgFrac);
    SignalGenerator bggen(&bgFas);
    bggen.FillEventList(eventListSgBox, NSgBoxEvents*(1.0-sgFrac));
  }
  if(generateNewBgBox){
    SignalGenerator bggen(&bgFas);
    bggen.FillEventList(eventListBgBox, NBgBoxEvents);
  }
  
  DalitzHistoSet datSgBoxH = eventListSgBox.histoSet();
  datSgBoxH.save("plotsFromEventListSgBox.root");
  DalitzHistoSet datBgBoxH = eventListBgBox.histoSet();
  datBgBoxH.save("plotsFromEventListBgBox.root");

  NamedParameter<std::string> SgIntegratorFname("SgIntegratorFname");
  NamedParameter<std::string> PeakyBgIntegratorFname("PeakyBgIntegratorFname");

  AmpsPdf pdfSg(&sgFas, &eventListSgBox, integPrecision, integMethod
	       , SgIntegratorFname);

  AmpsPdf pdfBg(&bgFas, &eventListSgBox, integPrecision, integMethod
		 , PeakyBgIntegratorFname);

  DalitzSumPdf sgPlusBg(&eventListSgBox, sgFrac, pdfSg, pdfBg);

  Neg2LL<IDalitzEvent> sgBoxLL(&sgPlusBg, &eventListSgBox, &fitMPS);
  Neg2LL<IDalitzEvent> bgBoxLL(&pdfBg, &eventListBgBox, &fitMPS);
  Neg2LLSum fcn(&sgBoxLL, &bgBoxLL, &fitMPS);

  NamedParameter<int> checkIntegration("CheckIntegration");
  if(checkIntegration){
    cout << " checking background norm" << endl;
    DalitzPdfNormChecker nc(&sgPlusBg, pdg);
    nc.checkNorm();
  }

  Minimiser mini(&fcn);
  mini.doFit();
  mini.printResultVsInput();
  
  DalitzHistoSet fitSgH = pdfSg.histoSet() * sgFrac;
  fitSgH.save("plotsFromIntegrator_sg.root");
  DalitzHistoSet fitBgH = pdfBg.histoSet() * (1.0 - sgFrac);
  fitBgH.save("plotsFromIntegrator_bg.root");

  pdfBg.setEventRecord(&eventListBgBox);
  // (event record just to tell it to normalise the histograms
  //  to the number of events in the Bg box)
  DalitzHistoSet fitBgBgH = pdfBg.histoSet();
  pdfBg.resetEventRecord();
  fitBgBgH.save("plotsFromIntegrator_bgbg.root");

  DalitzHistoSet fitSgBoxH(fitSgH + fitBgH);

  datSgBoxH.draw("dataPlots_");
  fitSgH.draw("fitPlotsSg_");
  fitBgH.draw("fitPlotsBgSgBox_");
  fitSgBoxH.draw("fitPlotsSgBox_");
  datSgBoxH.drawWithFit(fitSgBoxH, "datFitSgBox_");
  datBgBoxH.drawWithFit(fitBgBgH, "datFitBgBox_");

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

  //  rats.getRatios(pdfSg.getAmpSum());

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
