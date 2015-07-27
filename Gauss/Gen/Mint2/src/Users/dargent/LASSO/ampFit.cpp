// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk), Jack Benton (Jack.B.Benton@bristol.ac.uk)
// status:  Fri 28 Jun 2013 11:21:01 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Neg2LL.h"
#include "Mint/Neg2LLSum.h"
#include "Mint/DalitzEventList.h"
#include "Mint/NamedDecayTreeList.h"
#include "Mint/DecayTree.h"
#include "Mint/DiskResidentEventList.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/PdfBase.h"
#include "Mint/DalitzPdfBase.h"
#include "Mint/DalitzPdfBaseFastInteg.h"
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
#include "Mint/DalitzSumPdf.h"
#include "Mint/cexp.h"
#include "Mint/DalitzPdfNormChecker.h"
#include "Mint/IFastAmplitudeIntegrable.h"
#include "Mint/DalitzPdfSaveInteg.h"
#include "Mint/Chi2Binning.h"
#include "Mint/FitAmpIncoherentSum.h"
#include "Mint/FitAmpList.h"
#include "Mint/DalitzPdfBaseMCInteg.h"
#include "Mint/LASSO.h"
#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TNtupleD.h"
#include "TTree.h"
#include "TFile.h"
#include <TStyle.h>
#include "TRandom2.h"
#include "TRandom3.h"

#include <ctime>
#include <iostream>
#include <fstream>


using namespace std;
using namespace MINT;

class AmpsPdfFast
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
    double un_normalised_noPs(IDalitzEvent& evt){
        double ampSq =  _amps->RealVal(evt);
        return ampSq;// * getEvent()->phaseSpace();
    }
    
    AmpsPdfFast(DalitzEventPattern& pat, IFastAmplitudeIntegrable* amps
            , double precision=1.e-2
            , std::string method="efficient"
            , std::string fname =  "SignalIntegrationEvents.root", bool genMoreEvents = false
            )
    : DalitzPdfBaseFastInteg(pat, 0, amps, precision)
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
                _sgGen =  new SignalGenerator(pat, _amps);
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
                _sgGen =  new SignalGenerator( pat, _amps, _localRnd);
                _sgGen->setWeighted();
                _sgGen->dontSaveEvents();// saving events is done by FromFileGenerator
                if(genMoreEvents) _fileGen   = new FromFileGenerator( _integratorFileName, _sgGen);
                else{
                    _fileGen = new FromFileGenerator(_integratorFileName, 0);
                    cout << "not going to generate any more events" << endl;
                    cout << "using file " << (std::string) _integratorFileName << endl;
                }
                _chosenGen = _fileGen;
            }
            this->setEventGenerator(_chosenGen);
        }else{
            cout << "AmpsPdf uses flat integration." << endl;
        }
    }
    
    IFastAmplitudeIntegrable* getAmpSum(){ return _amps;}
    
    ~AmpsPdfFast(){
        if(0 != _fileGen)  delete _fileGen;
        if(0 != _sgGen)    delete _sgGen;
        if(0 != _localRnd) delete _localRnd;
    }
};

class AmpsPdf
  : public DalitzPdfBaseMCInteg
{
protected:
  TRandom* _localRnd;
  SignalGenerator* _sgGen;
  FromFileGenerator* _fileGen;
  IEventGenerator<IDalitzEvent>* _chosenGen;
  NamedParameter<std::string> _integratorSource;
  std::string _integratorFileName;
  IFastAmplitudeIntegrable* _amps;
public:
  double un_normalised_noPs(IDalitzEvent& evt){
    return _amps->RealVal(evt);
  }
  
  AmpsPdf(DalitzEventPattern& pat, IFastAmplitudeIntegrable* amps, double precision=1.e-2, std::string method="efficient"
          , std::string fname =  "SignalIntegrationEvents.root", bool genMoreEvents = false)
    : DalitzPdfBaseMCInteg(pat,amps,precision)
    , _localRnd(0)
    , _sgGen(0)
    , _fileGen(0)
    , _chosenGen(0)
    , _integratorSource("IntegratorSource", (std::string) "new", (char*) 0)
    , _integratorFileName(fname)
    , _amps(amps)
  {
    cout << " AmpsPdf with integ method " << method << endl;
    bool nonFlat = "efficient" == method;

    bool generateNew = ((string)_integratorSource == (string)"new");
    if(nonFlat){
      cout << "AmpsPdf uses nonFlat integration." << endl;
      if(generateNew){
          _sgGen =  new SignalGenerator(pat, _amps);
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
          _sgGen =  new SignalGenerator( pat, _amps, _localRnd);
          _sgGen->setWeighted();
          _sgGen->dontSaveEvents();// saving events is done by FromFileGenerator
          if(genMoreEvents) _fileGen   = new FromFileGenerator( _integratorFileName, _sgGen);
          else{
              _fileGen = new FromFileGenerator(_integratorFileName);
              cout << "not going to generate any more events" << endl;
              cout << "using file " << (std::string) _integratorFileName << endl;
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
  
  NamedParameter<string> InputFileName("InputFileName", (std::string) "");
  NamedParameter<string> InputTreeName("InputTreeName", (std::string) "DalitzEventList");
  std::string inputFile = InputFileName;
  std::string inputTreeName = InputTreeName;
  bool generateNew = (std::string) InputFileName == "";
  std::cout << "InputFileName: " << InputFileName << std::endl;

  NamedParameter<string> IntegratorEventFile("IntegratorEventFile"
					     , (std::string) "SignalIntegrationEvents.root"
					     , (char*) 0);
  NamedParameter<string> IntegratorInputFile("IntegratorInputFile"
					     , (std::string) "sgIntegrator"
					     , (char*) 0);
  NamedParameter<string> OutputRootFile("OutputRootFile"
					     , (std::string) "OutputRootFile.root"
					     , (char*) 0);

  NamedParameter<string> IntegratorOutputFile("IntegratorOutputFile"
					     , (std::string) "sgIntegrator"
					     , (char*) 0);

  NamedParameter<int>  Nevents("Nevents", 1000);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-2);
  NamedParameter<std::string> integMethod("IntegMethod", (std::string)"efficient");
  NamedParameter<int> fitLineshapeParameters("FitLineshapeParameters", 0);

  NamedParameter<int>  useLASSO("useLASSO", 0);
  NamedParameter<double>  lambda("lambda", 1.);
  NamedParameter<double>  threshold("threshold", 0.001);

  NamedParameter<string> OutputDir("OutputDir", (std::string) "", (char*) 0);
  NamedParameter<int>  useIntegrator("UseIntegrator", 0);
  NamedParameter<int> integratorFileSave("IntegratorFileSave", 0);  
    
  NamedParameter<int> EventPattern("Event Pattern", 521, 321, 211, -211, 443);
  DalitzEventPattern pat(EventPattern.getVector());
  cout << " got event pattern: " << pat << endl;

  FitAmpSum fas(pat);
    fas.print();
    
    //return 0; 
    
  DalitzEventList eventList;
    
  if(generateNew){
    	SignalGenerator sg(pat,&fas);
      //sg.setWeighted();
    	cout << "Generating " << Nevents << " MC events." << endl;
    	sg.FillEventList(eventList, Nevents);
    	eventList.saveAsNtuple(OutputRootFile);
  }
    
  if(!generateNew){
		TFile *_InputFile =  TFile::Open(inputFile.c_str());
		TTree* in_tree;
		in_tree=dynamic_cast<TTree*>(_InputFile->Get(inputTreeName.c_str()));
		cout << "reading events from file " << inputFile << endl;
		eventList.fromNtuple(in_tree,1);
		cout << " I've got " << eventList.size() << " events." << endl;
		_InputFile->Close();
  }
    
    //for (int i=0; i<eventList.size(); i++) {
    //    fas.getVal(eventList[i]);
    //}  
    
    DalitzHistoSet datH = eventList.weightedHistoSet();
    datH.draw("data_","eps");
    //return 0;

  if(fitLineshapeParameters){
        AmpsPdf amps(pat, &fas, integPrecision,integMethod, (std::string) IntegratorEventFile);
        Neg2LL fcn(amps, eventList);
        Neg2LLSum neg2LLSum(&fcn);
        neg2LLSum.addConstraints(); 
        Minimiser mini(&neg2LLSum);
        mini.doFit();
        mini.printResultVsInput();
        DalitzHistoSet fitH = amps.histoSet();
        datH.drawWithFitNorm(fitH, ((string)OutputDir+(string)"datFit_").c_str(),"eps");
  }
  
  else { 
      AmpsPdfFast amps(pat, &fas, integPrecision,integMethod, (std::string) IntegratorEventFile);
      if(useIntegrator)amps.setIntegratorFileName(((string)OutputDir+(string)IntegratorInputFile).c_str());
      
      Neg2LL neg2ll(amps, eventList);
      LASSO lasso(&amps,lambda);
      Neg2LLSum fcn(&neg2ll,&lasso);
      
      Minimiser mini;
      if(useLASSO)mini.attachFunction(&fcn);
      else mini.attachFunction(&neg2ll);
      mini.doSeekFit();
      mini.CallSimplex();
      mini.CallMigrad();
      mini.CallImprove();
      mini.printResultVsInput();
      amps.doFinalStats(&mini);
      if(integratorFileSave)amps.saveIntegrator(((string)OutputDir+(string)IntegratorOutputFile).c_str());

      //cout << "Saving each amplitude in a histogram." << endl;
      //amps.saveEachAmpsHistograms("EachAmpHistos");
      DalitzHistoSet fitH = amps.histoSet();
      DalitzHistoSet interferenceH = amps.interferenceHistoSet();
      datH.drawWithFitNorm(fitH, ((string)OutputDir+(string)"datFit_").c_str(),"eps");
      std::vector<DalitzHistoSet> EachAmpsHistos = amps.GetEachAmpsHistograms();
      datH.drawWithFitAndEachAmps(datH, fitH, EachAmpsHistos, ((string)OutputDir+(string)"WithAmps").c_str(), "eps");
      //datH.drawWithFitAndEachAmpsAndInterference(datH, fitH, interferenceH, EachAmpsHistos, ((string)OutputDir+(string)"WithAmpsAndInterference").c_str(), "eps");

      if(useLASSO){
          cout << "Number of fractions larger than " << threshold << " = " << lasso.numberOfFitFractionsLargerThanThreshold(threshold) << endl;
          cout << "AIC = " << neg2ll.getVal() + 2. * lasso.numberOfFitFractionsLargerThanThreshold(threshold) << endl;
          cout << "BIC = " << neg2ll.getVal() + lasso.numberOfFitFractionsLargerThanThreshold(threshold) * log(eventList.size()) << endl;
      }

  }
  cout << "==============================================" << endl;
  cout << " Done. " << " Total time since start " << (time(0) - startTime)/60.0 << " min." << endl;
  cout << "==============================================" << endl;

  return 0;
}


int main(){
  gStyle->SetOptStat(0);
  gStyle->SetTitleXSize(0.07);
  gStyle->SetTitleYSize(0.05);
  gStyle->SetTitleFont(42,"X");
  gStyle->SetTitleFont(42,"Y");
  gStyle->SetLabelFont(42,"X");
  gStyle->SetLabelFont(42,"Y");
  gStyle->SetLabelOffset(0.01,"X");
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  return ampFit();
}
//
