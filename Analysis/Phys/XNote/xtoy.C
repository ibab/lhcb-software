#include <vector>
#include <numeric>
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TChain.h"
#include "TH1F.h"
#include <iostream.h>
#include "TF1.h"
#include <math.h>
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooDataSet.h"
#include "RooVoigtian.h"
#include "RooBackPdf.h"
#include "RooAddPdf.h"
#include "RooFitResult.h"
#include "RooGaussian.h"
//#include "ksfun.h"

// binding 7.34 5.35 87.1

//gSystem->load("libRooFit");
using namespace RooFit;

//double nsig = 180.;
//double nback = 17000.;

/** macro to generate toy MC samples for X
.L xtoy
xtoy("output.root", nSignal, nBack)
*/

void xtoy(std::string thefile = "thefile.root", double nsig = 100, double nback = 100){

  //  double nsig = 100.;
  nback /= 0.1111;

  std::cout << "nsignal: " << nsig << std::endl;

  // model to generate from 
  RooRealVar x("x","x",3800, 3950.); 
  RooRealVar s("sigma", "sigma", 2.8, -10., 10);
  RooRealVar mean("mean", "mean", 3872, 0., 10000);

  RooRealVar sig("sig","sig fraction", nsig, 0., 100000.);
  RooRealVar fback("fback","fback", nback, 0., 100000.);
  RooRealVar width1("width1", "width1", 0.7, -100., 100);
  RooVoigtian voight("voight","voight(x,mean,sigma)",x, mean , width1,s);

  RooGaussian gauss("gauss","gauss(x,mean,sigma)",x, mean,s);

  RooRealVar c0("c0","c0",7.32,-100,100);
  RooRealVar c1("c1","c1",0.15,-1,1);
  RooRealVar c2("c2","c2",-0.00001755, -1,1);
  RooRealVar mr("mr","mr", 3376., 2000,4000);

  c0.setConstant(true);
  c2.setConstant(true);
  mr.setConstant(true);
  width1.setConstant(true);
  RooBackPdf bg("bg", "bg",x, mr, c0, c1, c2);
 
  RooAddPdf model("model","model", RooArgList(voight,bg),RooArgList(sig,fback));
  //RooAddPdf model("model","model", RooArgList(gauss,bg),RooArgList(sig,fback));
  // RooAddPdf model("gmodel","gmodel", RooArgList(voight),RooArgList(sig));

  // model to fit to
  RooRealVar s2("fitsigma", "fitsigma", 2.8, -10., 10);
  RooRealVar mean2("fitmean", "fitmean", 3872, 3000., 4000);
  RooRealVar sig2("fitsig","fitted sig fraction", nsig, 0., 2000.);
  RooRealVar fback2("fitback","fitted back fraction",  nback, 0., 100000.);
  RooVoigtian voight2("voight2","voight(x,mean,sigma)",x, mean2 , width1,s2);
  RooGaussian gauss2("gauss2","gauss(x,mean,sigma)",x, mean2 ,s2);

  RooRealVar fc1("fc1","fc1",0.15, -1.,1); 
  RooBackPdf bg2("bg2", "bg2",x, mr, c0, c1, c2);
  RooAddPdf fmodel("model2","model", RooArgList(voight2,bg2),RooArgList(sig2,fback2));
  //RooAddPdf fmodel("model2","model", RooArgList(gauss2,bg2),RooArgList(sig2,fback2));
  //RooAddPdf fmodel("gmodel2","gmodel2", RooArgList(voight2),RooArgList(sig2));

  // book output histograms
  TH1F* mpull = new TH1F("mpull", "mpull", 200, -10., 10);
  TH1F* spull = new TH1F("spull", "spull", 200, -10., 10);

  TH1F* dmasshisto = new TH1F("dmass", "dmass", 200, -5., 5);
  TH1F* sigmahisto = new TH1F("sigma", "sigma", 200,  0., 10);
  TH1F* dsigmahisto = new TH1F("dsigma", "dsigma", 200,  -10., 10);

  TH1F* emasshisto = new TH1F("emass", "emass", 200, 0., 10);
  TH1F* esigmahisto = new TH1F("esigma", "esigma", 200, 0., 10);

  // corelation matrix terms booking
  TObjArray* vec = new TObjArray(5*5); //vec.Expand(5);
  for (unsigned int i = 0; i < 5; ++i){
     for (unsigned int j = 0; j < 5; ++j){
       char name[10]; 
       sprintf(name,"cov%d%d", i, j);
       TH1F* histo = new TH1F(name, name, 201, -1.005 ,1.005);
       vec->Add(histo);
     }
  }
  
  // generate the data sets, make 500 samples
  for (int iSample =0 ; iSample< 500; ++iSample){ 
    RooDataSet* dset = model.generate(x, Extended(kTRUE)) ;
    RooFitResult* fitResult = fmodel.fitTo(*dset, Extended(), Save(), PrintLevel(-1),  PrintEvalErrors(-1) ,Verbose(false), Warnings(false));
    std::cout << "Making toy " << iSample  << " " <<dset->numEntries() << std::endl;
    double mp = (mean2.getVal() - mean.getVal())/mean2.getError();
    double sp = (TMath::Abs(s2.getVal()) - TMath::Abs(s.getVal()))/s2.getError();
    mpull->Fill(mp);
    spull->Fill(sp);
    dmasshisto->Fill((mean2.getVal() - mean.getVal()));
    dsigmahisto->Fill((s2.getVal() - s.getVal()));
    sigmahisto->Fill(s2.getVal());
    emasshisto->Fill(mean2.getError());
    esigmahisto->Fill(s2.getError());

    // order c1, fitback, fitmean, fitsig, fitsigma
    const TMatrixDSym& m = fitResult->correlationMatrix();
    for (unsigned int ii = 0; ii < 5; ++ii){
       for (unsigned int jj = 0; jj < 5; ++jj){
	 int bin = 5 * ii + jj;
         TObject* obj = vec->At(bin);
         TH1F* h = dynamic_cast<TH1F*>(obj);
         h->Fill(m[ii][jj]);  
       }
    }

    //    fitResult->correlationHist()->Draw("col*z");

    /*
     RooPlot* frame = x.frame();
     //  fmodel.plotOn(frame);
     dset.plotOn(frame);
     fmodel.plotOn(frame);
     frame.Draw();
    */
  }

 
  //  write the histograms to a file
  TFile* outFile = new TFile(thefile.c_str(),"RECREATE","output ROOT file");
  dmasshisto->Write(); 
  emasshisto->Write();
  dsigmahisto->Write(); 
  esigmahisto->Write(); 
  sigmahisto->Write();
  mpull->Write();
  spull->Write();
 
  for (int ic = 0; ic < 25; ++ic){
    TObject* obj = vec->At(ic);
    obj->Write();
  }

  outFile->Close();

  // getting the S/B - magics lines to integrate in RooFit
  RooArgSet nset(x) ;
  double window = 3 * s.getVal();
  x.setRange("signal",mean.getVal() - window, mean.getVal() + window) ;
  RooAbsReal* igx_sig = bg.createIntegral(x,NormSet(x),Range("signal")) ;
  double bfrac = igx_sig->getVal() * nback;
  cout << "S/B " << nsig / bfrac <<std::endl;
  cout << "S/B " << bfrac/ float(nback) <<std::endl;
}
