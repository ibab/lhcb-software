//Roofit stuff

#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "Riostream.h" 
#include "TMath.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "TAxis.h"
#include "RooDataHist.h"
// #include "RooBackPdf.h"
#include "RooRealVar.h"
#include "RooPlot.h"

#include "JStyle.h"
using namespace RooFit ;


void psi2SMassFit() {

  int nbins = 10;
  m23min = 300;
  m23max = 600;


  
  //===========================================
  //Graphical; plots options 
  //===========================================
  // gROOT->ProcessLine(".L ./RooBackPdf.cxx+");
  gROOT->ProcessLineSync(".L RooBackPdf.cxx+") ;
  gROOT->ProcessLine(".x ./mattStyle.C");
  gStyle->SetOptStat (0);


  TChain *DecayTreePiPi = new TChain ("JPsiPiPiAna/psiCand");
  DecayTreePiPi->Reset(); 
  // DecayTreePiPi->Add("./Tupletuple.root");
  TChain *DecayTreePiPi_MC = new TChain ("JPsiPiPiAna/psiCand");
  DecayTreePiPi_MC->Reset(); 

  
  DecayTreePiPi->Add("ntuples/127/1/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/0/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/2/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/3/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/4/outputdata/Tupletuple.root");


  TCut sig =  " m > 3677 && m < 3695";
  TCut back = " (m > 3668 && m < 3677) || (m > 3695 && m < 3704)";
  TCut cutPiPi = " PT_JPSI > 3500 && TMath::Min(pt_pi1, pt_pi2) > 500 && TMath::Min(pt_mu1, pt_mu2) > 1000 && pidk1 <0 && pidk2 <0";
 
 

  ///m23///
  sig_m23_data = new TH1D("sig_m23_data", "P_data; M_{#pi #pi}[MeV/c^{2}]",  nbins, m23min, m23max);
  sig_back_m23_data = new TH1D("sig_back_m23_data", "sig_back_P_data; M_{#pi #pi}[MeV/c^{2}]",  nbins, m23min, m23max);
  back_m23_data = new TH1D("back_m23_data", "back_P_data; M_{#pi #pi}[MeV/c^{2}]",  nbins, m23min, m23max);
  sig_m23_MC = new TH1D("sig_m23_MC", "m23_data; M_{#pi #pi}[MeV/c^{2}]",  nbins, m23min, m23max);
  sig_back_m23_MC = new TH1D("sig_back_m23_MC", "sig_back_m23_data;M_{#pi #pi}[MeV/c^{2}] ",  nbins, m23min, m23max);
  back_m23_MC = new TH1D("back_m23_MC", "back_P_data; M_{#pi #pi}[MeV/c^{2}]",  nbins, m23min, m23max);

  DecayTreePiPi->Draw("m23>>sig_back_m23_data", cutPiPi*sig);
  DecayTreePiPi->Draw("m23>>back_m23_data", cutPiPi*back);
  DecayTreePiPi_MC->Draw("m23>>sig_back_m23_MC", cutPiPi*sig);
  DecayTreePiPi_MC->Draw("m23>>back_m23_MC", cutPiPi*back);

  *sig_m23_data = (*sig_back_m23_data) - (*back_m23_data);
  sig_m23_data->Sumw2();
  sig_m23_data->Draw(); 

  RooRealVar pp("pp","pp",300., 600.);
  RooRealVar mPi("mPion","mPion", 139.57, 0., 150.);
  mPi.setConstant(true);
  RooRealVar mJpsi("mJpsi","mJpsi", 3096.7, 0.., 4000.);
  mJpsi.setConstant(true);
  RooRealVar mpsi2("mpsi2","mpsi2",3686., 0., 4000.);
  mpsi2.setConstant(true);
  RooRealVar lambda("lambda", "lambda",4 , 0., 100);
   lambda.setConstant(true);

  RooDataHist data("data", "dataset", pp , sig_m23_data);
  RooPsi2m ppModel= RooPsi2m("psi2", "psi2", pp, mJpsi, mpsi2, mPi , lambda); 
  ppModel.fitTo(data);
  RooPlot* frame = pp.frame(); 

  
  frame->GetXaxis()->SetTitle("M_{#pi #pi} [MeV/c^{2}] ");
  frame->GetYaxis()->SetTitle("Events ");
  frame->SetTitle("");
    
  TAxis* xachse = frame->GetXaxis(); TAxis* yachse = frame->GetYaxis();

  xachse->SetTitleFont (132);
  yachse->SetTitleFont (132);
  xachse->SetLabelFont (132);
  yachse->SetLabelFont (132); 
  xachse->SetTitleOffset(1.2);

  //  ppModel->plotOn(frame);
  data.plotOn(frame);
  ppModel.plotOn(frame, LineColor(2));
  frame->Draw();
  std::cout << sig_m23_data->GetEntries() << std::endl;


}//end of macro
