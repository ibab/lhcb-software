// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
#include <ctime>
#include <cmath>

#include "DKKpipiResonances.h"

//#include "RooM13.h"
#include "fourMxy.h"
#include "Roo4BodyPhaseSpace.h"
#include "ComplexSum.h"
#include "RooDalitz.h"
#include "dataThrowerAway.h"

#include "RooGlobalFunc.h"

#include "RooGaussian.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooArgList.h"
#include "RooFormulaVar.h"

#include "TH2.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"

//#include "MatrixElement.h"

//using namespace DKsPiPiResonances;
using namespace DKKpipiResonances;
using namespace RooFit;

TH2* flatPhaseSpace(){

  RooRealVar dummy("dummy", "dummy", 1);

  //  double mMum    = 2;
  //double mdgt[4] = {0.14, 0.005, 0.5, 1};
  double mMum    = DZero_M.getVal();
  double mdgt[4] = {Kplus_M.getVal()
		    , Kminus_M.getVal()
		    , piPlus_M.getVal()
		    , piMinus_M.getVal()};


  double min_t01 = pow(mdgt[0] + mdgt[1] + mdgt[2], 2); // t01 = s123
  double min_s12 = pow(mdgt[0] + mdgt[1], 2);
  double min_s23 = pow(mdgt[1] + mdgt[2], 2);
  double min_s34 = pow(mdgt[2] + mdgt[3], 2);
  double min_t40 = pow(mdgt[1] + mdgt[2] + mdgt[3], 2); // t40 = s234

  double max_t01 = pow(mMum - mdgt[3], 2);
  double max_s12 = pow(mMum - mdgt[2] - mdgt[3], 2);
  double max_s23 = pow(mMum - mdgt[0] - mdgt[3], 2);
  double max_s34 = pow(mMum - mdgt[0] - mdgt[1], 2);
  double max_t40 = pow(mMum - mdgt[0], 2);

  double eps = 0.1;

  double t01_def=2.3905
    , s12_def=1.37493, s23_def=0.959244, s34_def=0.365484
    , t40_def=1.5331;

  RooRealVar t01("t01", "t01", t01_def, min_t01*(1.-eps), max_t01*(1.+eps));
  RooRealVar s12("s12", "s12", s12_def, min_s12*(1.-eps), max_s12*(1.+eps));
  RooRealVar s23("s23", "s23", s23_def, min_s23*(1.-eps), max_s23*(1.+eps));
  RooRealVar s34("s34", "s34", s34_def, min_s34*(1.-eps), max_s34*(1.+eps));
  RooRealVar t40("t40", "t40", t40_def, min_t40*(1.-eps), max_t40*(1.+eps));

  RooRealVar m0("m0", "m0", mMum);
  RooRealVar m1("m1", "m1", mdgt[0]);
  RooRealVar m2("m2", "m2", mdgt[1]);
  RooRealVar m3("m3", "m3", mdgt[2]);
  RooRealVar m4("m4", "m4", mdgt[3]);

  fourMxy s0("s0", "MxySq"
	     , t01, s12, s23, s34, t40
	     , m0, m1, m2, m3, m4
	     , 13
	     );
  fourMxy s13(s0, 13, "s14");
  fourMxy s14(s0, 14, "s14");
  fourMxy s24(s0, 24, "s24");
  fourMxy s123(s0, 123, "s123");
  fourMxy s124(s0, 124, "s124");
  fourMxy s134(s0, 134, "s134");
  fourMxy s234(s0, 234, "s234");

  cout << "declaring phaseSpace " << endl;
  Roo4BodyPhaseSpace phaseSpace("phaseSpace", "phaseSpace"
				, t01
				, s12, s23, s34
				, t40
				, m0
				, m1, m2, m3, m4
				);
  cout << "phaseSpace declared " << endl;

  RooArgList ResonanceList(makeDKKpipiResonance(s12
					 , s13
					 , s14
					 , s23
					 , s24
					 , s34
					 , s123
					 , s124
					 , s134
					 , s234
					 ));

  cout << " ... end 2nd " << endl;
  RooArgList ResonanceListSwapped(makeDKKpipiResonance(s12
						, s24
						, s23
						, s14
						, s13
						, s34
						, s124
						, s123
						, s234
						, s134
						));

  cout << "done that" << endl;
  RooRealVar rB("rB", "rB", 0.2);

  RooRealVar delta("delta", "delta", 0);
  RooRealVar gamma("gamma", "gamma", 60);

  RooRealVar degrees("degrees", "degrees", TMath::Pi()/180.0);
  RooFormulaVar BPlusPhase("BPlusPhase", "(delta + gamma)*degrees"
			   , RooArgList(delta, gamma, degrees));
  RooFormulaVar BMinusPhase("BMinusPhase", "(delta - gamma)*degrees"
			    , RooArgList(delta, gamma, degrees));

  cout << "bplus phase " << BPlusPhase.getVal()
       << ", bminus phase " << BMinusPhase.getVal() << endl;

  ComplexSum BPlus1("BPlus1", "BPlus1"
		    , ResonanceList);
  ComplexSum BPlus2("BPlus2", "BPlus2"
		    , ResonanceListSwapped, rB, BPlusPhase);
  ComplexSum BMinus1("BMinus1", "BMinus1"
		     , ResonanceList);
  ComplexSum BMinus2("BMinus2", "BMinus2"
		     , ResonanceListSwapped, rB, BMinusPhase);

  RooArgList lplus(BPlus1, BPlus2);
  RooArgList lmin(BMinus1, BMinus2);

  ComplexSum BPlus("BPlus", "BPlus", lplus);
  ComplexSum BMinus("BMinus", "BMinus", lmin);

  RooDalitz DaPlusPDF_("DalitzPlus", "DalitzPlus", BPlus, dummy);
  RooProdPdf DaPlusPDF("dp", "dp", DaPlusPDF_, phaseSpace);
  RooDalitz DaMinusPDF_("DalitzMinus", "DalitzMinus", BMinus, dummy);
  RooProdPdf DaMinusPDF("dm", "dm", DaMinusPDF_, phaseSpace);
  time_t t7 = time(NULL);

  int Nevents = 100;
  cout << "Now generating the full thing" << endl;
  RooArgSet mset(t01
		 , s12, s23, s34
		 , t40
		 );
  RooArgList mlist(t01
		   , s12, s23, s34
		   , t40
		   );
  TCanvas tcan;

  RooPlot* t01Frame = t01.frame();
  RooPlot* s12Frame = s12.frame();
  RooPlot* s23Frame = s23.frame();
  RooPlot* s34Frame = s34.frame();
  RooPlot* t40Frame = t40.frame();


  RooPlot* pst01Frame = t01.frame();
  RooPlot* pss12Frame = s12.frame();
  RooPlot* pss23Frame = s23.frame();
  RooPlot* pss34Frame = s34.frame();
  RooPlot* pst40Frame = t40.frame();
  TH2* ph2Plus = s23.createHistogram("s23VSs12Plus"
				     , s12
				     );
  TH2* ph2Minus = s23.createHistogram("s23VSs12Minus"
				      , s12
				      );

  
  RooDataSet* dataPlus  = DaPlusPDF_.generate(mset, Nevents);

  cout << "generated + events." << endl;

  RooDataSet* dataMinus  = DaPlusPDF_.generate(mlist, 10*Nevents);

  cout << "generated - events." << endl;

  TFile fData("DKpipi1k.root", "RECREATE");
  fData.cd();
  dataPlus->Write();
  dataMinus->Write();
  fData.Close();

  dataPlus->fillHistogram(ph2Plus, RooArgList(s12, s23));
  dataMinus->fillHistogram(ph2Minus, RooArgList(s12, s23));
  
  TH2* diff = (TH2*) ph2Plus->Clone("diff");
  diff->Add(ph2Minus, -1.);
  

  int nbins = Nevents/20;
  if(nbins > 400)nbins=400;

  dataPlus->plotOn(t01Frame
		   );// , Binning(s23.getMin(), s23.getMax(), nbins));
  dataMinus->plotOn(t01Frame, MarkerColor(2), LineColor(2)
		    );// , Binning(s23.getMin(), s23.getMax(), nbins));
  t01Frame->Draw();
  tcan.Print("t01.eps");


  dataPlus->plotOn(s12Frame
		   );
  dataMinus->plotOn(s12Frame, MarkerColor(2), LineColor(2)
		    );//, Binning(m12sq.getMin(), m12sq.getMax(), nbins));
  s12Frame->Draw();
  tcan.Print("s12.eps");
  
  dataPlus->plotOn(s23Frame
		   );// , Binning(s23.getMin(), s23.getMax(), nbins));
  dataMinus->plotOn(s23Frame, MarkerColor(2), LineColor(2)
		    );// , Binning(s23.getMin(), s23.getMax(), nbins));
  s23Frame->Draw();
  tcan.Print("s23.eps");
    
  dataPlus->plotOn(s34Frame
		   );// , Binning(s34.getMin(), s34.getMax(), nbins));
  dataMinus->plotOn(s34Frame, MarkerColor(2), LineColor(2)
		    );// , Binning(s34.getMin(), s34.getMax(), nbins));
  s34Frame->Draw();
  tcan.Print("s34.eps");
    
  dataPlus->plotOn(t40Frame
		   );// , Binning(t40.getMin(), t40.getMax(), nbins));
  dataMinus->plotOn(t40Frame, MarkerColor(2), LineColor(2)
		    );// , Binning(t40.getMin(), t40.getMax(), nbins));
  t40Frame->Draw();
  tcan.Print("t40.eps");
    
  TFile fHisto("DalitzHisto.root", "RECREATE");
  fHisto.cd();
  ph2Plus->Write();
  t01Frame->Write();
  s12Frame->Write();
  s23Frame->Write();
  s34Frame->Write();
  t40Frame->Write();
  ph2Minus->Write();
  diff->Write();
  fHisto.Close();

  //  return ph2Plus;
  
  time_t t8 = time(NULL);
  cout << "generating full Dalitz took " 
       << difftime(t8, t7) << " s" << endl;
  cout << "now doing the fit to Bplus:" << endl;
  DaPlusPDF_.constOptimize(RooAbsArg::DeActivate) ;
  DaPlusPDF_.fitTo(*dataPlus, Optimize(false));
  time_t t9 = time(NULL);
  cout << "fitting Bplus took " 
       << difftime(t9, t8) << " s" << endl;
  cout << "now doing the fit to Bminus:" << endl;
  DaMinusPDF_.constOptimize(RooAbsArg::DeActivate) ;
  DaMinusPDF_.fitTo(*dataMinus, Optimize(false));
  time_t t10 = time(NULL);
 
  cout << " done the fit to Bminus That took" 
       << difftime(t10, t9) << " s" << endl;


  

  char* zAxisLabel = "Dalitz";
  Double_t xlo=0, xhi=5;
  Int_t nBins=100;
  
  RooArgSet allBut_t01(     s12, s23, s34, t40);
  RooArgSet allBut_s12(t01,      s23, s34, t40);
  RooArgSet allBut_s23(t01, s12,      s34, t40);
  RooArgSet allBut_s34(t01, s12, s23,      t40);
  RooArgSet allBut_t40(t01, s12, s23, s34     );

  DaPlusPDF_.plotOn(t01Frame);// , ProjWData(*dataPlus));
  t01Frame->Draw();
  tcan.Print("t01.eps");
  DaMinusPDF_.plotOn(t01Frame, LineColor(2));// , ProjWData(*dataMinus));
  t01Frame->Draw();
  tcan.Print("t01.eps");

  DaPlusPDF_.plotOn(s12Frame);// , ProjWData(*dataPlus));
  s12Frame->Draw();
  tcan.Print("s12.eps");
  DaMinusPDF_.plotOn(s12Frame, LineColor(2));// , ProjWData(*dataMinus));
  s12Frame->Draw();
  tcan.Print("s12.eps");
  
  DaPlusPDF_.plotOn(s23Frame);// , ProjWData(*dataPlus));
  s23Frame->Draw();
  tcan.Print("s23.eps");
  DaMinusPDF_.plotOn(s23Frame, LineColor(2));// , ProjWData(*dataMinus));
  s23Frame->Draw();
  tcan.Print("s23.eps");

  DaPlusPDF_.plotOn(s34Frame);// , ProjWData(*dataPlus));
  s34Frame->Draw();
  tcan.Print("s34.eps");
  DaMinusPDF_.plotOn(s34Frame, LineColor(2));// , ProjWData(*dataMinus));
  s34Frame->Draw();
  tcan.Print("s34.eps");
    
  DaPlusPDF_.plotOn(t40Frame);// , ProjWData(*dataPlus));
  t40Frame->Draw();
  tcan.Print("t40.eps");
  DaMinusPDF_.plotOn(t40Frame, LineColor(2));// , ProjWData(*dataMinus));
  t40Frame->Draw();
  tcan.Print("t40.eps");
    
  // now just the phase space on its own:
  phaseSpace.plotOn(pst01Frame);// , ProjWData(*dataPlus));
  pst01Frame->Draw();
  tcan.Print("pst01.eps");

  phaseSpace.plotOn(pss12Frame);// , ProjWData(*dataPlus));
  pss12Frame->Draw();
  tcan.Print("pss12.eps");
  
  phaseSpace.plotOn(pss23Frame);// , ProjWData(*dataPlus));
  pss23Frame->Draw();
  tcan.Print("pss23.eps");

  phaseSpace.plotOn(pss34Frame);// , ProjWData(*dataPlus));
  pss34Frame->Draw();
  tcan.Print("pss34.eps");
    
  phaseSpace.plotOn(pst40Frame);// , ProjWData(*dataPlus));
  pst40Frame->Draw();
  tcan.Print("pst40.eps");


  TFile fFit("DalitzHistoFit.root", "RECREATE");
  fFit.cd();
  ph2Plus->Write();
  ph2Minus->Write();
  t01Frame->Write();
  s12Frame->Write();
  s23Frame->Write();
  s34Frame->Write();
  t40Frame->Write();
  ph2Minus->Write();
  fFit.Close();
  return ph2Plus;
}
//
//

