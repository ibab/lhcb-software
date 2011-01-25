#include <string>
#include "JStyle.h"
gStyle->SetOptStat(0); 

TF1* fit(TH1F* h) {
  h->Fit("gaus");
  TF1* myfunc2 = h->GetFunction("gaus");
  myfunc2->SetLineColor(2);
  myfunc2->SetLineStyle(1);
  return myfunc2;
}


TCanvas* makeCanvas(std::string name) {
  TCanvas *c2 = new TCanvas(name.c_str(),name.c_str(),10,44,800,400);
  c2->Divide(2);
  c2->cd(1);
  return c2 ;
}



void toyPulls() {

  gStyle->SetOptFit(11);

  // get the file
  TFile *file =new TFile("forpulls.root");

  // make the p plot
  TCanvas* c2 = makeCanvas("c2");
  TH1F* histo = (TH1F*) file->Get("mpull;1"); // get the mean
  StyleTH1F(histo);
  histo->Rebin(2);
  histo->GetXaxis()->SetTitle("mass pull");
  TF1* f1 = fit(histo);
  histo->Draw("HISTO"); 
  f1->Draw("SAME");

  c2->cd(2);
  TH1F* histo2 = (TH1F*) file->Get("spull;1"); // get the mean
  StyleTH1F(histo2);
  histo2->GetXaxis()->SetTitle("#sigma pull");
  histo2->Rebin(2);
  fit(histo2);
  histo2->Draw("HISTO"); 
  f1->Draw("SAME");

  makeEps(c2,"toypulls.eps");

 
}
