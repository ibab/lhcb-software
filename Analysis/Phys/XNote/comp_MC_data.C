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


void comp_MC_data() {

  int nbins = 16;
  
  double pmin = 0;
  double pmax = 500;

  double ptmin = 0;
  double ptmax = 30;

  double Qmin = 0;
  double Qmax = 400;

  double m23min = 250;
  double m23max = 700;

  
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

  

  ///////////////////////////////// 37 /pb rescale ///////////////////////////////////////  
  // DecayTreePiPi->Add("/users/bressieu/126/1/outputdata/Tupletuple.root");
  // DecayTreePiPi->Add("/users/bressieu/126/0/outputdata/Tupletuple.root");
  // DecayTreePiPi->Add("/users/bressieu/126/2/outputdata/Tupletuple.root");
  // DecayTreePiPi->Add("/users/bressieu/126/3/outputdata/Tupletuple.root");
  // DecayTreePiPi->Add("/users/bressieu/126/4/outputdata/Tupletuple.root");

  DecayTreePiPi->Add("ntuples/127/1/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/0/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/2/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/3/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/4/outputdata/Tupletuple.root");

  DecayTreePiPi_MC->Add("ntuples/139/1/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC->Add("ntuples/139/0/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC->Add("ntuples/139/2/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC->Add("ntuples/139/3/outputdata/Tupletuple_MC.root");
  // DecayTreePiPi_MC->Add("/users/bressieu/139/4/outputdata/Tupletuple_MC.root");

  

  TCut sig =  " m > 3677 && m < 3695";
  TCut back = " (m > 3668 && m < 3677) || (m > 3695 && m < 3704)";
  TCut cutPiPi = " PT_JPSI > 3500 && TMath::Min(pt_pi1, pt_pi2) > 500 && TMath::Min(pt_mu1, pt_mu2) > 1000 && pidk1 <0 && pidk2 <0";
  TCut cutPiPi_MC = "PT_JPSI > 3500 && TMath::Min(pt_pi1, pt_pi2) > 500 && TMath::Min(pt_mu1, pt_mu2) > 1000 && m > 3868 && m< 3878 && pidk1 <0 && pidk2 <0";

  ///P/////
  sig_P_data = new TH1D("sig_P_data", "P_data; p[GeV/c]",  nbins, pmin, pmax);
  sig_back_P_data = new TH1D("sig_back_P_data", "sig_back_P_data; p[GeV/c]",  nbins, pmin, pmax);
  back_P_data = new TH1D("back_P_data", "back_P_data; p[GeV/c]",  nbins, pmin, pmax);
  sig_P_MC = new TH1D("sig_P_MC", "P_data; p[GeV/c]",  nbins, pmin, pmax);
  sig_back_P_MC = new TH1D("sig_back_P_MC", "sig_back_P_data; p[GeV/c]",  nbins, pmin, pmax);
  back_P_MC = new TH1D("back_P_MC", "back_P_data; p[GeV/c]",  nbins, pmin, pmax);

  DecayTreePiPi->Draw("P/1000>>sig_back_P_data", cutPiPi*sig);
  DecayTreePiPi->Draw("P/1000>>back_P_data", cutPiPi*back);
  DecayTreePiPi_MC->Draw("P/1000>>sig_back_P_MC", cutPiPi*sig);
  DecayTreePiPi_MC->Draw("P/1000>>back_P_MC", cutPiPi*back);

  *sig_P_data = (*sig_back_P_data) - (*back_P_data);
  *sig_P_MC = (*sig_back_P_MC) - (*back_P_MC);

  ///PT////
  sig_PT_data = new TH1D("sig_PT_data", "P_data; p_{T}[GeV/c]",  nbins, ptmin, ptmax);
  sig_back_PT_data = new TH1D("sig_back_PT_data", "sig_back_P_data; p_{T}[GeV/c]",  nbins, ptmin, ptmax);
  back_PT_data = new TH1D("back_PT_data", "back_P_data; p_{T}[GeV/c]",  nbins, ptmin, ptmax);
  sig_PT_MC = new TH1D("sig_PT_MC", "PT_data; p_{T}[GeV/c]",  nbins, ptmin, ptmax);
  sig_back_PT_MC = new TH1D("sig_back_PT_MC", "sig_back_PT_data; p_{T}[GeV/c]",  nbins, ptmin, ptmax);
  back_PT_MC = new TH1D("back_PT_MC", "back_P_data; p_{T}[GeV/c]",  nbins, ptmin, ptmax);

  DecayTreePiPi->Draw("PT/1000>>sig_back_PT_data", cutPiPi*sig);
  DecayTreePiPi->Draw("PT/1000>>back_PT_data", cutPiPi*back);
  DecayTreePiPi_MC->Draw("PT/1000>>sig_back_PT_MC", cutPiPi*sig);
  DecayTreePiPi_MC->Draw("PT/1000>>back_PT_MC", cutPiPi*back);

  *sig_PT_data = (*sig_back_PT_data) - (*back_PT_data);
  *sig_PT_MC = (*sig_back_PT_MC) - (*back_PT_MC);

  StyleTH1D(sig_P_MC);
  StyleTH1D(sig_P_data);
  StyleTH1D(sig_PT_MC);
  StyleTH1D(sig_PT_data); 



  /*

  ///Q////
  sig_Q_data = new TH1D("sig_Q_data", "P_data; Q[MeV/c^{2}]",  nbins, Qmin, Qmax);
  sig_back_Q_data = new TH1D("sig_back_Q_data", "sig_back_P_data; Q[MeV/c^{2}]",  nbins, Qmin, Qmax);
  back_Q_data = new TH1D("back_Q_data", "back_P_data; Q[MeV/c^{2}]",  nbins, Qmin, Qmax);
  sig_Q_MC = new TH1D("sig_Q_MC", "Q_data; Q[MeV/c^{2}]",  nbins, Qmin, Qmax);
  sig_back_Q_MC = new TH1D("sig_back_Q_MC", "sig_back_Q_data; Q[MeV/c^{2}]",  nbins, Qmin, Qmax);
  back_Q_MC = new TH1D("back_Q_MC", "back_P_data; Q[MeV/c^{2}]",  nbins, Qmin, Qmax);

  DecayTreePiPi->Draw("Q>>sig_back_Q_data", cutPiPi*sig);
  DecayTreePiPi->Draw("Q>>back_Q_data", cutPiPi*back);
  DecayTreePiPi_MC->Draw("Q>>sig_back_Q_MC", cutPiPi*sig);
  DecayTreePiPi_MC->Draw("Q>>back_Q_MC", cutPiPi*back);


  *sig_Q_data = (*sig_back_Q_data) - (*back_Q_data);
  *sig_Q_MC = (*sig_back_Q_MC) - (*back_Q_MC);



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
  *sig_m23_MC = (*sig_back_m23_MC) - (*back_m23_MC);
  TF1 *Straight_line = new TF1("Straight_line","0",0,2000000);
  */
  

  TCanvas* c1 = new TCanvas("c1","comp data MC",800,400) ;
  c1->Divide(2,1);
  c1->cd(1);
  sig_P_MC->SetLineColor(38);
  sig_P_MC->SetFillColor(38);
  sig_P_MC->GetXaxis()->SetTitleOffset(1.2);
  sig_P_MC->SetLineWidth(lineWidth);

  sig_P_MC->DrawNormalized();

  sig_P_data->DrawNormalized("same");

  //Straight_line->Draw("same");

  leg = new TLegend(0.6,0.75,0.9,0.98);
  leg->AddEntry(sig_P_data,"data","l");
  leg->SetHeader ("");
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetTextFont(132);
  //  leg->AddEntry(Q_MC_psi,"simulated #psi(2S)","l");
  leg->AddEntry(sig_P_MC,"#psi(2S) MC","lf");
  leg->Draw("same");


  leg = new TLegend(0.5,1,0.8,0.4);
  leg->AddEntry(sig_P_data,"data","l");
  leg->AddEntry(sig_P_MC,"simulated #psi(2S)","l");
  //  leg->Draw("same");

  c1->cd(2);
 
  sig_PT_MC->GetXaxis()->SetTitleOffset(1.2);
  sig_PT_MC->SetLineColor(38);
  sig_PT_MC->SetFillColor(38);
  sig_PT_data->SetLineWidth(lineWidth);
  sig_PT_MC->DrawNormalized();
  sig_PT_data->DrawNormalized("same");

  makeEps(c1,"compdistri.eps");

  //Straight_line->Draw("same");
  /*  
  c1->cd(3);
  sig_Q_data->DrawNormalized();
  sig_Q_MC->SetLineColor(kRed);
  sig_Q_MC->DrawNormalized("same");
  Straight_line->Draw("same");
  
  c1->cd(4);
  sig_m23_data->DrawNormalized();
  sig_m23_MC->SetLineColor(kRed);
  sig_m23_MC->DrawNormalized("same");
  Straight_line->Draw("same");
  */
}//end of macro
