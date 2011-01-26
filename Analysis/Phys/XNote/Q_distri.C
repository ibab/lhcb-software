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

using namespace RooFit ;
#include "JStyle.h"

void Q_distri() {

  gROOT -> ProcessLine( ".x ./mattStyle.C" );  

  int nbins = 25;
  
  double pmin = 0;
  double pmax = 600000;

  double ptmin = 0;
  double ptmax = 30000;

  double Qmin = -50;
  double Qmax = 800;

  
  //===========================================
  //Graphical; plots options 
  //===========================================
  //  gROOT->ProcessLine(".x ./lhcbstyle.C");
  gStyle->SetOptStat (0);

  TChain *DecayTreePiPi = new TChain ("JPsiPiPiAna/psiCand");
  DecayTreePiPi->Reset(); 
  // DecayTreePiPi->Add("/users/bressieu/127/1/outputdata/Tupletuple.root");
  // DecayTreePiPi->Add("/users/bressieu/127/0/outputdata/Tupletuple.root");
  // DecayTreePiPi->Add("/users/bressieu/127/2/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/3/outputdata/Tupletuple.root");
  DecayTreePiPi->Add("ntuples/127/4/outputdata/Tupletuple.root");

  TChain *DecayTreePiPi_MC = new TChain ("JPsiPiPiAna/psiCand");
  DecayTreePiPi_MC->Reset(); 
  DecayTreePiPi_MC->Add("ntuples/138/1/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC->Add("ntuples/138/0/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC->Add("ntuples/138/2/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC->Add("ntuples/138/3/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC->Add("ntuples/138/4/outputdata/Tupletuple_MC.root");

  TChain *DecayTreePiPi_MC_psi = new TChain ("JPsiPiPiAna/psiCand");
  DecayTreePiPi_MC_psi->Reset(); 
  DecayTreePiPi_MC_psi->Add("ntuples/139/1/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC_psi->Add("ntuples/139/0/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC_psi->Add("ntuples/139/2/outputdata/Tupletuple_MC.root");
  DecayTreePiPi_MC_psi->Add("ntuples/139/3/outputdata/Tupletuple_MC.root");


 
  char cutPiPi[500] = " PT_JPSI > 3500 && TMath::Min(pt_pi1, pt_pi2) > 500 && TMath::Min(pt_mu1, pt_mu2) > 1000 && pidk1 <0 && pidk2 <0";
  char cutPiPi_MC_X[500] = "PT_JPSI > 3500 && TMath::Min(pt_pi1, pt_pi2) > 500 && TMath::Min(pt_mu1, pt_mu2) > 1000 && m > 3868 && m< 3878& pidk1 <0 && pidk2 <0";
  char cutPiPi_MC_psi[500] = "PT_JPSI > 3500 && TMath::Min(pt_pi1, pt_pi2) > 500 && TMath::Min(pt_mu1, pt_mu2) > 1000 && m > 3670 && m< 37028& pidk1 <0 && pidk2 <0";

  Q_data = new TH1D("Q_data", "Q_data; Q[MeV/c^{2}]",          nbins, Qmin, Qmax);
  Q_MC_psi   = new TH1D("Q_MC_psi"  , "Q_MC;  Q[MeV/c^{2}]",   nbins, Qmin, Qmax);
  Q_MC_X   = new TH1D("Q_MC_X"  , "Q_MC;  Q[MeV/c^{2}]",       nbins, Qmin, Qmax);

  // m_MC_psi = new TH1D("m_MC_psi", "m; m[MeV/c^{2}]",  nbins, 3600, 4000);


  DecayTreePiPi->Draw("Q>>Q_data", cutPiPi);
  DecayTreePiPi_MC_psi->Draw("Q>>Q_MC_psi", cutPiPi_MC_psi);
  DecayTreePiPi_MC->Draw("Q>>Q_MC_X", cutPiPi_MC_X);

  TCanvas* c1 = new TCanvas("c1","comp data MC",500,500) ;
  c1->cd();
  StyleTH1D(Q_data);
  StyleTH1D(Q_MC_psi);
  StyleTH1D(Q_MC_X);
 

  Q_data->Scale(1/Q_data->GetEntries());
  Q_MC_psi->Scale(1/Q_MC_psi->GetEntries());
  Q_MC_X->Scale(1/Q_MC_X->GetEntries());
  Q_data->GetXaxis()->SetTitleOffset(1.2);

  Q_data->SetMaximum(0.32);
  Q_data->SetLineWidth(2.);
  
  Q_data->Draw() ;
  Q_MC_psi->SetLineColor(kRed);
  //  Q_MC_psi->Draw("same") ;
  Q_MC_X->SetLineColor(38);
  Q_MC_X->SetFillColor(38);
  Q_MC_X->Draw("same") ;
  Q_data->Draw("same") ;

  leg = new TLegend(0.6,0.75,0.9,0.98);
  leg->AddEntry(Q_data,"data","l");
  leg->SetHeader ("");
  leg->SetFillStyle(0);
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetTextFont(132);
  //  leg->AddEntry(Q_MC_psi,"simulated #psi(2S)","l");
  leg->AddEntry(Q_MC_X,"X(3872) MC","lf");
  leg->Draw("same");

  TLatex *   tex = new TLatex(270,0.155,"Cut");
  tex->SetLineWidth(2);
  tex->SetTextFont(132);
  tex->SetTextSize(0.045);
  tex->Draw();
  

   TArrow *arrow = new TArrow(300.,0.1475284,300,0.0765168,0.05,">");
   arrow->SetFillColor(1);
   arrow->SetFillStyle(1001);
   arrow->Draw();

   makeEps(c1,"Qdistri.eps");


}//end of macro
