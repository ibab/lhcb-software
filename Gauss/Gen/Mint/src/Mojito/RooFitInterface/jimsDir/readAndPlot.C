// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#include <ctime>
#include "DKsPiPiResonances.h"
//#include "MatrixElement.h"

using namespace DKsPiPiResonances;

TH2* readAndPlot(){

  gROOT->LoadMacro("twoDStyle.C");
  twoDStyle();
  RooRealVar dummy("dummy", "dummy", 1);

  gROOT->LoadMacro("AbsComplexBrackets.C+");
  gROOT->LoadMacro("AbsComplex.C+");
  gROOT->LoadMacro("AbsComplexPdf.C+");
  gROOT->LoadMacro("ComplexProxy.C+");
  gROOT->LoadMacro("ComplexBW.C+");
  gROOT->LoadMacro("DcyAmplitude.C+");
  gROOT->LoadMacro("newMatrixElement.C+");
  gROOT->LoadMacro("Resonance.C+");
  gROOT->LoadMacro("ComplexSum.C+");
  gROOT->LoadMacro("ComplexSumPdf.C+");
  gROOT->LoadMacro("ComplexProd.C+");
  gROOT->LoadMacro("RooM13.C+");
  gROOT->LoadMacro("Roo3BodyPhaseSpace.C+");
  gROOT->LoadMacro("RooDalitz.C+");


  RooRealVar m12sq("m12sq", "m12sq", 0, 3.2);
  RooRealVar m23sq("m23sq", "m23sq", 0, 3.2);

  RooM13 m13sq("m13sq", "m13sq"
	       , m12sq, m23sq, DZeroM, piPlusM, KsM, piMinusM); 
  RooM13 m13Swsq("m13sqSwapped", "m13sqSwapped"
	       , m23sq, m12sq, DZeroM, piPlusM, KsM, piMinusM); 
  // of course pi+ and pi- have the same mass, this is just a reminder of how
  // we interpret the particles later on 
  // (decides whether m12 or m13 is cabbibo suppressed).

  cout << "declaring phaseSpace " << endl;
  Roo3BodyPhaseSpace phaseSpace("phaseSpace", "phaseSpace"
				, m12sq, m23sq
				, DZeroM, piPlusM, KsM, piMinusM
				);

  RooArgList ResonanceList(makeResonance(m12sq, m23sq, m13sq));
  RooArgList ResonanceListSwapped(makeResonance(m23sq, m12sq
						, m13Swsq
						, "swapped"));

  RooRealVar rB("rB", "rB", 1);
  //  RooRealVar delta("delta", "delta", 0, -180, 180);
  //  RooRealVar rB("rB", "rB", 0.2);
  RooRealVar delta("delta", "delta", 0);
  RooRealVar gamma("gamma", "gamma", 65, 0, 180);
  //  RooRealVar gamma("gamma", "gamma", 60*deg, -180.*deg, 180.*deg);
  RooRealVar degrees("degrees", "degrees", TMath::Pi()/180.0);
  RooFormulaVar BPlusPhase("BPlusPhase", "(delta + gamma)*degrees"
			   , RooArgList(delta, gamma, degrees));
  RooFormulaVar BMinusPhase("BMinusPhase", "(delta - gamma)*degrees"
			    , RooArgList(delta, gamma, degrees));

  ComplexSum BPlus1("BPlus1", "BPlus1"
		    , ResonanceList);
  ComplexSum BPlus2("BPlus2", "BPlus2"
		    , ResonanceListSwapped, rB, BPlusPhase);
  ComplexSum BMinus1("BMinus1", "BMinus1"
		     , ResonanceList);
  ComplexSum BMinus2("BMinus2", "BMinus2"
		     , ResonanceListSwapped, rB, BMinusPhase);

  ComplexSum BPlus("BPlus", "BPlus", RooArgList(BPlus1, BPlus2));
  ComplexSum BMinus("BMinus", "BMinus", RooArgList(BMinus1, BMinus2));
  //  ComplexSum BMinus("bw", "bw", RooArgList(BMin1, BMin2));
  //  ComplexSum allB("bw", "bw", RooArgList(BPlus1, BPlus2, BMin1, BMin2));
  //  RooDalitz BPlusPDF("Dalitz", "Dalitz", BPlus, phaseSpace);
  //  RooDalitz BMinusPDF("Dalitz", "Dalitz", BMinus, phaseSpace);
  //  RooDalitz DaPDF("Dalitz", "Dalitz", BPlus, phaseSpace);
  RooDalitz DaPlusPDF("DalitzPlus", "DalitzPlus", BPlus, dummy);
  RooDalitz DaMinusPDF("DalitzMinus", "DalitzMinus", BMinus, dummy);
  time_t t7 = time(NULL);
  cout << "Now generating the full thing" << endl;
  //  RooDataSet* data = DaPDF.generate(RooArgSet(m12sq, m23sq), 1000);

  int Nevents = 4000;

  TFile fData("DKpipi.root");
  cout << "opened file" << endl;
  fData.cd();
  RooDataSet* dataPlus = fData.Get("DalitzPlusData");
  cout << "read in data plus" << endl;
  RooDataSet* dataMinus = fData.Get("DalitzMinusData");
  cout << "read in data min " << endl;
  cout << " the ptrs: " << dataPlus << ", " << dataMinus << endl;
  //  fData.Close();
  cout << "closed file" << endl;
  TH2* ph2Plus = m23sq.createHistogram("m23sqVSm12sqPlus"
				       , m12sq
				       );
  TH2* ph2Minus = m23sq.createHistogram("m23sqVSm12sqMinus"
					, m12sq
					);
  cout << "made histograms " << endl;
  dataPlus->fillHistogram(ph2Plus, RooArgList(m12sq, m23sq));
  cout << "filled data plus 2D histo " << endl;
  dataMinus->fillHistogram(ph2Minus, RooArgList(m12sq, m23sq));
  cout << " filled data minus 2D histo " << endl;
  TCanvas tcan;

  int nbins = Nevents/50;
  if(nbins > 100)nbins=100;

  RooPlot* m12Frame = m12sq.frame(20);
  dataPlus->plotOn(m12Frame
		   );//, Binning(m12sq.getMin(), m12sq.getMax(), nbins));
  //  m12FramePlus->Draw();
  //  RooPlot* m12FrameMinus = m12sq.frame();
  //m12FrameMinus->SetMarkerColor(2);
  dataMinus->plotOn(m12Frame, MarkerColor(2), LineColor(2)
		    );//, Binning(m12sq.getMin(), m12sq.getMax(), nbins));
  m12Frame->Draw();
  tcan.Print("m12.eps");
  
  RooPlot* m23Frame = m23sq.frame(20);
  dataPlus->plotOn(m23Frame
		   );// , Binning(m23sq.getMin(), m23sq.getMax(), nbins));
  dataMinus->plotOn(m23Frame, MarkerColor(2), LineColor(2)
		    );// , Binning(m23sq.getMin(), m23sq.getMax(), nbins));
  m23Frame->Draw();
  tcan.Print("m23.eps");
  
  TFile fHisto("DalitzHisto.root", "RECREATE");
  fHisto.cd();
  ph2Plus->Write();
  m12Frame->Write();
  m23Frame->Write();
  ph2Minus->Write();
  fHisto.Close();

  //  return ph2Plus;

  
  time_t t8 = time(NULL);
  cout << "generating full Dalitz took " 
       << difftime(t8, t7) << " s" << endl;
  cout << "now doing the fit to Bplus:" << endl;
  DaPlusPDF.constOptimize(RooAbsArg::DeActivate) ;
  DaPlusPDF.fitTo(*dataPlus, Optimize(false));
  time_t t9 = time(NULL);
  cout << "fitting Bplus took " 
       << difftime(t9, t8) << " s" << endl;
  cout << "now doing the fit to Bminus:" << endl;
  DaMinusPDF.constOptimize(RooAbsArg::DeActivate) ;
  DaMinusPDF.fitTo(*dataMinus, Optimize(false));
  time_t t10 = time(NULL);
 
  cout << " done the fit to Bminus That took" 
       << difftime(t10, t9) << " s" << endl;

  char* zAxisLabel = "Dalitz";
  Double_t xlo=0, xhi=5;
  Int_t nBins=100;
  
  DaPlusPDF.plotOn(m12Frame);
  DaMinusPDF.plotOn(m12Frame, LineColor(2));
  DaPlusPDF.plotOn(m23Frame);
  DaMinusPDF.plotOn(m23Frame, LineColor(2));


  TFile fFit("DalitzHistoFit.root", "RECREATE");
  fFit.cd();
  ph2->Write();
  m12Frame->Write();
  m23Frame->Write();
  ph2Minus->Write();
  fFit.Close();
  return ph2Plus;
}
//
//

