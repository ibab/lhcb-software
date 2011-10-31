#include "TCanvas.h"
#include "TFile.h"
#include <string>
TCanvas* canvas = 0;
void make_plot(const char* typ, const char* src) {
  std::string n,m,p;
  n = typ;
  n += ".ioperf.";
  n += src;
  m = n+".root";
  p = n+".gif";
  TFile* f1=TFile::Open(m.c_str());
  canvas->cd();
  f1->Get("Event_ioperf")->Draw();
  canvas->Draw();
  canvas->SaveAs(p.c_str());
  f1->Close();
  delete f1;
}

void perfN(const char* fn) {
  canvas = new TCanvas("c1");
  char* files[] = {"Read.010","Read.050","Read.090","Read.100"};
  for(int i=0; i<4; ++i) {
    make_plot(files[i],fn);
  }
  gSystem->Exit(0);
}
void perf(const char* fn) {
  canvas = new TCanvas("c1");
  //char* files[] = {"Read.100"};
  char* files[] = {"Read.050"};
  make_plot(files[0],fn);
  //gSystem->Exit(0);
}
