const int nov = 13;

void alphaHigh2(){

 TCanvas *c2 = new TCanvas("c2", "c2",10,44,700,500);
   c2->SetGridx();

   // 0.12
   Float_t Values[nov] = {-0.63,  -0.546, -0.6, -0.496, -0.48,-0.53, 1.13,  -0.68, 1.42, 0.25, -0.71, 2., 0.3};
   Float_t Errors[nov]={0.165168 ,  0.07, 0.06, 0.005, 0.04,0.04, 0.36,  0.03, 0.2, 0.2, 0.07, 0.2, 0.2};
   Int_t Colors[nov]={1,1,1,1, 1,1,1,1 ,1,1, 1,1,1};
   char* Labels[nov] = { "#Upsilon(2S)", "#Upsilon(1S)", "#psi(2S)", "J/#psi" , "D^{0}", "#bar{D^{0}}", "#phi", "K_{s} (LL)", "#Lambda (LL)", "#bar{#Lambda} (LL)", "K_{s} (DD)", "#Lambda (DD)", "#bar{#Lambda} (DD)"};
 
  float MinX = -2.;
  float MaxX = 2.5;
  float MinY = 0.;
  float MaxY = nov + 1;

  
  TH2F* frame = new TH2F("frame","",10,MinX,MaxX,10,MinY,MaxY);   
  frame->SetStats(0);
  frame->Draw();
  frame->GetYaxis()->SetNdivisions(0);
  frame->GetXaxis()->SetLabelFont(132);
  frame->GetXaxis()->SetTitle("#alpha [per mille]");
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



