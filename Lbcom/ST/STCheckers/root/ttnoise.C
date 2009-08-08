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


void ttnoise(){

  std::string filename = "testReprocess.root";
  TFile* aFile = new TFile(filename.c_str());
  aFile->ls(); 
  std::string name = "/TT/STPerformanceMonitor/noise";
  
  gStyle->SetPalette(2,0);
  gStyle->SetPalette(1,0);


  int nColors=51;
  Double_t s[5] = {0.00, 0.25, 0.50, 0.75, 1.00};
  Double_t r[5] = {0.99, 0.00, 0.87, 1.00, 0.70};
  Double_t g[5] = {0.99, 0.81, 1.00, 0.20, 0.00};
  Double_t b[5] = {0.99, 1.00, 0.12, 0.00, 0.00};
  TColor::CreateGradientColorTable(5, s, r, b, g, nColors);
  gStyle->SetNumberContours(nColors);

  TH2F* aHisto = (TH2F*)aFile->Get(name.c_str());
  
  aHisto->SetTitle("");
  TAxis *xachse = aHisto->GetXaxis (), *yachse = aHisto->GetYaxis ();
  xachse->SetTitleFont (132);
  yachse->SetTitleFont (132);
  xachse->SetLabelFont (132);
  yachse->SetLabelFont (132);

  aHisto->SetMinimum(1.5);
  aHisto->SetMaximum(5.0);
  aHisto->Draw("col");
 

}







