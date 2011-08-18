#ifndef SIMPLEDALITZKKK_HH
#define SIMPLEDALITZKKK_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT

#include <ctime>
#include <cmath>

#include "DKsKKResonances.h"

#include "RooM13.h"
#include "Roo3BodyPhaseSpace.h"

#include "ComplexSum.h"
#include "ComplexProd.h"

#include "RooDalitz.h"

#include "dataThrowerAway.h"

#include "mark.h"

#include "RooGlobalFunc.h"

#include "RooGaussian.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooArgList.h"
#include "RooFormulaVar.h"
#include "RooSimultaneous.h"
#include "RooAddPdf.h"


#include "TH2.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"


//#include "MatrixElement.h"
using namespace DKsKKResonances;

using namespace RooFit;

void simpleDalitzKKK();


void simpleDalitzKKK(){
  RooRealVar dummy("dummy", "dummy", 1);
  RooRealVar one("one", "one", 1);
  RooRealVar pi("pi", "pi", TMath::Pi());

  double eps = 0.1;
  
  Double_t PrimDzMass = 1.8645;
  Double_t deg = TMath::Pi()/180.0;

  //  RooRealVar DZeroM   ("DZeroM" , "DZeroMass", PrimDzMass);
  //  RooRealVar piPlusM   ("piPlusM" , "piMass", 0.13957018);
  //  RooRealVar piMinusM  ("piMinusM", "piMass", 0.13957018);
  //  RooRealVar KsM  ("KsM", "KsMass", 0.497648);

  RooRealVar Radius_r ("Radius_r", "Radius_r", 1.5);
  double minM12 = (KsM.getVal() + KMinusM.getVal());
  double minM12sq = minM12*minM12;
  double minM23 = (KsM.getVal() + KPlusM.getVal());
  double minM23sq = minM23*minM23;
  double maxM12 = (DZeroM.getVal() - KMinusM.getVal());
  double maxM12sq = maxM12*maxM12;
  double maxM23 = (DZeroM.getVal() - KPlusM.getVal());
  double maxM23sq = maxM23*maxM23;

  RooRealVar m12sq("m12sq", "m12sq", minM12sq *0, maxM12sq*1.1);
  RooRealVar m23sq("m23sq", "m23sq", minM23sq *0, maxM23sq*1.1);

  RooM13 m13sq("m13sq", "m13sq"
               , m12sq, m23sq, DZeroM, KPlusM, KsM, KMinusM); 
  RooM13 m13Swsq("m13sqSwapped", "m13sqSwapped"
               , m23sq, m12sq, DZeroM, KPlusM, KsM, KMinusM); 

  RooArgSet mset(m12sq, m23sq);
  RooArgList mlist(m12sq, m23sq);

  RooCategory bcharge("bcharge","bcharge") ;
  bcharge.defineType("B+",+1);
  bcharge.defineType("B-",-1); 
  bcharge.Print("s") ;
  
  bcharge.setIndex(1);
  bcharge.Print("v");
  bcharge.setIndex(1);
  bcharge.Print("v");



  cout << "declaring phaseSpace " << endl;
  Roo3BodyPhaseSpace phaseSpace("phaseSpace", "phaseSpace"
                                , m12sq, m23sq
                                , DZeroM, KPlusM, KsM, KMinusM
                                );
  cout << "phaseSpace declared " << endl;
  cout << "Now declaring 1st ResonanceList" << endl;

  RooArgList ResonanceList(makeResonance(m12sq, m23sq, m13sq));
  RooArgList ResonanceListSwapped(makeResonance(m23sq, m12sq
                                                , m13Swsq
                                                , "swapped"));
  cout << "done that" << endl;

  RooRealVar rB("rB", "rB", 0.1, 0.00, 0.5);

  RooRealVar delta("delta", "delta", 130, -180, 180);
  RooRealVar gamma("gamma", "gamma", 60, -180, 180);

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
		     , ResonanceListSwapped);
  ComplexSum BMinus2("BMinus2", "BMinus2"
		     , ResonanceList, rB, BMinusPhase);



  RooArgList lplus(BPlus1, BPlus2);
  RooArgList lmin(BMinus1, BMinus2);

  ComplexSum BPlus("BPlus", "BPlus", lplus);
  ComplexSum BMinus("BMinus", "BMinus", lmin);

  ComplexSum D("D", "D", ResonanceList);
  ComplexSum Dbar("Dbar", "Dbar", ResonanceListSwapped);
  RooArgList prodl(D, Dbar);
  ComplexProd Product("prod", "prod", prodl);

  double minx=0.8, maxx=maxM12sq*1.05;
  int nbx = 200;
  double bwx = (maxx-minx)/((double)nbx);
  double miny=0.8, maxy=maxM23sq*1.05;
  int nby = 200;
  double bwy = (maxy-miny)/((double)nby);

  TH1F HDKK("HDKK","",100,0.95,1.95);
  

  
  TH2F HD("HD", "HD", nbx, minx, maxx, nby, miny, maxy);
  TH2F HDbar("HDbar", "HDbar", nbx, minx, maxx, nby, miny, maxy);
  TH2F HDCP_E("HDCP_E", "HDCP_E", nbx, minx, maxx, nby, miny, maxy);
  TH2F HDCP_O("HDCP_O", "HDCP_O", nbx, minx, maxx, nby, miny, maxy);
  TH2F HProduct("HProd", "HDProd", nbx, minx, maxx, nby, miny, maxy);

  TH2F HBPlus("HBPlus", "HBPlus", nbx, minx, maxx, nby, miny, maxy);
  TH2F HBMinus("HBMinus", "HBMinus", nbx, minx, maxx, nby, miny, maxy);

  

  for(int ix=1; ix<=nbx; ix++){
    double x=minx + ix*bwx - bwx/2;
    m12sq.setVal(x);
    for(int iy=1; iy<=nby; iy++){
      double y=miny + iy*bwy - bwy/2;
      m23sq.setVal(y);


      if(phaseSpace.getVal() > 0){
        double d_even = norm(D.getCVal() + Dbar.getCVal());
        double d_odd  = norm(D.getCVal() - Dbar.getCVal());
        Double_t mkksq = pow(1.8645,2)+2.0*pow(0.493677,2)+pow(0.4976,2)-x-y;
        
        HDKK.Fill(mkksq,D.getVal()*phaseSpace.getVal()/10.0);
        
        HD.Fill(x, y, D.getVal()*phaseSpace.getVal()/10.0);
        HDbar.Fill(x, y, Dbar.getVal()*phaseSpace.getVal()/10.0);
        HProduct.Fill(x, y, Product.getCVal().real()*phaseSpace.getVal()/10.0);
        HDCP_E.Fill(x, y, d_even*phaseSpace.getVal()/10.0);
        HDCP_O.Fill(x, y, d_odd*phaseSpace.getVal()/10.0);
        
        HBPlus.Fill(x, y, BPlus.getVal()*phaseSpace.getVal()/10.0);
        HBMinus.Fill(y, x, BMinus.getVal()*phaseSpace.getVal()/10.0);
      }
    }
  }
  TFile fd("fd.root", "RECREATE");
  fd.cd();
  HDKK.Write();
  
  HD.Write();
  HDbar.Write();
  HProduct.Write();
  HDCP_E.Write();
  HDCP_O.Write();
  HBPlus.Write();
  HBMinus.Write();
  
  fd.Close();
  cout << "drawn a few histos" << endl;
  //  return;


  RooDalitz DaPlusPDF("DalitzPlus", "DalitzPlus"
		      , BPlus, phaseSpace, mlist);
  RooDalitz DaMinusPDF("DalitzMinus", "DalitzMinus"
		       , BMinus, phaseSpace, mlist);

  RooDalitz DpiPlusPDF("DpiPlus", "DpiPlus"
		      , BPlus1, phaseSpace, mlist);
  RooDalitz DpiMinusPDF("DpiMinus", "DpiMinus"
		       , BMinus1, phaseSpace, mlist);

  time_t t7 = time(NULL);

  int Nevents = 6800;
  cout << "Now generating the full thing" << endl;

  RooDataSet* dataPlus  = phaseSpace.generate(mset, 10*Nevents);

  cout << "generated phase-space events." << endl;

  dataPlus = dataThrowerAway(BPlus, mlist, dataPlus);
  bcharge.setIndex(1);
  dataPlus = mark(dataPlus, bcharge);
  dataPlus->Print("v");
  cout << "after dataThrowerAway I have " 
       << dataPlus->numEntries() 
       << " in dataPlus " << endl;

  RooDataSet* dataMinus  = phaseSpace.generate(mlist, 10*Nevents);
  dataMinus = dataThrowerAway(BMinus, mlist, dataMinus);
  bcharge.setIndex(-1);
  dataMinus = mark(dataMinus, bcharge);
  cout << "after dataThrowerAway I have " 
       << dataMinus->numEntries() 
       << " in dataMinus " << endl;
    //  return 0;

  RooDataSet* data = (RooDataSet*) dataPlus->Clone();
  data->append(*dataMinus);
  data->Print("v");

  //add some background 
  RooDataSet* dpiPlusBkg = phaseSpace.generate(mset,2.8*Nevents);
  dpiPlusBkg = dataThrowerAway(BPlus1, mlist, dpiPlusBkg);
  bcharge.setIndex(1);
  dpiPlusBkg = mark(dpiPlusBkg,bcharge);
  data->append(*dpiPlusBkg);

  RooDataSet* dpiMinusBkg = phaseSpace.generate(mset,2.8*Nevents);
  dpiMinusBkg = dataThrowerAway(BMinus1, mlist, dpiMinusBkg);
  bcharge.setIndex(-1);
  dpiMinusBkg = mark(dpiMinusBkg,bcharge);
  data->append(*dpiMinusBkg);

  
  
  TFile fData("DKpipi1k.root", "RECREATE");
  fData.cd();
  dataPlus->Write();
  dataMinus->Write();
  fData.Close();

  TH2* ph2Plus = m23sq.createHistogram("m23sqVSm12sqPlus"
                                       , m12sq
                                       );
  TH2* ph2Minus = m23sq.createHistogram("m23sqVSm12sqMinus"
                                        , m12sq
                                        );
  dataPlus->fillHistogram(ph2Plus, RooArgList(m12sq, m23sq));
  dataMinus->fillHistogram(ph2Minus, RooArgList(m23sq, m12sq));
  TCanvas tcan;

  gStyle->SetOptStat(0);
  
  ph2Plus->Draw();
  tcan.Print("bplus.eps");
  ph2Minus->Draw();
  tcan.Print("bminus.eps");

  TH2* diff = (TH2*) ph2Plus->Clone("diff");
  diff->Add(ph2Minus, -1.);

  int nbins = Nevents/50;
  if(nbins > 100)nbins=100;

  RooPlot* m12Frame = m12sq.frame();
  dataPlus->plotOn(m12Frame
                   );//, Binning(m12sq.getMin(), m12sq.getMax(), nbins));
  //  m12FramePlus->Draw();
  //  RooPlot* m12FrameMinus = m12sq.frame();
  //m12FrameMinus->SetMarkerColor(2);
  dataMinus->plotOn(m12Frame, MarkerColor(2), LineColor(2)
                    );//, Binning(m12sq.getMin(), m12sq.getMax(), nbins));
  m12Frame->Draw();
  tcan.Print("m12.eps");
 
  RooPlot* m23Frame = m23sq.frame();
  dataPlus->plotOn(m23Frame
                   );// , Binning(m23sq.getMin(), m23sq.getMax(), nbins));
  dataMinus->plotOn(m23Frame, MarkerColor(2), LineColor(2)
                    );// , Binning(m23sq.getMin(), m23sq.getMax(), nbins));
  m23Frame->Draw();
  tcan.Print("m23.eps");

  std::string newName = "Bplusm13sqCopy";
      
  RooM13* Bplusm13sqCopy = (RooM13*) m13sq.clone(newName.c_str());
  Bplusm13sqCopy->attachDataSet(*dataPlus);
  TH1F* hBplusm13sq = 
    new TH1F("hBplusm13sq","B^{+} #pi^{+}#pi^{-} invariant mass",100,0.0,2.0);  
  for(int i=0; i< dataPlus->numEntries(); i++){
    const RooArgSet* row = dataPlus->get(i);
    double val = Bplusm13sqCopy->getVal();
    hBplusm13sq->Fill(val);
  }
  
  newName = "Bminusm13sqCopy";
  RooM13* Bminusm13sqCopy = (RooM13*) m13sq.clone(newName.c_str());
  Bminusm13sqCopy->attachDataSet(*dataMinus);
  TH1F* hBminusm13sq = 
    new TH1F("hBminusm13sq","B^{+} #pi^{+}#pi^{-} invariant mass",100,0.0,2.0);
  for(int i=0; i< dataMinus->numEntries(); i++){
    const RooArgSet* row = dataMinus->get(i);
    double val = Bminusm13sqCopy->getVal();
    hBminusm13sq->Fill(val);
  }
      
  hBminusm13sq->SetMarkerStyle(21);
  hBminusm13sq->SetMarkerColor(kRed);
  hBplusm13sq->SetMarkerStyle(21);
  hBplusm13sq->SetMarkerColor(kBlack);
  hBminusm13sq->Draw("ep");
  hBplusm13sq->Draw("epsame");
  tcan.Print("m13.eps");

  
  TFile fHisto("DalitzHisto.root", "RECREATE");
  fHisto.cd();
  ph2Plus->Write();
  m12Frame->Write();
  m23Frame->Write();
  ph2Minus->Write();
  diff->Write();
  fHisto.Close();


  cout << "now doing the fit to Bplus:" << endl;
  DaPlusPDF.constOptimize(RooAbsArg::DeActivate) ;
  DaMinusPDF.constOptimize(RooAbsArg::DeActivate) ;
  DpiPlusPDF.constOptimize(RooAbsArg::DeActivate) ;
  DpiMinusPDF.constOptimize(RooAbsArg::DeActivate) ;
  //  DaPlusPDF.useRooFitsIntegrator();
  //  DaMinusPDF.useRooFitsIntegrator();
  phaseSpace.constOptimize(RooAbsArg::DeActivate) ;
  


  time_t t8 = time(NULL);

  RooSimultaneous simPdf("simPdf", "simPdf", bcharge);
  simPdf.addPdf(DaPlusPDF, "B+");
  simPdf.addPdf(DaMinusPDF, "B-");
  

  RooSimultaneous simDpiPdf("simDpiPdf", "simDpiPdf", bcharge);
  simDpiPdf.addPdf(DpiPlusPDF, "B+");
  simDpiPdf.addPdf(DpiMinusPDF, "B-");

  RooRealVar fsig("fsig","signal fraction",0.5,0.0,1.0);

  
  
  RooAddPdf sigPlusBkg("sigPlusBkg","sigPlusBkg",RooArgList(simPdf,simDpiPdf),RooArgList(fsig));
  


  //  simPdf.fitTo(*data, Optimize(false));

  //  sigPlusBkg.fitTo(*data,Optimize(false));
  

  //  DaPlusPDF.fitTo(*dataPlus, Optimize(false));
  time_t t9 = time(NULL);
  cout << "fitting Bplus took " 
       << difftime(t9, t8) << " s" << endl;


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
  //  DaPlusPDF.plotNLLOn(llPlusframe, dataPlus);
  llPlusframe->Draw();
  tcan.Print("llPlus.eps");


  cout << "now doing the fit to Bminus:" << endl;
  DaMinusPDF.constOptimize(RooAbsArg::DeActivate) ;
  //  DaMinusPDF.fitTo(*dataMinus, Optimize(false));
  time_t t10 = time(NULL);
 
  cout << " done the fit to Bminus That took" 
       << difftime(t10, t9) << " s" << endl;
  RooPlot* llMinusframe = gamma.frame(-180, 180, 100);
  //  DaMinusPDF.plotNLLOn(llMinusframe, dataMinus);
  llMinusframe->Draw();
  tcan.Print("llMinus.eps");

  char* zAxisLabel = "Dalitz";
  Double_t xlo=0, xhi=5;
  Int_t nBins=100;
  
  TFile fLL("fLL.root", "RECREATE");
  fLL.cd();
  llPlusframe->Write();
  llMinusframe->Write();
  fLL.Close();
  
  return;
}
#endif
//
//

