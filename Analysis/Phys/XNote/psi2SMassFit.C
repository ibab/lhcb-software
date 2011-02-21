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
  //  TCut back = " (m > 3668 && m < 3677) || (m > 3695 && m < 3704)";
     TCut back = " (m > 3659 && m < 3677) || (m > 3695 && m < 3713)";
  TCut cutPiPi = " PT_JPSI > 3500 && TMath::Min(pt_pi1, pt_pi2) > 500 && TMath::Min(pt_mu1, pt_mu2) > 1000 && pidk1 <0 && pidk2 <0 && ( m - m23 - M_JPSI ) < 300";
 
 

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

  back_m23_data->Sumw2();
  sig_back_m23_data->Sumw2();
  sig_m23_data->Sumw2();
  //  *sig_m23_data = (*sig_back_m23_data) - (*back_m23_data);
  *sig_m23_data = *sig_back_m23_data;
  sig_m23_data->Add(back_m23_data,-0.5);

  // sig_m23_data->Sumw2();
  std::cout<< sig_m23_data->GetBinError(1) << " " << sig_back_m23_data->GetBinError(1) << " " << back_m23_data->GetBinError(1) << std::endl;
  back_m23_data->SetMinimum(0);
  back_m23_data->Draw("HISTO");
  

  /*
  for (int i = 1 ; i <= sig_m23_data->GetNbinsX(); ++i  ) {
    float err = sqrt( sig_back_m23_data->GetBinError(i)* sig_back_m23_data->GetBinError(i) + back_m23_data->GetBinError(1)*back_m23_data->GetBinError(1));
    sig_m23_data->SetBinError(i , err);
  }
  */
  // sig_m23_data->Sumw2();
  sig_m23_data->Draw(); 

  RooRealVar pp("pp","pp",300., 600.);
  RooRealVar mPi("mPion","mPion", 139.57, 0., 150.);
  mPi.setConstant(true);
  RooRealVar mJpsi("mJpsi","mJpsi", 3096.9, 0.., 4000.);
  mJpsi.setConstant(true);
  RooRealVar mpsi2("mpsi2","mpsi2",3686.1, 0., 4000.);
  mpsi2.setConstant(true);
  RooRealVar lambda("lambda", "lambda",4. , 0., 100);
  //  lambda.setConstant(true);
   RooRealVar p0("p0","p0",3.06  , -4., 4.);
     p0.setConstant(true);
   RooRealVar p1("p1","p1", -0.0077  , -4., 4.);
  
   p1.setConstant(true);
  RooRealVar p2("p2","p2", 6.2e-6   , -4., 4.);
  p2.setConstant(true);

  RooDataHist data("data", "dataset", pp , sig_m23_data);
  RooPsi2m ppModel= RooPsi2m("psi2", "psi2", pp, mJpsi, mpsi2, mPi , lambda, p0, p1,p2); 
  ppModel.fitTo(data);
  RooPlot* frame = pp.frame(); 

  
  frame->GetXaxis()->SetTitle("m_{#pi #pi} [MeV/c^{2}] ");
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


   TCanvas *c2 = new TCanvas("c2", "c2",10,44,600,500);
   pad1 = new TPad("pad1","This is pad1",0.05,0.3,0.95,0.97);
   pad2 = new TPad("pad2","This is pad2",0.05,0.0,0.95,0.3);
   pad1->Draw();
   pad2->Draw();
   pad1->cd();
   //  c2->Divide(1,2);
   // c2->cd(1);
   frame->Draw();
   RooPlot* frame2 =  pp->frame() ; // Same way you made 'frame' 
   // c2->cd(2);
   pad2->cd();
   RooHist* phist = frame->pullHist( );
   int np = phist->GetN();

   std::cout <<  " np " << np << std::endl; 
   //  std::vector<float> xv; std::vector<float> yv;
   double xv[200] ;
   double yv[200] ;
   for (int i = 0; i < np; ++i){
     xv[i] = phist->GetX()[i];
     yv[i] =  phist->GetY()[i] - phist->GetErrorYlow(i)  ; 
   }

  int start = i;
   for (int i = np-1; i >= 0; --i, ++start){
     xv[start] = phist->GetX()[i];
     yv[start] =  phist->GetY()[i] + phist->GetErrorYhigh(i)  ; 
   }

   xv[np*2] = xv[0];
   yv[np*2] = yv[0] -  phist->GetErrorYlow(0) ;

  TPolyLine* pline = new TPolyLine(2*np +1,xv, yv);
  pline->SetFillColor(31);
  pline->SetLineColor(2);
  pline->SetLineWidth(4);

  TPolyLine* line = new TPolyLine(np, phist->GetX(), phist->GetY());
  line->SetLineColor(2);
  line->SetLineWidth(2);

   frame2.SetMinimum(-6) ; 
   frame2.SetMaximum(6) ;
  TAxis* xachse = frame2.GetXaxis(); TAxis* yachse = frame2.GetYaxis();
  // mean.plotOn(xframe);

 xachse->SetTitleFont (132);
  yachse->SetTitleFont (132);
  xachse->SetLabelFont (132);
  yachse->SetLabelFont (132); 
  yachse->SetTitleOffset(0.3); 
  yachse->SetTitleSize(0.1);
  yachse->SetLabelSize(0.08);
  xachse->SetLabelOffset(0.2);
   frame2.SetXTitle("");
   frame2.SetYTitle("pull");
   frame2.SetTitle("");


  frame2.Draw() ; 
   pline->Draw("fsame");
   line->Draw("lsame");
  std::cout << sig_m23_data->GetEntries() << std::endl;


}//end of macro
