#include "TCanvas.h"
#include "TLine.h"
#include "TLatex.h"
#include "TH2F.h"
#include "TBox.h"
#include <math.h>
#include <iostream>
#include "TMarker.h"

const int nov = 6;

void XComp(){

  TCanvas *c2 = new TCanvas("c2", "c2",10,44,700,500);
  c2->SetGridx();

  Float_t Values[nov] = { 3871.61, 3871.4, 3868.7, 3871.8, 3872.0 ,0};
  Float_t StatErrors[nov]={ 0.16, 0.6, 1.5, 3.1, 0.6, 0};
  Float_t SystErrors[nov]={ 0.19, 0.1, 0.4, 3.0, 0.5, 0};
  Float_t totErrors[nov]; Float_t Errors[nov];
  int i = 0;
  for (i = 0; i < nov -1 ; ++i ) {
    totErrors[i] = sqrt(StatErrors[i]*StatErrors[i]  + SystErrors[i]*SystErrors[i]); 
    Errors[i] = totErrors[i];
    std::cout << "Total error " <<  Errors[i] << std::endl;
  }

  // make the average - sum the 1/w^2
  float sumWeight = 0;
  Float_t w2[nov] ;
  for (i = 0; i < nov-1; ++i ) {
    w2[i] = 1/(Errors[i]*Errors[i]);
    sumWeight += w2[i]; 
  }

  // take the average
  float avg = 0;
  for ( i = 0; i < nov-1; ++i ) {
    avg += Values[i]*w2[i] ;
  }
  avg /= sumWeight;
  Errors[nov-1] = sqrt(1/sumWeight); 
  Values[nov-1]= avg;  

  std::cout << "Average mass " << avg << " +/- " << sqrt(1/sumWeight) << std::endl;

  Int_t Colors[nov]={1,1,1,1,1,1};
  char* Labels[nov] = {"CDF", "Babar B^{+}", "Babar B^{0}", "D0", "Belle", "Avg"};
 
  float MinX = 3860;
  float MaxX = 3880;
  float MinY = 0.;
  float MaxY = nov + 1;

  
  TH2F* frame = new TH2F("frame","",8, MinX,MaxX,10,MinY,MaxY);   
  frame->SetStats(0);
  frame->Draw();
  frame->GetYaxis()->SetNdivisions(0);
  frame->GetXaxis()->SetLabelFont(132);
  frame->GetXaxis()->SetTitle("X(3872) Mass [MeV/c^{2}]");
  frame->GetXaxis()->SetTitleFont(132);

  //  frame->Draw("axissame");
  TLatex label;
  label.SetTextAlign(22);
  label.SetTextSize(0.027);

  
  for (int i=0;i<nov;i++){
     Float_t x1=Values[i]-Errors[i];
     Float_t xc=Values[i];
     Float_t x2=Values[i]+Errors[i];
     Float_t y=nov-i;

     gStyle->SetLineColor(Colors[i]);
     gStyle->SetMarkerColor(Colors[i]);
     TMarker* pt=new TMarker(xc,y,20);
     TLine* l=new TLine(x1,y,x2,y);
     TLine* g1=new TLine(x1,y+0.1,x1,y-0.1);
     TLine* g2=new TLine(x2,y+0.1,x2,y-0.1);
     TLine* cp=new TLine(x2,y+0.1,x2,y-0.1);
     TBox* pav = new TBox(x1,y - 0.1,x2 , y + 0.1);
     pav->SetLineColor(0);
     pav->SetFillColor(91);
     if (i == nov-1) pav->SetFillColor(99);
     pav->Draw();
     l->Draw();
     pt->Draw();
     g1->Draw();
     //     pav->Draw();
     g2->Draw();
     label.DrawLatex(MinX+(MaxX-MinX)*0.1,y,Labels[i]);
  }

  frame->Draw("axissame");

}



