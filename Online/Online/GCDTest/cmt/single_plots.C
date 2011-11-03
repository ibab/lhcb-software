#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TPad.h"
#include <cstring>
#include <cstdio>

bool batch = false;

int input() {
  char c = 0;
  if ( !batch ) {
    printf("Hit return to continue:");
    fscanf(stdin,"%c",&c);
    printf("Input=%d\n",int(c));
  }
  return c;
}

void plot(TTree* t, int id, const char* title) {
  char cut[255];
  Int_t i, maxthread=35;
  Double_t *x = new Double_t[maxthread], *y = new Double_t[maxthread];
  TGraph* gr = 0;
  TH1F* f1 = 0;
  for(i=0; i<maxthread;++i) {
    x[i] = i+1;
    y[i] = 600.0/(i+1);
  }
  gr = new TGraph(maxthread,x,y);
  switch(id) {
  case 1000000:
    f1 = gPad->DrawFrame(0.0, 0.0, double(maxthread), 6000.0, title);
    break;
  case 1000001:
  case 1000002:
  case 1000003:
    f1 = gPad->DrawFrame(0.0, 10.0, double(maxthread), 700.0, title);
    f1->GetXaxis()->SetTitle("Max.number of threads");
    f1->GetYaxis()->SetTitle("Avg.execution time [msec]");
    f1->GetYaxis()->SetTitleOffset(1.2);
    gPad->SetLogy();
    gPad->SetCrosshair(2);
    gPad->SetGridy(1);
    break;
  }
  gr->Draw("C");
  t->SetLineWidth(5);
  for(i=10; i>0;--i) {
    int col = i==10 ? 1 : i;
    ::sprintf(cut,"id==%d&&parallel==%d&&threads<%d", id, i, maxthread);
    ::printf("Cut:%s Color:%d\n", cut, col);
    t->SetMarkerStyle(21+i);
    t->SetLineColor(col);
    t->Draw("mean/1000.0:threads", cut, "SAMECP");
  }
  gPad->Update();
  delete [] x;
  delete [] y;
}

TCanvas* makeSummary(const char* dir, int sweep) {
  char text[255];
  sprintf(text,"%s/sweep.%02d.root",dir,sweep);
  TFile* f = TFile::Open(text);
  if ( 0==f || f->IsZombie() ) {
    printf("The file:%s does not exist\n",text);
    TCanvas* canvas=new TCanvas("c1","Histos",10,10,700,900);
    return canvas;
  }
  TTree* t = (TTree*)f->Get("Summary");
  if ( 0 == t ) {
    printf("The tree:Summary does not exist in file:%s\n",text);
    TCanvas* canvas=new TCanvas("c1","Histos",10,10,700,900);
    return canvas;
  }

  t->Scan("*","parallel==5&&id>10000&&threads==41");
  TCanvas* canvas=new TCanvas("c1","Histos",10,10,500,500);
  sprintf(text,"Mean ellapsed time per event [msec]. Max %d instances/algorithm",sweep);
  plot(t,1000003,text);
  canvas->Update();
  f->Close();
  delete f;
  return canvas;
}

void plot_multiplicities() {
  int num_instances = 1;
  TCanvas* canvas = 0;

  num_instances = 1;
  canvas = makeSummary("sweep.cutoff-0musec",num_instances);
  canvas->Print("Max_1_algs_ALL.png","png");
  input();
  delete canvas;
  canvas = makeSummary("sweep.cutoff-10000musec",num_instances);
  canvas->Print("Max_1_algs_top7.png","png");
  input();
  delete canvas;
  canvas = makeSummary("sweep.cutoff-25000musec",num_instances);
  canvas->Print("Max_1_algs_top4.png","png");
  input();
  delete canvas;
  canvas = makeSummary("sweep.cutoff-200000musec",num_instances);
  canvas->Print("Max_1_algs_top1.png","png");
  input();
  num_instances = 2;
  canvas = makeSummary("sweep.cutoff-0musec",num_instances);
  canvas->Print("Max_2_algs_ALL.png","png");
  input();
  delete canvas;
  canvas = makeSummary("sweep.cutoff-10000musec",num_instances);
  canvas->Print("Max_2_algs_top7.png","png");
  input();
  delete canvas;
  canvas = makeSummary("sweep.cutoff-25000musec",num_instances);
  canvas->Print("Max_2_algs_top4.png","png");
  input();
  delete canvas;
  canvas = makeSummary("sweep.cutoff-200000musec",num_instances);
  canvas->Print("Max_2_algs_top1.png","png");
  input();
  num_instances = 10;
  canvas = makeSummary("sweep.cutoff-0musec",num_instances);
  canvas->Print("Max_10_algs_ALL.png","png");
  input();
  delete canvas;
  canvas = makeSummary("sweep.cutoff-10000musec",num_instances);
  canvas->Print("Max_10_algs_top7.png","png");
  input();
  delete canvas;
  canvas = makeSummary("sweep.cutoff-25000musec",num_instances);
  canvas->Print("Max_10_algs_top4.png","png");
  input();
  delete canvas;
  canvas = makeSummary("sweep.cutoff-200000musec",num_instances);
  canvas->Print("Max_10_algs_top1.png","png");
  input();
}

void single_plots() {
  batch = true;
  plot_multiplicities();
}
