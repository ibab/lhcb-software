#include "TROOT.h"
//#include "TPalette.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include <iostream.h>
#include <vector>
#include <string>
#include "TH2D.h"
#include "TH1D.h"


void itactiveplot(){

  std::string filename = "testReprocess.root";
  TFile* aFile = new TFile(filename.c_str());
  aFile->ls(); 
  std::string name = "/IT/ITPerf/fracActive";
  
  gStyle->SetPalette(2,0);
  gStyle->SetPalette(1,0);


  int nColors=51;
  Double_t s[5] = {0.00, 0.25, 0.50, 0.75, 1.00};
  Double_t r[5] = {0.99, 0.00, 0.87, 1.00, 0.70};
  Double_t g[5] = {0.99, 0.81, 1.00, 0.20, 0.00};
  Double_t b[5] = {0.99, 1.00, 0.12, 0.00, 0.00};
  TColor::CreateGradientColorTable(5, s, g, r, b, nColors);
  gStyle->SetNumberContours(nColors);

  TH2F* aHisto = (TH2F*)aFile->Get(name.c_str());
  
  aHisto->SetTitle("");
  TAxis *xachse = aHisto->GetXaxis (), *yachse = aHisto->GetYaxis ();
  xachse->SetTitleFont (132);
  yachse->SetTitleFont (132);
  xachse->SetLabelFont (132);
  yachse->SetLabelFont (132);

  aHisto->SetMinimum(0.0);
  aHisto->SetMaximum(1.0);
  aHisto->Draw("col");
 
  TLatex* tex = new TLatex(-10, 9.5, "IT1");
  tex->SetLineWidth(2);
  tex->SetTextFont(2);
  tex->Draw("");

  TLatex* tex = new TLatex(-10, 25.5, "IT2");
  tex->SetLineWidth(2);
  tex->SetTextFont(2);
  tex->Draw("");

  TLatex* tex = new TLatex(-10, 41.5, "IT3");
  tex->SetLineWidth(2);
  tex->SetTextFont(2);
  tex->Draw("");

  TLatex* tex = new TLatex(-0.25, 29.5, "T");
  tex->SetLineWidth(2);
  tex->SetTextFont(2);
  tex->Draw("");

  TLatex* tex = new TLatex(-0.25, 21.5, "B");
  tex->SetLineWidth(2);
  tex->SetTextFont(2);
  tex->Draw("");

  TLatex* tex = new TLatex(-5, 25.5, "A");
  tex->SetLineWidth(2);
  tex->SetTextFont(2);
  tex->Draw("");

  TLatex* tex = new TLatex(4, 25.5, "C");
  tex->SetLineWidth(2);
  tex->SetTextFont(2);
  tex->Draw("");



   


}







