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
  for(i=0; i<maxthread;++i) {
    x[i] = i+1;
    y[i] = 600.0/(i+1);
  }
  gr = new TGraph(maxthread,x,y);
  switch(id) {
  case 1000000:
    gPad->DrawFrame(0.0, 0.0, double(maxthread), 6000.0, title);
    break;
  case 1000001:
  case 1000002:
  case 1000003:
    gPad->DrawFrame(0.0, 10.0, double(maxthread), 700.0, title);
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

void makeSummary(const char* dir, int sweep) {
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
  TCanvas* canvas=new TCanvas("c1","Histos",10,10,700,900);
  canvas->Divide(2,2);
  canvas->cd(1);
  sprintf(text,"Mean processing time per event [msec]. Max %d instances/algorithm",sweep);
  plot(t,1000000,text);
  canvas->cd(2);
  sprintf(text,"Mean ellapsed time per event [msec]. Max %d instances/algorithm",sweep);
  plot(t,1000003,text);
  canvas->cd(3);
  sprintf(text,"Time difference between event submission [msec]. Max %d instances/algorithm",sweep);
  plot(t,1000001,text);
  canvas->cd(4);
  sprintf(text,"Time difference between event completion [msec]. Max %d instances/algorithm",sweep);
  plot(t,1000002,text);
  canvas->Update();
  f->Close();
  delete f;
  return canvas;
}

void plot_multiplicities(const char* dir) {
  char opt[256], text[256];
  for(int first=1,last=10,i=first; i<=last;++i) {
    TCanvas* canvas = makeSummary(dir,i);
    sprintf(text,"Title:Max %d concurrent algs/type",i);
    if ( i==first )
      sprintf(opt,"%s/%s.pdf(",dir,dir);
    else if ( i == last ) 
      sprintf(opt,"%s/%s.pdf)",dir,dir);
    else 
      sprintf(opt,"%s/%s.pdf",dir,dir);
    canvas->Print(opt,text);
    input();
    delete canvas;
    //break;
  }
}

void plot_ellapsed_time(TTree* t, float cut_val, int color) {
  char cut[255], text[255];
  int i=1;
  int col = i==10 ? 1 : i;
  int maxthread=15;

  ::sprintf(cut,"parallel==%d&&threads==%d&&id<1000", i, maxthread);
  gPad->SetLogy();
  gPad->SetLogx();
  gPad->DrawFrame(0.9, 0.1, 260.0, 260.0, "Mean ellapsed time [msec] / algorithm");

  TLine* l=new TLine(cut_val,0.1,cut_val,260);
  l->SetLineWidth(5);
  l->SetLineColor(kRed);
  l->Draw();
  TArrow *arr=new TArrow(cut_val,0.3,50,0.3,0.02,"|>");
  arr->SetLineColor(color);
  arr->SetFillColor(color);
  arr->SetLineWidth(5);
  arr->Draw();

  arr=new TArrow(4,100.,35,100,0.03,"<|>");
  arr->SetLineColor(kGreen+2);
  arr->SetFillColor(kGreen+2);
  arr->SetLineWidth(5);
  arr->Draw();

  t->SetLineWidth(2);
  t->SetMarkerStyle(21+i);
  t->SetLineColor(col);
  t->Draw("mean/1000.0", cut, "SAMEAH");
  gPad->SetCrosshair(2);
  gPad->SetGridy(1);
  gPad->Update();
}

void plot_ellapsed_time2(TTree* t) {
  char cut[255], text[255];
  int i=1, sweep=5;
  int col = i==10 ? 1 : i;
  int maxthread=15;

  ::sprintf(cut,"parallel==%d&&threads==%d&&id<1000", i, maxthread);
  gPad->SetLogy();
  gPad->SetLogx();
  gPad->DrawFrame(0.9, 0.1, 260.0, 260.0, "Mean ellapsed time [msec] / algorithm");

  TLatex latex;
  TLine* l = new TLine(0.9,0.1,0.9,260);
  l->SetLineWidth(5);
  l->SetLineColor(kRed-10);
  l->Draw();
  latex.DrawLatex(1.0,0.12,"All algs.reentrant");

  l = new TLine(10,0.1,10,260);
  l->SetLineWidth(5);
  l->SetLineColor(kRed-9);
  l->Draw();
  latex.DrawLatex(11.,12.,"Top 7");

  l = new TLine(25,0.1,25,260);
  l->SetLineWidth(5);
  l->SetLineColor(kRed-6);
  l->Draw();
  latex.DrawLatex(27.,12.,"Top 4");

  l = new TLine(50,0.1,50,260);
  l->SetLineWidth(5);
  l->SetLineColor(kRed-2);
  l->Draw();
  latex.DrawLatex(53.,12.,"Top 1");

  t->SetLineWidth(2);
  t->SetMarkerStyle(21+i);
  t->SetLineColor(col);
  t->Draw("mean/1000.0", cut, "SAMEAH");
  gPad->SetCrosshair(2);
  gPad->SetGridy(1);
  gPad->Update();
}

void plot_ellapsed_vs_id(TTree* t)  {
  int i = 1, maxthread = 15;
  int col = i==10 ? 1 : i;
  char cut[255];

  ::sprintf(cut,"parallel==%d&&threads==%d&&id<1000", i, maxthread);
  gPad->SetLogy();
  gPad->SetCrosshair(2);
  gPad->SetGridy(1);
  gPad->DrawFrame(0.0, 0.9, 260.0, 260.0, "Algorithm Seq.No. vs. Mean ellapsed time [msec]");
  TLine* l=new TLine(0,10.5,260,10.5);
  l->SetLineWidth(5);
  l->SetLineColor(kRed);
  l->Draw();
  TArrow *arr=new TArrow(20,10.5,20,50,0.02,"|>");
  arr->SetLineColor(kRed);
  arr->SetFillColor(kRed);
  arr->SetLineWidth(5);
  arr->Draw();
  arr=new TArrow(240,10.5,240,50,0.02,"|>");
  arr->SetLineColor(kRed);
  arr->SetFillColor(kRed);
  arr->SetLineWidth(5);
  arr->Draw();
  t->SetLineWidth(2);
  t->SetMarkerStyle(21+i);
  t->SetLineColor(col);
  t->Draw("mean/1000.0:id", cut,"SAMEP");
  //gPad->Update();
}

TCanvas* plot_algorithm_times() {
  char text[255];
  int sweep=5, maxthread=27;
  sprintf(text,"sweep.1/sweep.%02d.root",sweep);
  TFile* f = TFile::Open(text);
  TTree* t = (TTree*)f->Get("Summary");

  TCanvas* canvas = new TCanvas("c1","Histos",10,10,700,700);
  canvas->Divide(1,2);
  TPad* p = canvas->cd(1);
  p->Divide(2,1);
  p->cd(1);
  plot_ellapsed_time(t,10.0,kRed);
  p->cd(2);
  plot_ellapsed_vs_id(t);
  canvas->cd(2);
  plot_ellapsed_time2(t);
  f->Close();
  delete f;
  return canvas;
}

void plot1_ellapsed_vs_threads(int id, const char* title) {
  TLine* l;
  TLatex *txt;
  TArrow* arr;
  char cut[255];
  int  sweep = 5, maxthread=27;

  gPad->DrawFrame(0.0, 30.0, double(maxthread)-2, 1000.0, title);
  gPad->SetLogy();
  gPad->SetCrosshair(2);
  gPad->SetGridy(1);
  txt = new TLatex(12.2,300,"N_{Evt}=1");
  txt->Draw();
  txt = new TLatex(12.2,150,"N_{Evt}=2");
  txt->Draw();
  txt = new TLatex(12.2,100,"N_{Evt}=3");
  txt->Draw();
  txt = new TLatex(25.2,90,"...");
  txt->Draw();
  l = new TLine(0,90,10,90);
  l->SetLineWidth(5);
  l->SetLineColor(kBlue);
  l->Draw();
  arr=new TArrow(5,90,5,40,0.02,"|>");
  arr->SetLineColor(kBlue);
  arr->SetFillColor(kBlue);
  arr->SetLineWidth(5);
  arr->Draw();

  l = new TLine(0,125,7.5,125);
  l->SetLineWidth(5);
  l->SetLineColor(kCyan);
  l->Draw();
  arr=new TArrow(3,125,3,40,0.02,"|>");
  arr->SetLineColor(kCyan);
  arr->SetFillColor(kCyan);
  arr->SetLineWidth(5);
  arr->Draw();

  for(sweep=0;sweep<=3;++sweep) {
    char text[255];
    sprintf(text,"sweep.%d/sweep.05.root",sweep);
    TFile* f = TFile::Open(text);
    if ( f && !f->IsZombie() ) {
      TTree* t = (TTree*)f->Get("Summary");
      for(int par=1; par<10; ++par) {
	::sprintf(cut,"id==%d&&parallel==%d&&threads<%d", id, par, maxthread);
	t->SetLineWidth(5);
	t->SetMarkerStyle(21+sweep);
	t->SetLineColor(sweep+1);
	t->Draw("mean/1000.0:threads", cut, "SAMEC");
      }
      f->Close();
      delete f;
    }
  }
}

void plot_alg_times() {
  TCanvas* canvas = plot_algorithm_times();
  canvas->Print("alg_times.pdf(","Title:Algorithm times");
  canvas->Print("alg_times.png","png");
  input();
  delete canvas;  
  canvas=new TCanvas("c1","Histos",10,10,700,700);
  plot1_ellapsed_vs_threads(1000003,"No.of threads vs. Mean ellapsed time/event [msec]");
  canvas->Print("alg_times.pdf)","Title:Event time vs. threads");
  canvas->Print("event_times.png","png");
  input();
  delete canvas;
}

void ana() {
  batch = false;
  //plot_multiplicities("sweep.cutoff-0musec");
  //plot_multiplicities("sweep.cutoff-10000musec");
  //plot_multiplicities("sweep.cutoff-25000musec");
  //plot_multiplicities("sweep.cutoff-200000musec");
  plot_alg_times();
}
