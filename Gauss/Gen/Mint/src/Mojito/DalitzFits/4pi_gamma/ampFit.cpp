// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Neg2LL.h"
#include "Mint/Neg2LLSum.h"
#include "Mint/DalitzEventList.h"
#include "TH2D.h"
#include "TNtupleD.h"

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

#include "Mint/IReturnComplex.h"
#include <complex>

using namespace std;
using namespace MINT;


class AmpRatioB : virtual public IReturnComplex{
  FitParameter& _rB;
  FitParameter& _delta;
  FitParameter& _gamma;
  int _Bcharge;
  double _DCSFactor;
public:
  AmpRatioB(FitParameter& rB, FitParameter& delta, FitParameter& gamma
	    , int Bcharge, double DCSFactor=1)
    : _rB(rB), _delta(delta), _gamma(gamma), _Bcharge(Bcharge), _DCSFactor(DCSFactor) {}
  
  complex<double> ComplexVal(){
    //return (double)_rB;
    std::complex<double> result = 
      polar((double) _rB, (double)(_delta + _gamma * (double) _Bcharge));
    //result = _rB;
    //if(_Bcharge < 0){
    //  result = 1./result;
    //}
    //cout << "result " << result << endl;
    return result/_DCSFactor;
  }
};



counted_ptr<FitAmpSum> makeGammaFitAmpSum(DalitzEventPattern pat
					  , const counted_ptr<AmpRatioB>& ar){
  counted_ptr<FitAmpSum> DSum( new FitAmpSum(pat) );
  //return DSum;
  counted_ptr<FitAmpSum> D0barSum(0);
  if(pat.selfConjugateFS()){
    cout << "makeGammaFitAmpSum: self-conjugate final state (pat = " << pat << ")" << endl;
    D0barSum = (counted_ptr<FitAmpSum>) DSum->GetCPConjugateSameFitParameters();
  }else{
    cout << "makeGammaFitAmpSum: not self-con. final state (pat = " << pat << ")" << endl;
    DalitzEventPattern patBar(pat);
    patBar[0].antiThis();
    D0barSum = counted_ptr<FitAmpSum>(new FitAmpSum(patBar));
    //return D0barSum;
  }
  DSum->multiply( (counted_ptr<IReturnComplex>) ar);
  DSum->add(*D0barSum);
  return DSum;
}

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

  AmpsPdf( const counted_ptr<FitAmpSum>& amps
	   , IDalitzEventList* events=0
	   , double precision=1.e-4
	   , const std::string& method="efficient"
	   , const std::string& integFileName="IntegFile.root"
	   )
    : DalitzPdfBaseFastInteg(events, 0, amps.get(), precision)
    , _localRnd(0)
    , _sgGen(0)
    , _fileGen(0)
    , _chosenGen(0)
    , _integratorSource("IntegratorSource", (std::string) "new", (char*) 0)
    , _integratorFileName(integFileName)
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

  NamedParameter<string> BpSignalFileName("BpSignalFileName", (std::string) "BpSg.root");
  NamedParameter<string> BmSignalFileName("BmSignalFileName", (std::string) "BmSg.root");

  NamedParameter<string> BpIntegFileName("BpIntegFileName", (std::string) "BpInteg.root");
  NamedParameter<string> BmIntegFileName("BmIntegFileName", (std::string) "BmInteg.root");

  NamedParameter<int>  Nevents("Nevents", 10000);
  NamedParameter<int>  WeightTestEvents("WeightTestEvents", 100000);
  NamedParameter<int>  doScan("doScan", 0);
  NamedParameter<double> integPrecision("IntegPrecision", 1.e-4);
  NamedParameter<std::string> integMethod("IntegMethod"
					  , (std::string)"efficient");
   
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  DalitzEventPattern pat(EventPattern.getVector());

  NamedParameter<double> DCSFactor("DCSFactor", 0.225*0.225);


  // pull-specific things
  NamedParameter<std::string> NtpName("NtpName"
				      , (std::string)"pull.root");

  NamedParameter<int>  Nexperiments("Nexperiments", 100);

  TFile* paraFile = new TFile("pull.root", "RECREATE");
  TNtupleD* ntp=0;
  
  // fit parameters

  for(int j_ex = 0; j_ex < Nexperiments; j_ex++){
    
    FitParameter rB("rB");
    FitParameter delta("delta");
    FitParameter gamma("gamma");
    
    
    counted_ptr<AmpRatioB> xiBplus(new AmpRatioB(rB, delta, gamma
						 , 1, DCSFactor));
    counted_ptr<AmpRatioB> xiBminus(new AmpRatioB(rB, delta, gamma
						  , -1, DCSFactor));
    
    counted_ptr<FitAmpSum> BpAmps = makeGammaFitAmpSum(pat, xiBplus);
    counted_ptr<FitAmpSum> BmAmps = makeGammaFitAmpSum(pat, xiBminus);
    
    
    cout << "ex " << j_ex << ") " << " got event pattern: " << pat << endl;
    DalitzEventList BpEventList, BmEventList;

    bool doWeightTest= true && (0 == j_ex);
    if(doWeightTest){
      SignalGenerator sgP_generateWeighted(BpAmps.get());
      sgP_generateWeighted.setWeighted();
      DalitzEventList sgPWeighted;
      sgP_generateWeighted.FillEventList(sgPWeighted, WeightTestEvents);
      DalitzHistoSet datH         = sgPWeighted.histoSet();
      DalitzHistoSet datHWeighted = sgPWeighted.weightedHistoSet();
      sgPWeighted.save("sgPWeighted.root");
      datH.save("plotsFromEventList.root");
      datHWeighted.save("plotsFromEventListWeighted.root");
      
      datHWeighted.drawWithFitNorm(datH, "plots_");
      
      DalitzHistoSet ratio(datHWeighted/datH);
      ratio.draw("ratioHistos");
    }

    
    //FitAmpSum justBp(pat);
    //SignalGenerator sgP(&justBp);

    cout << "ex " << j_ex << ") " << "generating " << (int) Nevents << " B+ events" << endl;
    SignalGenerator sgP(BpAmps.get());
    sgP.FillEventList(BpEventList, Nevents);
    cout << "ex " << j_ex << ") " << "done Bp." << endl;
    cout << "ex " << j_ex << ") " << "Now I got " << (int) BpEventList.size() << " B+ events" << endl;
    BpEventList.save("BplusEvents.root");

    cout << "ex " << j_ex << ") " << "generating " << (int) Nevents << " B- events:" << endl;
    SignalGenerator sgM(BmAmps.get());
    sgM.FillEventList(BmEventList, Nevents);
    BmEventList.save("BminusEvents.root");

    cout << "ex " << j_ex << ") " << "done B-" << endl;
    cout << "ex " << j_ex << ") " << "Now I got " << (int) BmEventList.size() << " B- events" << endl;

    DalitzHistoSet datHP = BpEventList.histoSet();
    datHP.save("plotsFromBpEventList.root");
    //DalitzHistoSet datHM = BmEventList.histoSet();
    //datHM.save("plotsFromBmEventList.root");
    
    AmpsPdf BpPdf(BpAmps, &BpEventList, integPrecision, integMethod
		  , BpIntegFileName);
    BpPdf.setIntegratorFileName("spIntegrator_2");
    
    Neg2LL<IDalitzEvent> fcnP(&BpPdf, &BpEventList);
    
    AmpsPdf BmPdf(BmAmps, &BmEventList, integPrecision, integMethod
		  , BmIntegFileName);
    BmPdf.setIntegratorFileName("smIntegrator_2");
    
    Neg2LL<IDalitzEvent> fcnM(&BmPdf, &BmEventList);
    
    Neg2LLSum fcn(&fcnP, &fcnM);
    
    /*
      DalitzPdfNormChecker nc(&BpPdf, pat);
      nc.checkNorm();
    */
    
    cout << "ex " << j_ex << ") " << "starting fit" << endl;
    Minimiser mini(&fcn);
    //Minimiser mini(&fcnP);
    //mini.doMinosFit();
    mini.doFit();
    mini.printResultVsInput();
    BpPdf.doFinalStats();
    BpPdf.saveEachAmpsHistograms("Bp");
    BmPdf.doFinalStats();
    BmPdf.saveEachAmpsHistograms("Bm");

    cout << "ex " << j_ex << ") " << "done fit" << endl;

    MinuitParameterSet::getDefaultSet()->fillNtp(paraFile, ntp);    
    ntp->AutoSave();

    BpPdf.saveIntegrator("spIntegrator_2");
    BmPdf.saveIntegrator("smIntegrator_2");
    
    bool do2DScan= false; (0 == j_ex);
    
    if(do2DScan){
      cout << "ex " << j_ex << ") " << "now doing 2D scan:" << endl;
      int scanBins=100;
      double scanMin=0, scanMax=2*pi;
      double nSigmaZoom = 8;
      double scanMinGammaZoom=min(gamma.meanInit(), gamma.mean()) - nSigmaZoom*gamma.err();
      double scanMaxGammaZoom=max(gamma.meanInit(), gamma.mean()) + nSigmaZoom*gamma.err();
      double scanMinDeltaZoom=min(delta.meanInit(), delta.mean()) - nSigmaZoom*delta.err();
      double scanMaxDeltaZoom=max(delta.meanInit(), delta.mean()) + nSigmaZoom*delta.err();
      double gammaZoomRange = scanMaxGammaZoom - scanMinGammaZoom;
      double deltaZoomRange = scanMaxDeltaZoom - scanMinDeltaZoom;

      TFile* scanFile = new TFile(("scan_" + anythingToString(j_ex) + ".root").c_str(), "RECREATE");
      TH2D* scanHisto = new TH2D(("scan_" + anythingToString(j_ex)).c_str(), ("scan_" + anythingToString(j_ex)).c_str(), scanBins, 
				 scanMin, scanMax, scanBins, scanMin, scanMax);
      TH2D* scanHistoP = new TH2D(("scanP_" + anythingToString(j_ex)).c_str(), ("scanP_" + anythingToString(j_ex)).c_str(), scanBins, 
				 scanMin, scanMax, scanBins, scanMin, scanMax);
      TH2D* scanHistoM = new TH2D(("scanM_" + anythingToString(j_ex)).c_str(), ("scanM_" + anythingToString(j_ex)).c_str(), scanBins, 
				 scanMin, scanMax, scanBins, scanMin, scanMax);
      TH2D* scanZoomHisto = new TH2D(("scanZoom_" + anythingToString(j_ex)).c_str(), ("scanZoom_" + anythingToString(j_ex)).c_str(), scanBins, 
				 scanMinGammaZoom, scanMaxGammaZoom, scanBins, scanMinDeltaZoom, scanMaxDeltaZoom);
      
      double scanMinLL=-9999;
      double scanMinLLP=-9999;
      double scanMinLLM=-9999;
      double scanMinLLZ=-9999;

      for(int i=0; i < scanBins; i++){
	double gamma_value = ((double)i+0.5)*2*pi/((double)scanBins);
	gamma.setCurrentFitVal(gamma_value);
	for(int j=0; j < scanBins; j++){
	  double delta_value = ((double)j+0.5)*2*pi/((double)scanBins);
	  delta.setCurrentFitVal(delta_value);

	  double v = fcn.getNewVal();  
	  if( (i==0 && j==0) || v < scanMinLL) scanMinLL=v;
	  scanHisto->Fill(gamma, delta, v);

	  double vP = fcnP.getNewVal();  
	  if( (i==0 && j==0) || vP < scanMinLLP) scanMinLLP=vP;
	  scanHistoP->Fill(gamma, delta, vP);

	  double vM = fcnM.getNewVal();  
	  if( (i==0 && j==0) || vM < scanMinLLM) scanMinLLM=vM;
	  scanHistoM->Fill(gamma, delta, vM);
	}
      }
      for(int i=0; i < scanBins; i++){
	double gamma_value = scanMinGammaZoom + ((double)i+0.5) * gammaZoomRange/((double)scanBins);
	gamma.setCurrentFitVal(gamma_value);
	for(int j=0; j < scanBins; j++){
	  double delta_value = scanMinDeltaZoom + ((double)j+0.5) * deltaZoomRange/((double)scanBins);
	  delta.setCurrentFitVal(delta_value);
	  double v = fcn.getNewVal();
	  
	  if( (i==0 && j==0) || v < scanMinLLZ) scanMinLLZ=v;
	  
	  scanZoomHisto->Fill(gamma, delta, v);
	}
      }

      for(int i=0; i < scanBins; i++){
	double gamma_value = ((double)i+0.5)*2*pi/((double)scanBins);
	for(int j=0; j < scanBins; j++){
	  double delta_value = ((double)j+0.5)*2*pi/((double)scanBins);
	  scanHisto->Fill(gamma_value, delta_value, -scanMinLL);
	  scanHistoP->Fill(gamma_value, delta_value, -scanMinLLP);
	  scanHistoM->Fill(gamma_value, delta_value, -scanMinLLM);
	}
      }
      for(int i=0; i < scanBins; i++){
	double gamma_value = scanMinGammaZoom + ((double)i+0.5) * gammaZoomRange/((double)scanBins);
	for(int j=0; j < scanBins; j++){
	  double delta_value = scanMinDeltaZoom + ((double)j+0.5) * deltaZoomRange/((double)scanBins);
	  scanZoomHisto->Fill(gamma_value, delta_value, -scanMinLLZ);
	}
      }
      scanFile->cd();
      scanHisto->Write();
      scanHistoP->Write();
      scanHistoM->Write();
      scanZoomHisto->Write();
      scanFile->Close();
      
      cout << "ex " << j_ex << ") " << "done 2-D scan" << endl;
    }
    
    DalitzHistoSet fitHP = BpPdf.histoSet(); 
    fitHP.save("plotsFromBpIntegrator.root");
    //  DalitzHistoSet fitHM = BmPdf.histoSet(); 
    //fitHM.save("plotsFromBmIntegrator.root");
    
    datHP.draw("dataPlotsBp_");
    fitHP.draw("fitPlotsBp_");
    datHP.drawWithFit(fitHP, "datFitBp_");
    
    /*
      datHM.draw("dataPlotsBm_");
      fitHM.draw("fitPlotsBm_");
      datHM.drawWithFit(fitHM, "datFitBm_");
    */
    
    cout << "ex " << j_ex << ") " 
	 << " ampFit done. Took " << (time(0) - startTime)/60 
	 << " min. Returning 0." << endl;
  }
  cout << "Pull study with " << (int) Nexperiments << " done." << endl;
  return 0;
}


int main(){

  return ampFit();

}
//
