#ifndef SIMPLEDALITZ3_HH
#define SIMPLEDALITZ3_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT

#include <ctime>
#include <cmath>

#include "DKsPiPiResonances.h"

#include "RooM13.h"
#include "Roo3BodyPhaseSpace.h"

#include "ComplexSum.h"

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
#include "RooRandom.h"

#include "TH2.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"

//#include "MatrixElement.h"
using namespace DKsPiPiResonances;

using namespace RooFit;

void simpleDalitz3(int seed = 1){

  cout << "simple Dalitz: setting random seeds to " << seed << endl;
  RooRandom::randomGenerator()->SetSeed(seed);
  gRandom->SetSeed(seed*10);

  RooRealVar dummy("dummy", "dummy", 1);
  double eps = 0.1;
  
  Double_t PrimDzMass = 1.8645;
  Double_t deg = TMath::Pi()/180.0;

  //  RooRealVar DZeroM   ("DZeroM" , "DZeroMass", PrimDzMass);
  //  RooRealVar piPlusM   ("piPlusM" , "piMass", 0.13957018);
  //  RooRealVar piMinusM  ("piMinusM", "piMass", 0.13957018);
  //  RooRealVar KsM  ("KsM", "KsMass", 0.497648);

  RooRealVar Radius_r ("Radius_r", "Radius_r", 1.5);
  double minM12 = (KsM.getVal() + piMinusM.getVal());
  double minM12sq = minM12*minM12;
  double minM23 = (KsM.getVal() + piPlusM.getVal());
  double minM23sq = minM23*minM23;
  double maxM12 = (DZeroM.getVal() - piMinusM.getVal());
  double maxM12sq = maxM12*maxM12;
  double maxM23 = (DZeroM.getVal() - piPlusM.getVal());
  double maxM23sq = maxM23*maxM23;

  RooRealVar m12sq("m12sq", "m12sq", minM12sq *0, maxM12sq*1.1);
  RooRealVar m23sq("m23sq", "m23sq", minM23sq *0, maxM23sq*1.1);

  RooM13 m13sq("m13sq", "m13sq"
               , m12sq, m23sq, DZeroM, piPlusM, KsM, piMinusM); 
  RooM13 m13Swsq("m13sqSwapped", "m13sqSwapped"
               , m23sq, m12sq, DZeroM, piPlusM, KsM, piMinusM); 

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
                                , DZeroM, piPlusM, KsM, piMinusM
                                );
  cout << "phaseSpace declared " << endl;
  cout << "Now declaring 1st ResonanceList" << endl;

  RooArgList ResonanceList(makeResonance(m12sq, m23sq, m13sq));
  RooArgList ResonanceListSwapped(makeResonance(m23sq, m12sq
                                                , m13Swsq
                                                , "swapped"));
  cout << "done that" << endl;

  RooRealVar rB("rB", "rB", 0.1, 0.00,0.5);

  RooRealVar delta("delta", "delta", 130, -180, 180);
  RooRealVar gamma("gamma", "gamma", 60, -180, 180);
  RooRealVar null("null", "null", 0);
  RooRealVar unit("unit", "unit", 1);
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

  RooDalitz DaPlusPDF("DalitzPlus", "DalitzPlus"
		      , BPlus, phaseSpace, mlist);
  RooDalitz DaMinusPDF("DalitzMinus", "DalitzMinus"
		       , BMinus, phaseSpace, mlist);

  RooDalitz DpiPlusPDF("DpiPlus", "DpiPlus"
		      , BPlus1, phaseSpace, mlist);
  RooDalitz DpiMinusPDF("DpiMinus", "DpiMinus"
		       , BMinus1, phaseSpace, mlist);

  RooRealVar NonRes("NonRes","NonRes",1.0);
  

  RooDalitz NonResPlusPDF("NonResPlus", "NonResPlus"
		      , NonRes, phaseSpace, mlist);
  RooDalitz NonResMinusPDF("NonResMinus", "NonResMinus"
		       , NonRes, phaseSpace, mlist);

  time_t t7 = time(NULL);
  //
  TH1F* hBplusm13sq =
    new TH1F("hBplusm13sq","B^{+} #pi^{+}#pi^{-} invariant mass",50,0.0,3.0);  
  TH1F* hBplusm12sq =
    new TH1F("hBplusm12sq","B^{+} K_{S}^{0}#pi^{+} invariant mass",50,0.0,3.0);  
  TH1F* hBplusm23sq =
    new TH1F("hBplusm23sq","B^{+} K_{S}^{0}#pi^{-} invariant mass",50,0.0,3.0);  
  
  TH1F* hBminusm13sq = 
    new TH1F("hBminusm13sq","B^{+} #pi^{+}#pi^{-} invariant mass",50,0.0,3.0);
  TH1F* hBminusm12sq =
    new TH1F("hBminusm12sq","B^{+} K_{S}^{0}#pi^{+} invariant mass",50,0.0,3.0);  
  TH1F* hBminusm23sq =
    new TH1F("hBminusm23sq","B^{+} K_{S}^{0}#pi^{-} invariant mass",50,0.0,3.0);  
  
  TH1F* hDpiplusm13sq =
    new TH1F("hDpiplusm13sq","B^{+} #pi^{+}#pi^{-} invariant mass",100,0.0,2.0);  
  TH1F* hDpiplusm12sq =
    new TH1F("hDpiplusm12sq","B^{+} K_{S}^{0}#pi^{+} invariant mass",150,0.0,3.0);  
  TH1F* hDpiplusm23sq =
    new TH1F("hDpiplusm23sq","B^{+} K_{S}^{0}#pi^{-} invariant mass",150,0.0,3.0);  
  
  TH1F* hDpiminusm13sq = 
    new TH1F("hDpiminusm13sq","B^{+} #pi^{+}#pi^{-} invariant mass",100,0.0,2.0);
  TH1F* hDpiminusm12sq =
    new TH1F("hDpiminusm12sq","B^{+} K_{S}^{0}#pi^{+} invariant mass",150,0.0,3.0);  
  TH1F* hDpiminusm23sq =
    new TH1F("hDpiminusm23sq","B^{+} K_{S}^{0}#pi^{-} invariant mass",150,0.0,3.0);  
  
  TH1F* hCombplusm13sq =
    new TH1F("hCombplusm13sq","B^{+} #pi^{+}#pi^{-} invariant mass",100,0.0,2.0);  
  TH1F* hCombplusm12sq =
    new TH1F("hCombplusm12sq","B^{+} K_{S}^{0}#pi^{+} invariant mass",150,0.0,3.0);  
  TH1F* hCombplusm23sq =
    new TH1F("hCombplusm23sq","B^{+} K_{S}^{0}#pi^{-} invariant mass",150,0.0,3.0);  
  
  TH1F* hCombminusm13sq = 
    new TH1F("hCombminusm13sq","B^{+} #pi^{+}#pi^{-} invariant mass",100,0.0,2.0);
  TH1F* hCombminusm12sq =
    new TH1F("hCombminusm12sq","B^{+} K_{S}^{0}#pi^{+} invariant mass",150,0.0,3.0);  
  TH1F* hCombminusm23sq =
    new TH1F("hCombminusm23sq","B^{+} K_{S}^{0}#pi^{-} invariant mass",150,0.0,3.0);  

  typedef struct
  {
    Float_t rb,erb,gam,egam,del,edel;
  } TOYRESULT;
  static TOYRESULT toyresult;
  TTree* results = new TTree("results","Pull studies results");
  results->Branch("toyresult",&toyresult,"rb:erb:gam:egam:del:edel");

  Int_t MAXPULL = 10;
  int Nevents = 6800;  
  TCanvas tcan;
    


  for (Int_t ipull = 0; ipull<MAXPULL; ipull++){
    rB.setVal(0.15);
    gamma.setVal(60.0);
    delta.setVal(130.0);

    
    cout << "simple Dalitz: setting random seeds to " << seed 
         << " for pull " << ipull << " toy experiment " << endl;
    RooRandom::randomGenerator()->SetSeed(seed*10+ipull);
    gRandom->SetSeed(seed*100+ipull);

    RooDataSet* dataPlus  = phaseSpace.generate(mset, 10*Nevents);
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

    RooDataSet* data = (RooDataSet*) dataPlus->Clone();
    data->append(*dataMinus);
    data->Print("v");

    Bool_t bkg = kFALSE;
  
  
    // add some background 
    RooDataSet* dpiPlusBkg;
    RooDataSet* dcombPlusBkg;
    RooDataSet* dcombwsPlusBkg;
    RooDataSet* dpiMinusBkg;
    RooDataSet* dcombMinusBkg;
    RooDataSet* dcombwsMinusBkg;
    RooDataSet* NonResMinusBkg;
    RooDataSet* NonResPlusBkg;
  
    if (bkg){
      dpiPlusBkg = phaseSpace.generate(mset,2.4*Nevents);
      dpiPlusBkg = dataThrowerAway(BPlus1, mlist, dpiPlusBkg);
      bcharge.setIndex(1);
      dpiPlusBkg = mark(dpiPlusBkg,bcharge);
      //data->append(*dpiPlusBkg);
      
      dcombPlusBkg = phaseSpace.generate(mset,3.5*Nevents);
      dcombPlusBkg = dataThrowerAway(BPlus1, mlist, dcombPlusBkg);
      bcharge.setIndex(1);
      dcombPlusBkg = mark(dcombPlusBkg,bcharge);
      //      data->append(*dcombPlusBkg);
      
      dcombwsPlusBkg = phaseSpace.generate(mset,3.5*Nevents);
      dcombwsPlusBkg = dataThrowerAway(BMinus1, mlist, dcombwsPlusBkg);
      bcharge.setIndex(1);
      dcombwsPlusBkg = mark(dcombwsPlusBkg,bcharge);
      dcombPlusBkg->append(*dcombwsPlusBkg);
      // data->append(*dcombwsPlusBkg);

      NonResPlusBkg = phaseSpace.generate(mset,0.7*Nevents);
      bcharge.setIndex(1);
      NonResPlusBkg = mark(NonResPlusBkg,bcharge);
      data->append(*NonResPlusBkg);


      
      dpiMinusBkg = phaseSpace.generate(mset,2.4*Nevents);
      dpiMinusBkg = dataThrowerAway(BMinus1, mlist, dpiMinusBkg);
      bcharge.setIndex(-1);
      dpiMinusBkg = mark(dpiMinusBkg,bcharge);
      //      data->append(*dpiMinusBkg);

      dcombMinusBkg = phaseSpace.generate(mset,3.5*Nevents);
      dcombMinusBkg = dataThrowerAway(BMinus1, mlist, dcombMinusBkg);
      bcharge.setIndex(-1);
      dcombMinusBkg = mark(dcombMinusBkg,bcharge);
      //      data->append(*dcombMinusBkg);

      dcombwsMinusBkg = phaseSpace.generate(mset,3.5*Nevents);
      dcombwsMinusBkg = dataThrowerAway(BPlus1, mlist, dcombwsMinusBkg);
      bcharge.setIndex(-1);
      dcombwsMinusBkg = mark(dcombwsMinusBkg,bcharge);
      dcombMinusBkg->append(*dcombwsMinusBkg);
      //      data->append(*dcombwsMinusBkg);

      NonResMinusBkg = phaseSpace.generate(mset,0.7*Nevents);
      bcharge.setIndex(-1);
      NonResMinusBkg = mark(NonResMinusBkg,bcharge);
      data->append(*NonResMinusBkg);
    }
  
    
    Double_t fsigval = 1.0;
    Double_t fbkgval1 = 0.0;

    if (bkg) {
      fsigval =
        Double_t(dataMinus->numEntries() + dataPlus->numEntries())/
        Double_t(data->numEntries());
    
      fbkgval1 = 
        //Double_t( (dpiPlusBkg->numEntries() + dpiMinusBkg->numEntries())
        //          +(dcombPlusBkg->numEntries() + dcombMinusBkg->numEntries())
        //          -(dcombwsPlusBkg->numEntries() + dcombwsMinusBkg->numEntries()))
        Double_t( (NonResPlusBkg->numEntries() + NonResMinusBkg->numEntries()))

        /Double_t(data->numEntries());
    }
    cout << fsigval << " " << fbkgval1 << endl;
    

    if (ipull ==0){
      TH2* ph2Plus = m23sq.createHistogram("m23sqVSm12sqPlus", m12sq);
      TH2* ph2Minus = m23sq.createHistogram("m23sqVSm12sqMinus" , m12sq);
      dataPlus->fillHistogram(ph2Plus, RooArgList(m12sq, m23sq));
      dataMinus->fillHistogram(ph2Minus, RooArgList(m23sq, m12sq));

      ph2Plus->Draw();
      tcan.Print("bplus.eps");
      ph2Minus->Draw();
      tcan.Print("bminus.eps");
    
      TH2* diff = (TH2*) ph2Plus->Clone("diff");
      diff->Add(ph2Minus, -1.);
      
      int nbins = Nevents/50;
      if(nbins > 100)nbins=100;
      
      RooPlot* m12Frame = m12sq.frame();
      dataPlus->plotOn(m12Frame);
      dataMinus->plotOn(m12Frame, MarkerColor(2), LineColor(2));
      m12Frame->Draw();
      tcan.Print("m12.eps");
      
      RooPlot* m23Frame = m23sq.frame();
      dataPlus->plotOn(m23Frame);
      dataMinus->plotOn(m23Frame, MarkerColor(2), LineColor(2));
      m23Frame->Draw();
      tcan.Print("m23.eps");

      std::string newName = "Bplusm13sqCopy";
    
      RooM13* Bplusm13sqCopy = (RooM13*) m13sq.clone(newName.c_str());
      Bplusm13sqCopy->attachDataSet(*dataPlus);
      for(int i=0; i< dataPlus->numEntries(); i++){
        const RooArgSet* row = dataPlus->get(i);
        double val = Bplusm13sqCopy->getVal();
        hBplusm13sq->Fill(val);
        val = row->getRealValue("m12sq");
        hBplusm12sq->Fill(val);
        val = row->getRealValue("m23sq");
        hBplusm23sq->Fill(val);
      }

      newName = "Bminusm13sqCopy";
      RooM13* Bminusm13sqCopy = (RooM13*) m13sq.clone(newName.c_str());
      Bminusm13sqCopy->attachDataSet(*dataMinus);
    
      for(int i=0; i< dataMinus->numEntries(); i++){
        const RooArgSet* row = dataMinus->get(i);
        double val = Bminusm13sqCopy->getVal();
        hBminusm13sq->Fill(val);
        val = row->getRealValue("m12sq");
        hBminusm12sq->Fill(val);
        val = row->getRealValue("m23sq");
        hBminusm23sq->Fill(val);
      }

      if (bkg) {
        newName = "Dpiplusm13sqCopy";
        RooM13* Dpiplusm13sqCopy = (RooM13*) m13sq.clone(newName.c_str());
        Dpiplusm13sqCopy->attachDataSet(*dpiPlusBkg);
        
        for(int i=0; i< dpiPlusBkg->numEntries(); i++){
          const RooArgSet* row = dpiPlusBkg->get(i);
          double val = Dpiplusm13sqCopy->getVal();
          hDpiplusm13sq->Fill(val);
          val = row->getRealValue("m12sq");
          hDpiplusm12sq->Fill(val);
          val = row->getRealValue("m23sq");
          hDpiplusm23sq->Fill(val);
        }

        newName = "Dpiminusm13sqCopy";
        RooM13* Dpiminusm13sqCopy = (RooM13*) m13sq.clone(newName.c_str());
        Dpiminusm13sqCopy->attachDataSet(*dpiMinusBkg);

        for(int i=0; i< dpiMinusBkg->numEntries(); i++){
          const RooArgSet* row = dpiMinusBkg->get(i);
          double val = Dpiminusm13sqCopy->getVal();
          hDpiminusm13sq->Fill(val);
          val = row->getRealValue("m12sq");
          hDpiminusm12sq->Fill(val);
          val = row->getRealValue("m23sq");
          hDpiminusm23sq->Fill(val);
        }
        
        newName = "Combplusm13sqCopy";
        RooM13* Combplusm13sqCopy = (RooM13*) m13sq.clone(newName.c_str());
        Combplusm13sqCopy->attachDataSet(*dcombPlusBkg);

        for(int i=0; i< dcombPlusBkg->numEntries(); i++){
          const RooArgSet* row = dcombPlusBkg->get(i);
          double val = Combplusm13sqCopy->getVal();
          hCombplusm13sq->Fill(val);
          val = row->getRealValue("m12sq");
          hCombplusm12sq->Fill(val);
          val = row->getRealValue("m23sq");
          hCombplusm23sq->Fill(val);
        }
  
        newName = "Combminusm13sqCopy";
        RooM13* Combminusm13sqCopy = (RooM13*) m13sq.clone(newName.c_str());
        Combminusm13sqCopy->attachDataSet(*dcombMinusBkg);
        
        for(int i=0; i< dcombMinusBkg->numEntries(); i++){
          const RooArgSet* row = dcombMinusBkg->get(i);
          double val = Combminusm13sqCopy->getVal();
          hCombminusm13sq->Fill(val);
          val = row->getRealValue("m12sq");
          hCombminusm12sq->Fill(val);
          val = row->getRealValue("m23sq");
          hCombminusm23sq->Fill(val);
        }
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
      ph2Minus->Write();
      
      
      hBminusm13sq->Write();
      hBplusm13sq->Write();
      hBminusm12sq->Write();
      hBplusm12sq->Write();
      hBminusm23sq->Write();
      hBplusm23sq->Write();
      hDpiminusm13sq->Write();
      hDpiplusm13sq->Write();
      hDpiminusm12sq->Write();
      hDpiplusm12sq->Write();
      hDpiminusm23sq->Write();
      hDpiplusm23sq->Write();
      hCombminusm13sq->Write();
      hCombplusm13sq->Write();
      hCombminusm12sq->Write();
      hCombplusm12sq->Write();
      hCombminusm23sq->Write();
      hCombplusm23sq->Write();
      fHisto.Close();
    }

    cout << "now doing the fit" << endl;

    DaPlusPDF.constOptimize(RooAbsArg::DeActivate) ;
    DaMinusPDF.constOptimize(RooAbsArg::DeActivate) ;
    DpiPlusPDF.constOptimize(RooAbsArg::DeActivate) ;
    DpiMinusPDF.constOptimize(RooAbsArg::DeActivate) ;
    NonResPlusPDF.constOptimize(RooAbsArg::DeActivate) ;
    NonResMinusPDF.constOptimize(RooAbsArg::DeActivate) ;
    phaseSpace.constOptimize(RooAbsArg::DeActivate) ;

    RooSimultaneous simPdf("simPdf", "simPdf", bcharge);
    simPdf.addPdf(DaPlusPDF, "B+");
    simPdf.addPdf(DaMinusPDF, "B-");
    
    RooSimultaneous simDpiPdf("simDpiPdf", "simDpiPdf", bcharge);
    simDpiPdf.addPdf(DpiPlusPDF, "B+");
    simDpiPdf.addPdf(DpiMinusPDF, "B-");
    
    RooSimultaneous simDcombPdf("simDcombPdf", "simDcombPdf", bcharge);
    simDcombPdf.addPdf(DpiPlusPDF, "B-");
    simDcombPdf.addPdf(DpiMinusPDF, "B+");

    RooSimultaneous simNonResPdf("simNonResPdf", "simNonResPdf", bcharge);
    simNonResPdf.addPdf(NonResMinusPDF, "B-");
    simNonResPdf.addPdf(NonResPlusPDF, "B+");

    RooRealVar fsig("fsig","signal fraction",fsigval);
    RooRealVar fbkg1("fbkg1","D pi  fraction",fbkgval1);

    RooAddPdf sigPlusBkg("sigPlusBkg","sigPlusBkg",RooArgList(simPdf,simNonResPdf),RooArgList(fsig));
    
    sigPlusBkg.fitTo(*data,Optimize(false),Minos(false));

    toyresult.rb = Float_t(rB.getVal());
    toyresult.gam = Float_t(gamma.getVal());
    toyresult.del = Float_t(delta.getVal());
    toyresult.erb = Float_t(rB.getError());
    toyresult.egam = Float_t(gamma.getError());
    toyresult.edel = Float_t(delta.getError());
    
    results->Fill();
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
  }

 
  TFile fResults("KpipiResults.root", "RECREATE");
  fResults.cd();
  results->Write();
  fResults.Close();
  

  return;
}
#endif
//
//

