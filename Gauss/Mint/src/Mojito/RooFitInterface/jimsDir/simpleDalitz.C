// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
#include <ctime>
#include <cmath>

#include "DKKpipiResonances.h"

//#include "RooM13.h"
#include "fourMxy.h"
#include "Roo4BodyPhaseSpace.h"
#include "ComplexSum.h"
#include "RooDalitz.h"
#include "dataThrowerAway.h"
#include "mark.h"

#include "doFractions.h"

#include "makeMassPlots4.h"

#include "RooGlobalFunc.h"

#include "RooGaussian.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooArgList.h"
#include "RooFormulaVar.h"
#include "RooSimultaneous.h"
#include "RooRandom.h"

#include "TH2.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"

//#include "MatrixElement.h"

//using namespace DKsPiPiResonances;
using namespace DKKpipiResonances;
using namespace RooFit;

bool noLLPlot=false;
bool noLLSeparatePlot=true;

bool fast = true;

TH2* simpleDalitz(int seed=0){
  cout << "simple Dalitz: setting random seeds to " << seed << endl;
  RooRandom::randomGenerator()->SetSeed(seed);
  gRandom->SetSeed(seed*10);

  RooRealVar dummy("dummy", "dummy", 1);

  int wasteFactor = 3500;
  int Nevents = 500;
  cout << "Now generating the full thing" << endl;

  

  //  double mMum    = 2;
  //double mdgt[4] = {0.14, 0.005, 0.5, 1};
  double mMum    = DZero_M.getVal();
  double mdgt[4] = {Kplus_M.getVal()
		    , Kminus_M.getVal()
		    , piPlus_M.getVal()
		    , piMinus_M.getVal()};

  double min_t01 = pow(mdgt[1] + mdgt[2] + mdgt[3], 2); // t01 = s234
  double min_s12 = pow(mdgt[0] + mdgt[1], 2);
  double min_s23 = pow(mdgt[1] + mdgt[2], 2);
  double min_s34 = pow(mdgt[2] + mdgt[3], 2);
  double min_t40 = pow(mdgt[0] + mdgt[1] + mdgt[2], 2); // t40 = s123

  double max_t01 = pow(mMum - mdgt[0], 2);
  double max_s12 = pow(mMum - mdgt[2] - mdgt[3], 2);
  double max_s23 = pow(mMum - mdgt[0] - mdgt[3], 2);
  double max_s34 = pow(mMum - mdgt[0] - mdgt[1], 2);
  double max_t40 = pow(mMum - mdgt[3], 2);


  double eps = 0.1;
 
  RooRealVar t01("t01", "t01", min_t01*(1.-eps), max_t01*(1.+eps));
  RooRealVar s12("s12", "s12", min_s12*(1.-eps), max_s12*(1.+eps));
  RooRealVar s23("s23", "s23", min_s23*(1.-eps), max_s23*(1.+eps));
  RooRealVar s34("s34", "s34", min_s34*(1.-eps), max_s34*(1.+eps));
  RooRealVar t40("t40", "t40", min_t40*(1.-eps), max_t40*(1.+eps));

  RooArgSet mset(t01
		 , s12, s23, s34
		 , t40
		 );
  RooArgList mlist(t01
		   , s12, s23, s34
		   , t40
		   );

  RooCategory bcharge("bcharge","bcharge") ;
  bcharge.defineType("B+",+1);
  bcharge.defineType("B-",-1); 
  bcharge.Print("s") ;
  
  bcharge.setIndex(1);
  bcharge.Print("v");
  bcharge.setIndex(1);
  bcharge.Print("v");


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
  fourMxy s13(s0, 13, "s13");
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
  cout << "Now declaring 1st ResonanceList" << endl;

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
						, m0
						, m1, m2, m3, m4
						, "notSwapped"
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
						       , m0
						       , m1, m2, m3, m4
						       , "Swapped"));

  cout << "done that" << endl;
  //  RooRealVar rB("rB", "rB", 0.15);
  RooRealVar rB("rB", "rB", 0.1, 0.01, 0.5);

  //RooRealVar delta("delta", "delta", 130);
  //RooRealVar gamma("gamma", "gamma", 60);
  RooRealVar delta("delta", "delta", 130, -180, 180);
  RooRealVar gamma("gamma", "gamma", 60, -180, 180);

  //RooRealVar delta("delta", "delta", 0, -180, 180);
  //RooRealVar gamma("gamma", "gamma", 0, -180, 180);


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


  cout << "done BPlus1, 2, Minus.." << endl;

  RooArgList lplus(BPlus1, BPlus2);
  RooArgList lmin(BMinus1, BMinus2);

  cout << " made argLists" << endl;

  ComplexSum BPlus("BPlus", "BPlus", lplus);
  ComplexSum BMinus("BMinus", "BMinus", lmin);

  cout << "made BPlus, BMinus" << endl;

  RooDalitz DaPlusPDF("DalitzPlus", "DalitzPlus"
		      , BPlus, phaseSpace, mlist);
  RooDalitz DaMinusPDF("DalitzMinus", "DalitzMinus"
		       , BMinus, phaseSpace, mlist);
  time_t t7 = time(NULL);

  cout << " generating phase-space events. " << endl;
  RooDataSet* dataPlus  = phaseSpace.generate(mset, wasteFactor*Nevents);

  cout << "generated phase-space events." << endl;
  cout << "getting FRACTIONS!!!" << endl;
  cout << " -- BPlus1 on dataPlus: " << endl;
  doFractions(BPlus1, dataPlus, 1000);
  /*
  cout << " -- BPlus2 on dataPlus: " << endl;
  doFractions(BPlus2, dataPlus);

  cout << " -- BMinus1 on dataPlus: " << endl;
  doFractions(BMinus1, dataPlus);
  cout << " -- BMinus2 on dataPlus: " << endl;
  doFractions(BMinus2, dataPlus);
  */

  dataPlus = dataThrowerAway(BPlus, mlist, dataPlus, Nevents);
  bcharge.setIndex(1);
  dataPlus = mark(dataPlus, bcharge);
  dataPlus->Print("v");
  cout << "after dataThrowerAway I have " 
       << dataPlus->numEntries() 
       << " in dataPlus " << endl;

  RooDataSet* dataMinus  = phaseSpace.generate(mlist, wasteFactor*Nevents);
  dataMinus = dataThrowerAway(BMinus, mlist, dataMinus, Nevents);
  bcharge.setIndex(-1);
  dataMinus = mark(dataMinus, bcharge);
  cout << "after dataThrowerAway I have " 
       << dataMinus->numEntries() 
       << " in dataMinus " << endl;
    //  return 0;

  RooDataSet* data = (RooDataSet*) dataPlus->Clone();
  data->append(*dataMinus);
  data->Print("v");

  /*
  //  RooDataSet* dataMinus = phaseSpace.generate(RooArgSet(m12sq, m23sq), 1000) 
  RooDataSet* dataPlus = DaPlusPDF.generate(RooArgSet(m12sq, m23sq), Nevents);
  cout << "generated B+" << endl;
  RooDataSet* dataMinus = DaMinusPDF.generate(RooArgSet(m12sq, m23sq), Nevents);
  cout << "generated B-" << endl;
  */

  cout << "making ph2Plus histogram" << endl;
  TFile fData("DKpipi1k.root", "RECREATE");
  fData.cd();
  dataPlus->Write();
  dataMinus->Write();
  fData.Close();
  TH2* ph2Plus = s23.createHistogram("s23VSs12Plus"
				     , s12
				     );
  TH2* ph2Minus = s23.createHistogram("s23VSs12Minus"
				      , s12
				      );

  cout << "made the histogram, now filling it:" << endl;
  dataPlus->fillHistogram(ph2Plus, RooArgList(s12, s23));
  cout << "filled plus " << endl;
  dataMinus->fillHistogram(ph2Minus, RooArgList(s12, s23));
  cout << "filled minus" << endl;
  
  TH2* diff = (TH2*) ph2Plus->Clone("diff");
  cout << "made diff" << endl;
  diff->Add(ph2Minus, -1.);
  cout << "filled diff" << endl;

  TCanvas tcan("canofwank");

  cout << "made tcan" << endl;
  int nbins = Nevents/20;
  if(nbins > 400)nbins=400;

  RooPlot* t01Frame = t01.frame();
  dataPlus->plotOn(t01Frame
		   );// , Binning(s23.getMin(), s23.getMax(), nbins));
  dataMinus->plotOn(t01Frame, MarkerColor(2), LineColor(2)
		    );// , Binning(s23.getMin(), s23.getMax(), nbins));
  t01Frame->Draw();
  tcan.Print("t01.eps");


  RooPlot* s12Frame = s12.frame();
  dataPlus->plotOn(s12Frame
		   );
  dataMinus->plotOn(s12Frame, MarkerColor(2), LineColor(2)
		    );//, Binning(m12sq.getMin(), m12sq.getMax(), nbins));
  s12Frame->Draw();
  tcan.Print("s12.eps");
  
  RooPlot* s23Frame = s23.frame();
  dataPlus->plotOn(s23Frame
		   );// , Binning(s23.getMin(), s23.getMax(), nbins));
  dataMinus->plotOn(s23Frame, MarkerColor(2), LineColor(2)
		    );// , Binning(s23.getMin(), s23.getMax(), nbins));
  s23Frame->Draw();
  tcan.Print("s23.eps");
    
  RooPlot* s34Frame = s34.frame();
  dataPlus->plotOn(s34Frame
		   );// , Binning(s34.getMin(), s34.getMax(), nbins));
  dataMinus->plotOn(s34Frame, MarkerColor(2), LineColor(2)
		    );// , Binning(s34.getMin(), s34.getMax(), nbins));
  s34Frame->Draw();
  tcan.Print("s34.eps");
    
  RooPlot* t40Frame = t40.frame();
  dataPlus->plotOn(t40Frame
		   );// , Binning(t40.getMin(), t40.getMax(), nbins));
  dataMinus->plotOn(t40Frame, MarkerColor(2), LineColor(2)
		    );// , Binning(t40.getMin(), t40.getMax(), nbins));
  t40Frame->Draw();
  tcan.Print("t40.eps");
  
  cout << "made all the plots. " << endl;
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

  cout << "making massPlots: " << endl;
  massPlotSet4 plotsAll = makeMassPlots4(s0, data, "all");
  cout << "made 'all'" << endl;
  plotsAll.saveHistosAndEps();
  cout << "saved 'all'"<< endl;

  massPlotSet4 plotsPlus = makeMassPlots4(s0, dataPlus, "plus");
  cout << "made plus" << endl;
  plotsPlus.saveHistosAndEps();
  cout << "saved plus" << endl;

  massPlotSet4 plotsMinus = makeMassPlots4(s0, dataMinus, "minus");
  cout << "made minus" << endl;
  plotsMinus.saveHistosAndEps();
  cout << "saved minus" << endl;

  cout << "made all mass plots. " << endl;
  //  return ph2Plus;

  
  time_t t8 = time(NULL);
  cout << "generating full Dalitz took " 
       << difftime(t8, t7) << " s" << endl;
  cout << "now doing the fit to Bplus:" << endl;
  DaPlusPDF.constOptimize(RooAbsArg::DeActivate) ;
  DaMinusPDF.constOptimize(RooAbsArg::DeActivate) ;


  RooSimultaneous simPdf("simPdf", "simPdf", bcharge);
  simPdf.addPdf(DaPlusPDF, "B+");
  simPdf.addPdf(DaMinusPDF, "B-");
  
  if(fast){
    simPdf.fitTo(*data, Optimize(false), Minos(false), Hesse(false));
  }else{
    simPdf.fitTo(*data, Optimize(false));
  }
  cout << "Done the fit. Drawing logL" << endl;

  RooPlot* llframe = gamma.frame(-180, 180, 100);
  cout << "got the frame" << endl;
  if(! noLLPlot){
    cout << "calling simPdf.plotNLLOn(llframe, data)" << endl;
    simPdf.plotNLLOn(llframe, data);
    cout << "did that." << endl;
    llframe->Draw();
    tcan.Print("ll.eps");
    
    TFile fLL("DalitzLL.root", "RECREATE");
    fLL.cd();
    llframe->Write();
    fLL.Close();
  }

  return 0;

  bool doHandLik=false;
  if(doHandLik){
    int nhbins = 400;
    double hmin=-180, hmax=180;
    double bw=(hmax-hmin)/((double)nhbins);
    TH1F moh("myOwnHisto", "myOwnHisto", nhbins, hmin, hmax);
    
    DaPlusPDF.attachDataSet(*dataPlus);
    for(int i=0; i < dataPlus->numEntries();  i++){
      dataPlus->get(i);
      for(int bin=1; bin <= nhbins; bin++){
	double g = (((double) bin) - 0.5)*bw + hmin;
	gamma.setVal(g);
	double ll = -DaPlusPDF.getLogVal();
	moh.Fill(g, ll);
      }
    }
    moh.Draw();
    tcan.Print("mohPlus.eps");
  }

  RooPlot* llPlusframe = gamma.frame(-180, 180, 100);
  RooPlot* llMinusframe = gamma.frame(-180, 180, 100);
  if(! (noLLSeparatePlot || noLLPlot)){
    DaPlusPDF.plotNLLOn(llPlusframe, dataPlus);
    llPlusframe->Draw();
    tcan.Print("llPlus.eps");

    DaMinusPDF.plotNLLOn(llMinusframe, dataMinus);
    llMinusframe->Draw();
    tcan.Print("llMinus.eps");
  }

  char* zAxisLabel = "Dalitz";
  Double_t xlo=0, xhi=5;
  Int_t nBins=100;
  
  RooArgSet allBut_t01(     s12, s23, s34, t40);
  RooArgSet allBut_s12(t01,      s23, s34, t40);
  RooArgSet allBut_s23(t01, s12,      s34, t40);
  RooArgSet allBut_s34(t01, s12, s23,      t40);
  RooArgSet allBut_t40(t01, s12, s23, s34     );

  /*
  phaseSpace.plotOn(t01Frame, LineColor(4),ProjWData(*dataPlus));
  t01Frame->Draw();
  tcan.Print("t01.eps");
  */
  
  DaPlusPDF.plotOn(t01Frame, ProjWData(*dataPlus));
  t01Frame->Draw();
  tcan.Print("t01.eps");
  DaMinusPDF.plotOn(t01Frame, LineColor(2), ProjWData(*dataMinus));
  t01Frame->Draw();
  tcan.Print("t01.eps");
  
  /*
  phaseSpace.plotOn(s12Frame, LineColor(4),ProjWData(*dataPlus));
  s12Frame->Draw();
  tcan.Print("s12.eps");
  */

  DaPlusPDF.plotOn(s12Frame, ProjWData(*dataPlus));
  s12Frame->Draw();
  tcan.Print("s12.eps");
  DaMinusPDF.plotOn(s12Frame, LineColor(2), ProjWData(*dataMinus));
  s12Frame->Draw();
  tcan.Print("s12.eps");

  DaPlusPDF.plotOn(s23Frame, ProjWData(*dataPlus));
  s23Frame->Draw();
  tcan.Print("s23.eps");
  DaMinusPDF.plotOn(s23Frame, LineColor(2), ProjWData(*dataMinus));
  s23Frame->Draw();
  tcan.Print("s23.eps");

  DaPlusPDF.plotOn(s34Frame, ProjWData(*dataPlus));
  s34Frame->Draw();
  tcan.Print("s34.eps");
  DaMinusPDF.plotOn(s34Frame, LineColor(2), ProjWData(*dataMinus));
  s34Frame->Draw();
  tcan.Print("s34.eps");
    
  DaPlusPDF.plotOn(t40Frame, ProjWData(*dataPlus));
  t40Frame->Draw();
  tcan.Print("t40.eps");
  DaMinusPDF.plotOn(t40Frame, LineColor(2), ProjWData(*dataMinus));
  t40Frame->Draw();
  tcan.Print("t40.eps");
    

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
  llframe->Write();
  llPlusframe->Write();
  llMinusframe->Write();

  fFit.Close();
  return ph2Plus;
}
//
//

