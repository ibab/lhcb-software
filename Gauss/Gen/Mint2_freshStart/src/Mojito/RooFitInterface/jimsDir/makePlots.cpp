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
#include "mark.h"

#include "doFractions.h"

#include "makeMassPlots4.h"

#include "RooGlobalFunc.h"

#include "RooAbsArg.h"
#include "RooGaussian.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooBinning.h"
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

bool just2D=true;


RooPlot* plotMsq(const RooDataSet* dataPlus
		 , const RooDataSet* dataMinus
		 , const RooRealVar& var
		 , int nbins
		 , const char* xUnits_in=0
		 , const char* xTitle_in=0
		 ){
  RooPlot* varFrame = var.frame();
  //  return varFrame;

  std::string varName = var.GetName();
  char xUnits[200];
  if(! xUnits_in){
    sprintf(xUnits, "GeV^{ 2} ");
  }else{
    sprintf(xUnits, "%s", xUnits_in);
  }
  char xTitle[200];
  if(! xTitle_in){
    sprintf(xTitle, "%s [%s]", varName.c_str(), xUnits);
  }else{
    sprintf(xTitle, "%s", xTitle_in);
  }
  double bw = (var.getMax()-var.getMin())/((double)nbins);
  char yTitle[200];
  sprintf(yTitle, "Events/(%.3f %s)", bw, xUnits);
  printf("\n y-title:  Events/(%.3f %s)\n", bw, xUnits);
  cout << " bw " << bw << " xUnits " << xUnits << endl;
 
  char* drawOpt="";

  dataPlus->plotOn(varFrame, DrawOption(drawOpt), LineStyle(1)
		   , Binning(nbins, var.getMin(), var.getMax()));
  cout << "plotted dataPlus on varFrame" << endl;
  dataMinus->plotOn(varFrame, DrawOption(drawOpt), LineStyle(2)
		    , MarkerColor(2), LineColor(2)
		    , Binning(nbins, var.getMin(), var.getMax()));
  varFrame->SetNameTitle(varName.c_str(), varName.c_str());

  varFrame->GetXaxis()->SetTitle(xTitle);
  varFrame->GetXaxis()->SetTitleOffset(1.1);
  varFrame->GetXaxis()->SetTitleColor(1);
  varFrame->GetXaxis()->SetLabelOffset(0.015);

  varFrame->GetYaxis()->SetTitle(yTitle);
  varFrame->GetYaxis()->SetTitleOffset(2.2);
  varFrame->GetYaxis()->SetLabelOffset(0.015);

  varFrame->GetXaxis()->SetNdivisions(505);
  varFrame->GetYaxis()->SetNdivisions(507);

  //  varFrame->GetXaxis()->SetTitleSize(0.07);
  //  varFrame->GetYaxis()->SetTitleSize(0.05);

  std::string canvasName = varName + "_canvas";
  TCanvas tcan(canvasName.c_str());
  tcan.cd();

  varFrame->Draw();

  std::string plotFileName=varName + ".eps";
  tcan.Print(plotFileName.c_str());

  return varFrame;
}

RooPlot* plotAsy(const RooDataSet* data
		 , const RooCategory& bcharge
		 , const RooRealVar& var
		 , int nbins
		 , const char* xUnits_in=0
		 , const char* xTitle_in=0
		 ){
  RooPlot* varFrame = var.frame();
  std::string varName = var.GetName();
  char xUnits[200];
  if(! xUnits_in){
    sprintf(xUnits, "GeV^{ 2} ");
  }else{
    sprintf(xUnits, "%s", xUnits_in);
  }
  char xTitle[200];
  if(! xTitle_in){
    sprintf(xTitle, "%s [%s]", varName.c_str(), xUnits);
  }else{
    sprintf(xTitle, "%s", xTitle_in);
  }
  double bw = (var.getMax()-var.getMin())/((double)nbins);
  char yTitle[200];
  //  sprintf(yTitle, "#frac{N(B^{+ }) - N(B^{- })}{N(B^{+ }) + N(B^{- })} ");
  sprintf(yTitle, "Asymmetry");
 
  char* drawOpt="";

  data->plotOn(varFrame, Asymmetry(bcharge)
	       , Binning(nbins, var.getMin(), var.getMax()));
  cout << "plotted data on varFrame for Asy" << endl;

  varFrame->SetNameTitle((varName+"Asymmetry").c_str()
			 , (varName+"Asymmetry").c_str());

  varFrame->GetXaxis()->SetTitle(xTitle);
  varFrame->GetXaxis()->SetTitleOffset(1.1);
  varFrame->GetXaxis()->SetTitleColor(1);
  varFrame->GetXaxis()->SetLabelOffset(0.015);

  varFrame->GetYaxis()->SetTitle(yTitle);
  varFrame->GetYaxis()->SetTitleOffset(1.6);
  varFrame->GetYaxis()->SetLabelOffset(0.015);

  varFrame->GetXaxis()->SetNdivisions(505);
  varFrame->GetYaxis()->SetNdivisions(505);

  double epsilon=1.e-8;

  varFrame->GetYaxis()->SetRangeUser(-0.2-epsilon, 0.2);

  std::string canvasName = varName + "_canvas";
  TCanvas tcan(canvasName.c_str());
  tcan.cd();

  varFrame->Draw();

  std::string plotFileName=varName + "As.eps";
  tcan.Print(plotFileName.c_str());

  return varFrame;
}


int makePlots(int seed=0){
  cout << "simple Dalitz: setting random seeds to " << seed << endl;
  RooRandom::randomGenerator()->SetSeed(seed);
  gRandom->SetSeed(seed*10);

  RooRealVar dummy("dummy", "dummy", 1);

  int wasteFactor = 2500;
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



  double rB_dble=0.10;
  double delta_dble=130;
  double gamma_dble=60;

  
  RooRealVar rB("rB", "rB", rB_dble, 0, 0.15);
  RooRealVar delta("delta", "delta", delta_dble, -180, 180);
  RooRealVar gamma("gamma", "gamma", gamma_dble, -180, 180);
  

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

  TFile fin("addedData.root");
  //  TFile fin("someData.root");
  //  TFile fin("oneData.root");
  //TFile fin("fewData.root");
  //  TFile fin("veryFewData.root");

  cout << "opened file." << endl;
  RooDataSet* dataPlus  = (RooDataSet*) fin.Get("plus");
  cout << " read dataPlus " << (dataPlus ? "OK" : "NULL POINTER") <<endl;
  dataPlus->Print("v");
  RooDataSet* dataMinus = (RooDataSet*) fin.Get("minus");
  cout << " read dataMinus " << (dataMinus ? "OK" : "NULL POINTER") <<endl;
  dataMinus->Print("v");
  cout << "merging them into data. 1st step, cloning" << endl;

  RooDataSet* data = (RooDataSet*) dataPlus->Clone();

  cout << "2nd step: appending" << endl;
  data->append(*dataMinus);
  data->Print("v");
  cout << "done merging" << endl;


  cout << "Event numbers: "
       << "\n dataPlus:  " << dataPlus->numEntries()
       << "\n dataMinus: " << dataMinus->numEntries()
       << "\n all:       " << data->numEntries()
       << endl;
  /*
  //  RooDataSet* dataMinus = phaseSpace.generate(RooArgSet(m12sq, m23sq), 1000) 
  RooDataSet* dataPlus = DaPlusPDF.generate(RooArgSet(m12sq, m23sq), Nevents);
  cout << "generated B+" << endl;
  RooDataSet* dataMinus = DaMinusPDF.generate(RooArgSet(m12sq, m23sq), Nevents);
  cout << "generated B-" << endl;
  */

  cout << "making ph2Plus histogram" << endl;

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
  int nbins = 40;
 
  RooPlot* t01Frame = plotMsq(dataPlus, dataMinus
			      , t01
			      , nbins
			      , "GeV^{2} "
			      , "t_{01} [GeV^{2} ]");//t01.frame();
  cout << "made t01Frame" << endl;

  RooPlot* s12Frame = plotMsq(dataPlus, dataMinus
			      , s12
			      , nbins
			      , "GeV^{2} "
			      , "s_{12} [GeV^{2} ]");
  cout << "made s12Frame" << endl;
  RooPlot* s23Frame = plotMsq(dataPlus, dataMinus
			      , s23
			      , nbins
			      , "GeV^{2} "
			      , "s_{23} [GeV^{2} ]");
  cout << "made s23Frame" << endl;
  RooPlot* s34Frame = plotMsq(dataPlus, dataMinus
			      , s34
			      , nbins
			      , "GeV^{2} "
			      , "s_{34} [GeV^{2} ]");
  cout << "made s34Frame" << endl;
  RooPlot* t40Frame = plotMsq(dataPlus, dataMinus
			      , t40
			      , nbins
			      , "GeV^{2} "
			      , "t_{40} [GeV^{2} ]");
  cout << "made t40Frame" << endl;


  return 0;

  RooPlot* t01FrameAs = plotAsy(data, bcharge
				, t01
				, nbins
				, "GeV^{2} "
				, "t_{01} [GeV^{2} ]");

  RooPlot* s12FrameAs = plotAsy(data, bcharge
				, s12
				, nbins
				, "GeV^{2} "
				, "s_{12} [GeV^{2} ]");
 
  RooPlot* s23FrameAs = plotAsy(data, bcharge
				, s23
				, nbins
				, "GeV^{2} "
				, "s_{23} [GeV^{2} ]");
 
  RooPlot* s34FrameAs = plotAsy(data, bcharge
				, s34
				, nbins
				, "GeV^{2} "
				, "s_{34} [GeV^{2} ]");
 
  RooPlot* t40FrameAs = plotAsy(data, bcharge
				, t40
				, nbins
				, "GeV^{2} "
				, "t_{40} [GeV^{2} ]");
 
  
  cout << "made all the plots. " << endl;
  TFile fHisto("DalitzHisto.root", "RECREATE");
  fHisto.cd();
  ph2Plus->Write();
  t01Frame->Write();
  s12Frame->Write();
  s23Frame->Write();
  s34Frame->Write();
  t40Frame->Write();
  t01FrameAs->Write();
  s12FrameAs->Write();
  s23FrameAs->Write();
  s34FrameAs->Write();
  t40FrameAs->Write();
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

  return 0; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

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
  simPdf.attachDataSet(*data);
  
  /*
  if(fast){
    simPdf.fitTo(*data, Optimize(false), Minos(false), Hesse(false));
  }else{
    simPdf.fitTo(*data, Optimize(false));
  }
  */
  cout << "Done the fit. Drawing logL" << endl;
  

  double rB_fit= rB_dble;
  double d_fit=  delta_dble;
  double g_fit=  gamma_dble;

  //cout << "testing testing" << endl;
  //cout << (result->floatParsFinal().find("rB")) << endl;

  RooAbsReal* rB_fit_ptr= 0;//((RooAbsReal*) (result->floatParsFinal().find("rB")));
  if(rB_fit_ptr){
    rB_fit = rB_fit_ptr->getVal();
  }

  RooAbsReal* d_fit_ptr= 0;// ((RooAbsReal*) (result->floatParsFinal().find("delta")));
  if(d_fit_ptr){
    d_fit = d_fit_ptr->getVal();
  }
  
  RooAbsReal* g_fit_ptr = 0;// ((RooAbsReal*) (result->floatParsFinal().find("gamma")));
  if(g_fit_ptr){
    g_fit = g_fit_ptr->getVal();
  }

  cout << "just got fit results as doubles. They are:"
       << "\n rB = " << rB_fit
       << "\n delta = " << d_fit
       << "\n gamma = " << g_fit
       << endl;


  std::string newName="simPdfsClone";
  RooAbsPdf* pdf_clone = (RooAbsPdf*) simPdf.clone(newName.c_str());
  pdf_clone->attachDataSet(*data);
  
  bool doHandLik=true;

  if((!just2D) && doHandLik){
    cout << "doing 1-D by hand" << endl;
    int nhbins = 37;//361;
    double hmin=-180, hmax=180;
    double bw=(hmax-hmin)/((double)nhbins);
    TH1F gammaScan("gammaScan", "gammaScan", nhbins, hmin, hmax);
    TH1F deltaScan("deltaScan", "deltaScan", nhbins, hmin, hmax);

    double rbmin=0, rbmax=0.15;
    double rbbins=100;
    double rbbw=(rbmax-rbmin)/((double)rbbins);

    TH1F rBScan("rBScan", "deltaScan", rbbins, rbmin, rbmax);


        
    rB.setVal(rB_fit);
    delta.setVal(d_fit);
    for(int bin=1; bin <= nhbins; bin++){
      double g = (((double) bin) - 0.5)*bw + hmin;
      gamma.setVal(g);
      cout << "gamma = " << g << endl;// " -ll = ";
      for(int i=0; i < data->numEntries();  i++){
	data->get(i);
	double ll = -pdf_clone->getLogVal(&mset);
	//	cout << ll << ", ";
	gammaScan.Fill(g, ll);
      }
      //      cout << endl;
    }

    rB.setVal(rB_fit);
    gamma.setVal(g_fit);
    for(int bin=1; bin <= nhbins; bin++){
      double d = (((double) bin) - 0.5)*bw + hmin;
      delta.setVal(d);
      cout << "delta = " << d << endl;//" -ll = ";
      for(int i=0; i < data->numEntries();  i++){
	data->get(i);
	double ll = -pdf_clone->getLogVal(&mset);
	//	cout << ll << ", ";
	deltaScan.Fill(d, ll);
      }
      //      cout << endl;
    }


    delta.setVal(d_fit);
    gamma.setVal(g_fit);
    for(int bin=1; bin <= rbbins; bin++){
      double r = (((double) bin) - 0.5)*rbbw + rbmin;
      rB.setVal(r);
      cout << "rB = " << r << endl;//" -ll = ";
      for(int i=0; i < data->numEntries();  i++){
	data->get(i);
	double ll = -pdf_clone->getLogVal(&mset);
	//	cout << ll << ", ";
	rBScan.Fill(r, ll);
      }
      //      cout << endl;
    }


    gammaScan.Draw();
    tcan.Print("gammaScan.eps");
    deltaScan.Draw();
    tcan.Print("deltaScan.eps");
  
    rBScan.Draw();
    tcan.Print("rBScan.eps");
    
    TFile fscans1D("scans1D.root", "RECREATE");
    fscans1D.cd();
    gammaScan.Write();
    deltaScan.Write();
    rBScan.Write();
    fscans1D.Close();
  }

  bool doHandLik2d=true;
  if(doHandLik2d){
    cout << "doing 2-D by hand" << endl;

    std::string newName="simPdfCloneFor2D";
    RooAbsPdf* pdf_clone2D = (RooAbsPdf*) simPdf.clone(newName.c_str());
    pdf_clone2D->attachDataSet(*data);

    /*
    int nhbins = 18+1; // more bins next time!!
    double hmin=-180, hmax=180;
    double bw=(hmax-hmin)/((double)nhbins);

    TH2F gammaDelta2D("gammaDelta2D", "gammaDelta2D"
		    , nhbins, hmin, hmax
		    , nhbins, hmin, hmax);
    
    rB.setVal(rB_fit);

    for(int binx=1; binx <= nhbins; binx++){
      double g = (((double) binx) - 0.5)*bw + hmin;
      gamma.setVal(g);
      cout << " gamma = " << g << endl;
      for(int biny=1; biny<=nhbins; biny++){
	double d = (((double) biny) - 0.5)*bw + hmin;
	delta.setVal(d);
	cout << " delta " << d << endl;
	for(int i=0; i < data->numEntries();  i++){
	  data->get(i);
	  double ll = -(pdf_clone2D->getLogVal(&mset));
	  gammaDelta2D.Fill(g, d, ll);
	}
      }
    }

    gammaDelta2D.Draw();
    tcan.Print("gammaDelta2D.eps");
    TFile fgammaDelta2D("gammaDelta2D.root", "RECREATE");
    fgammaDelta2D.cd();
    gammaDelta2D.Write();
    fgammaDelta2D.Close();
   
    */
    // ==============================================
    cout << "doing rB-gamma 2D Zoom by hand" << endl;
    int nhbinsX = 20;
    double hminX=20, hmaxX=100;
    double bwX=(hmaxX-hminX)/((double)nhbinsX);
    int nhbinsY = 20;
    double hminY=0.05, hmaxY=0.15;
    double bwY=(hmaxY-hminY)/((double)nhbinsY);

    /*
    TH2F gammaRB2DZoom("gammaRB2DZoom", "gammaRB2DZoom"
		   , nhbinsX, hminX, hmaxX
		   , nhbinsY, hminY, hmaxY);
    

    delta.setVal(d_fit);

    for(int binx=1; binx <= nhbinsX; binx++){
      double g = (((double) binx) - 0.5)*bwX + hminX;
      gamma.setVal(g);
      for(int biny=1; biny<=nhbinsY; biny++){
	double r = (((double) biny) - 0.5)*bwY + hminY;
	rB.setVal(r);
	for(int i=0; i < data->numEntries();  i++){
	  data->get(i);
	  double ll = -(pdf_clone2D->getLogVal(&mset));
	  gammaRB2DZoom.Fill(g, r, ll);
	}
      }
    }
    gammaRB2DZoom.Draw();
    tcan.Print("gammaRB2DZoom.eps");
    TFile fgammaRB2DZoom("gammaRB2DZoom.root", "RECREATE");
    fgammaRB2DZoom.cd();
    gammaRB2DZoom.Write();
    fgammaRB2DZoom.Close();
    */

    // ==============================================
    cout << "doing rB-gamma 2D by hand" << endl;
    nhbinsX = 18+1;
    hminX=-180, hmaxX=180;
    bwX=(hmaxX-hminX)/((double)nhbinsX);
    nhbinsY = 20;
    hminY=0.05, hmaxY=0.15;
    bwY=(hmaxY-hminY)/((double)nhbinsY);
    TH2F gammaRB2D("gammaRB2D", "gammaRB2D"
		   , nhbinsX, hminX, hmaxX
		   , nhbinsY, hminY, hmaxY);
    

    delta.setVal(d_fit);

    for(int binx=1; binx <= nhbinsX; binx++){
      double g = (((double) binx) - 0.5)*bwX + hminX;
      gamma.setVal(g);
      for(int biny=1; biny<=nhbinsY; biny++){
	double r = (((double) biny) - 0.5)*bwY + hminY;
	rB.setVal(r);
	for(int i=0; i < data->numEntries();  i++){
	  data->get(i);
	  double ll = -(pdf_clone2D->getLogVal(&mset));
	  gammaRB2D.Fill(g, r, ll);
	}
      }
    }
    gammaRB2D.Draw();
    tcan.Print("gammaRB2D.eps");
    TFile fgammaRB2D("gammaRB2D.root", "RECREATE");
    fgammaRB2D.cd();
    gammaRB2D.Write();
    fgammaRB2D.Close();
    
    // ==============================================
    cout << "doing rB-delta 2D by hand" << endl;
    nhbinsX = 36+1;
    hminX=-180, hmaxX=180;
    bwX=(hmaxX-hminX)/((double)nhbinsX);
    nhbinsY = 40;
    hminY=0.5; hmaxY=0.15;
    bwY=(hmaxY-hminY)/((double)nhbinsY);
    TH2F deltaRB2D("deltaRB2D", "deltaRB2D"
		   , nhbinsX, hminX, hmaxX
		   , nhbinsY, hminY, hmaxY);
    
    
    gamma.setVal(d_fit);
    
    for(int binx=1; binx <= nhbinsX; binx++){
      double d = (((double) binx) - 0.5)*bwX + hminX;
      delta.setVal(d);
      for(int biny=1; biny<=nhbinsY; biny++){
	double r = (((double) biny) - 0.5)*bwY + hminY;
	rB.setVal(r);
	for(int i=0; i < data->numEntries();  i++){
	  data->get(i);
	  double ll = -(pdf_clone2D->getLogVal(&mset));
	  deltaRB2D.Fill(d, r, ll);
	}
      }
    }
    deltaRB2D.Draw();
    tcan.Print("deltaRB2D.eps");
    TFile fdeltaRB2D("deltaRB2D.root", "RECREATE");
    fdeltaRB2D.cd();
    deltaRB2D.Write();
    fdeltaRB2D.Close();
  }



  // doing the 1D stuff with built-in utilities:
  RooPlot* llframe = gamma.frame(-180, 180, 37);
  /*
  if(! noLLPlot){
    cout << "calling simPdf.plotNLLOn(llframe, data)" << endl;
    simPdf.plotNLLOn(llframe, data);
    cout << "did that." << endl;
    llframe->Draw();
    tcan.Print("ll.eps");
    
    TFile fLL("DalitzLL.root", "RECREATE");
    fLL.cd();
    llframe->Write();
    //    ll_2D->Write();
    fLL.Close();
  }

  */

  /*
  cout << "making nll variable" << endl;
  RooNLLVar nLL("nll", "nll", simPdf, *data);

  cout << "creating histogram" << endl;
  TH2* ll_2D = (TH2*) nLL.createHistogram("nLL2D"
					  , gamma, Binning(36, -180, 180)
					  , YVar( rB, Binning(30, 0, 0.3))
					  );
  cout << "created histo, now drawing it" << endl;
  ll_2D->Draw();
  tcan.Print("ll_2D.eps");

  cout << "creating histogram" << endl;
  TH2* ll_2D_gammaDelta = (TH2*) nLL.createHistogram("nLL2D_gammaDelta"
					  , gamma, Binning(36, -180, 180)
					  , YVar( delta, Binning(36, -180, 180))
					  );
  ll_2D_gammaDelta->Draw();
  tcan.Print("ll_2D_gammaDelta.eps");
  */

  return 0;

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
  tcan.Print("t01_fit.eps");
  DaMinusPDF.plotOn(t01Frame, LineColor(2), ProjWData(*dataMinus));
  t01Frame->Draw();
  tcan.Print("t01_fit.eps");
  
  /*
  phaseSpace.plotOn(s12Frame, LineColor(4),ProjWData(*dataPlus));
  s12Frame->Draw();
  tcan.Print("s12_fit.eps");
  */

  DaPlusPDF.plotOn(s12Frame, ProjWData(*dataPlus));
  s12Frame->Draw();
  tcan.Print("s12_fit.eps");
  DaMinusPDF.plotOn(s12Frame, LineColor(2), ProjWData(*dataMinus));
  s12Frame->Draw();
  tcan.Print("s12_fit.eps");

  DaPlusPDF.plotOn(s23Frame, ProjWData(*dataPlus));
  s23Frame->Draw();
  tcan.Print("s23_fit.eps");
  DaMinusPDF.plotOn(s23Frame, LineColor(2), ProjWData(*dataMinus));
  s23Frame->Draw();
  tcan.Print("s23_fit.eps");

  DaPlusPDF.plotOn(s34Frame, ProjWData(*dataPlus));
  s34Frame->Draw();
  tcan.Print("s34_fit.eps");
  DaMinusPDF.plotOn(s34Frame, LineColor(2), ProjWData(*dataMinus));
  s34Frame->Draw();
  tcan.Print("s34_fit.eps");
    
  DaPlusPDF.plotOn(t40Frame, ProjWData(*dataPlus));
  t40Frame->Draw();
  tcan.Print("t40_fit.eps");
  DaMinusPDF.plotOn(t40Frame, LineColor(2), ProjWData(*dataMinus));
  t40Frame->Draw();
  tcan.Print("t40_fit.eps");
    

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
  //  llPlusframe->Write();
  //  llMinusframe->Write();

  fFit.Close();

  return 0;
}
//
//

