// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include <ctime>
#include <cstdio>

#include "mark.h"
#include "DKKpipiResonances.h"
#include "RooM13.h"
#include "fourMxy.h"
#include "Roo4BodyPhaseSpace.h"
//#include "Roo3BodyPhaseSpace.h"
#include "ComplexSum.h"
#include "RooDalitz.h"
#include "RooBinomiDalitz.h"
#include "dataThrowerAway.h"
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
#include "RooCategory.h"
#include "TRandom.h"
#include "RooRandom.h"

#include "TH2.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"

//#include "MatrixElement.h"

using namespace DKKpipiResonances;
using namespace RooFit;

TH2* binomiDalitz(){

  RooRandom::randomGenerator()->SetSeed(1248+3);
  gRandom->SetSeed(3+3);

  bool manyPlots=false;
  int wasteFactor = 2000;
  int Nevents = 5000;

  bool betterNorm=true;

  RooRealVar dummy("dummy", "dummy", 1);

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



  RooRealVar rB("rB", "rB", 0.15, 0.0, 1.0);
  RooRealVar delta("delta", "delta", 130, -180, 180);
  //RooRealVar rB("rB", "rB", 0.15);
  //RooRealVar delta("delta", "delta", 0);
  RooRealVar gamma("gamma", "gamma", 60, -180, 180);
  //RooRealVar gamma("gamma", "gamma", 60);
  RooRealVar prodRatio("prodRatio", "prodRatio", 1, 0.5, 2);
  //  RooRealVar prodRatio("prodRatio", "prodRatio", 1.0);

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

  //  RooDalitz BPlusPDF_("BPlusPDF_", "BPlusPDF_", BPlus, dummy);
  //  RooDalitz BMinusPDF_("BMinusPDF_", "BPlusPDF_", BPlus, dummy);

  RooBinomiDalitz DaPDF("BinomiDalitz", "BinomiDalitz"
			, BPlus, BMinus, bcharge, prodRatio);
  time_t t7 = time(NULL);

  cout << "Now generating the full thing" << endl;

  RooDataSet* dataPlus   = phaseSpace.generate(mset,wasteFactor*Nevents);
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


  RooDataSet* dataMinus  = phaseSpace.generate(mset,wasteFactor*Nevents);

  double largest=-1;
  if(!betterNorm){
    double largestPlusPlus = getLargestValue(BPlus, mlist, dataPlus);
    double largestMinusMinus = getLargestValue(BMinus, mlist, dataMinus);
    double largestPlusMinus = getLargestValue(BPlus, mlist, dataMinus);
    double largestMinusPlus = getLargestValue(BMinus, mlist, dataPlus);
    
    double largestPlus = largestPlusPlus > largestPlusMinus? largestPlusPlus : largestPlusMinus;
    double largestMinus = largestMinusMinus > largestMinusPlus? largestMinusMinus : largestMinusPlus;
    
    largest = largestPlus > largestMinus ? largestPlus : largestMinus;
  }
  
  cout << "before dataThrowerAway I have " 
       << dataPlus->numEntries() 
       << " events in dataPlus " << endl;
  if(! betterNorm)
    dataPlus = dataThrowerAway(BPlus, mlist, dataPlus, -1, largest);
  else
    dataPlus = dataThrowerAway(BPlus, mlist, dataPlus, Nevents);
  bcharge.setIndex(1);
  dataPlus = mark(dataPlus, bcharge);
  dataPlus->Print("v");

  massPlotSet4 plotsPlus = makeMassPlots4(s0, dataPlus, "plus");
  plotsPlus.saveHistosAndEps();

  cout << "after dataThrowerAway I have " 
       << dataPlus->numEntries() 
       << " events in dataPlus " << endl;
  //  RooDataSet* dataMinus  = phaseSpace.generate(mset, 50*Nevents);
  if(! betterNorm)
    dataMinus = dataThrowerAway(BMinus, mlist, dataMinus, -1, largest);
  else
    dataMinus = dataThrowerAway(BMinus, mlist, dataMinus, Nevents);
  
  dataMinus->Print("v");
  bcharge.setIndex(-1);
  dataMinus = mark(dataMinus, bcharge);
  cout << "after dataThrowerAway I have " 
       << dataMinus->numEntries() 
       << " events in dataMinus " << endl;
  
  if(dataPlus->numEntries() > 0){
    cout << "this means that the production ratio is: "
	 << ((double)dataMinus->numEntries())/((double)dataPlus->numEntries())
	 << endl;
  }

  RooDataSet* data = (RooDataSet*) dataPlus->Clone();
  data->append(*dataMinus);
  data->Print("v");

  massPlotSet4 plotsMinus = makeMassPlots4(s0, dataMinus, "minus");
  plotsMinus.saveHistosAndEps();

  massPlotSet4 plotsAll = makeMassPlots4(s0, data, "all");
  plotsAll.saveHistosAndEps();



  TFile fData("DKKpipiBinomi.root", "RECREATE");
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
  dataPlus->fillHistogram(ph2Plus, RooArgList(s12, s23));
  dataMinus->fillHistogram(ph2Minus, RooArgList(s12, s23));
  
  TH2* diff = (TH2*) ph2Plus->Clone("diff");
  diff->Add(ph2Minus, -1.);
  

  TCanvas tcan;

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


  // asymmetries    
  RooPlot* t01FrameAs = t01.frame();
  data->plotOn(t01FrameAs, Asymmetry(bcharge)
		   );// , Binning(s23.getMin(), s23.getMax(), nbins));
  t01FrameAs->Draw();
  tcan.Print("t01As.eps");


  RooPlot* s12FrameAs = s12.frame();
  data->plotOn(s12FrameAs, Asymmetry(bcharge)
		   );
  s12FrameAs->Draw();
  tcan.Print("s12As.eps");
  
  RooPlot* s23FrameAs = s23.frame();
  data->plotOn(s23FrameAs, Asymmetry(bcharge)
		   );// , Binning(s23.getMin(), s23.getMax(), nbins));
  s23FrameAs->Draw();
  tcan.Print("s23As.eps");
    
  RooPlot* s34FrameAs = s34.frame();
  data->plotOn(s34FrameAs, Asymmetry(bcharge)
		   );// , Binning(s34.getMin(), s34.getMax(), nbins));
  s34FrameAs->Draw();
  tcan.Print("s34As.eps");
    
  RooPlot* t40FrameAs = t40.frame();
  data->plotOn(t40FrameAs, Asymmetry(bcharge)
		   );// , Binning(t40.getMin(), t40.getMax(), nbins));
  t40FrameAs->Draw();
  tcan.Print("t40As.eps");
    
  TFile fHisto("BiDalitzHisto.root", "RECREATE");
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
  ph2Minus->Write();
  diff->Write();
  fHisto.Close();

  
  time_t t8 = time(NULL);
  cout << "generating full Dalitz took " 
       << difftime(t8, t7) << " s" << endl;
  cout << "now doing the fit" << endl;
  DaPDF.constOptimize(RooAbsArg::DeActivate) ;
  DaPDF.fitTo(*data, Optimize(false));
  //DaPDF.fitTo(*data);
  time_t t9 = time(NULL);
  cout << "fitting  took " 
       << difftime(t9, t8) << " s" << endl;

  
  
  RooPlot* llZoomframe = gamma.frame(20, 90, 40);
  DaPDF.plotNLLOn(llZoomframe, data);
  llZoomframe->Draw();
  tcan.Print("llZoom.eps");
  
  RooPlot* llframe = gamma.frame(0, 180, 40);
  DaPDF.plotNLLOn(llframe, data);
  llframe->Draw();
  tcan.Print("ll.eps");

  if(manyPlots){

    int nhbins = 400;
    double hmin=50, hmax=70;
    double bw=(hmax-hmin)/((double)nhbins);
    TH1F moh("myOwnHisto", "myOwnHisto", nhbins, hmin, hmax);
    
    DaPDF.attachDataSet(*data);
    for(int i=0; i < data->numEntries();  i++){    
      data->get(i);
      for(int bin=1; bin <= nhbins; bin++){
	double g = (((double) bin) - 0.5)*bw + hmin;
	gamma = g;
	double ll = -DaPDF.getLogVal();
	moh.Fill(g, ll);
      }
    }
    
    moh.Draw();
    tcan.Print("moh.eps");

    RooPlot* llSingleSummaryframe=gamma.frame(50, 70, 40);
    for(int i=0; i < data->numEntries();  i++){       
      
      RooPlot* llSingleframe = gamma.frame(50, 70, 40);
      cout << "plotting LL for event " << i << endl;
      const RooArgSet* row = data->get(i);
      row->Print("v");
      RooDataSet singleEventData("singleEventData"
				 , "singleEventData"
				 , *(data->get(i))
				 );
      singleEventData.add(*row);
      int colCode = i%9 + 1;
      cout << "got the event, now actually doing the plot with colCode " 
	   << colCode << endl;
      //     llSingleframe->SetLineColor(colCode);
      
      DaPDF.plotNLLOn(llSingleframe, &singleEventData);
      llSingleframe->getAttLine()->SetLineColor(colCode);
      DaPDF.plotNLLOn(llSingleSummaryframe, &singleEventData);
      llSingleSummaryframe->getAttLine()->SetLineColor(colCode);
      
      cout << "made it, now drawing it +" << endl;
      llSingleframe->Draw();
      char plotname[1000];
      sprintf(plotname, "ll_%i.eps", i);  
      tcan.Print(plotname);
      
      llSingleSummaryframe->Draw();
      sprintf(plotname, "lls_%i.eps", i);  
      tcan.Print(plotname);
      
      delete llSingleframe;
    }
  }


  // asymmetries    
  DaPDF.plotOn(t01FrameAs, Asymmetry(bcharge), ProjWData(*data)
		   );// , Binning(s23.getMin(), s23.getMax(), nbins));
  t01FrameAs->Draw();
  tcan.Print("t01AsPDF.eps");

  DaPDF.plotOn(s12FrameAs, Asymmetry(bcharge), ProjWData(*data)
		   );
  s12FrameAs->Draw();
  tcan.Print("s12AsPDF.eps");
  
  DaPDF.plotOn(s23FrameAs, Asymmetry(bcharge), ProjWData(*data)
		   );// , Binning(s23.getMin(), s23.getMax(), nbins));
  s23FrameAs->Draw();
  tcan.Print("s23AsPDF.eps");
    
  DaPDF.plotOn(s34FrameAs, Asymmetry(bcharge), ProjWData(*data)
		   );// , Binning(s34.getMin(), s34.getMax(), nbins));
  s34FrameAs->Draw();
  tcan.Print("s34AsPDF.eps");
    
  DaPDF.plotOn(t40FrameAs, Asymmetry(bcharge), ProjWData(*data)
		   );// , Binning(t40.getMin(), t40.getMax(), nbins));
  t40FrameAs->Draw();
  tcan.Print("t40AsPDF.eps");


  TFile fHistoPDF("BiDalitzHistoPDF.root", "RECREATE");
  fHistoPDF.cd();
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
  ph2Minus->Write();
  diff->Write();
  fHistoPDF.Close();

  

  char* zAxisLabel = "Dalitz";
  Double_t xlo=0, xhi=5;
  Int_t nBins=100;
  
  return ph2Plus;
}
//
//

