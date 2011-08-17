// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
void makeProjections()
{
  gROOT->LoadMacro("~/rootlogon.C");
  rootlogon();

  gStyle->SetOptStat(0);
  

  TFile* f = new TFile("DalitzHisto.root");

  TCanvas*c1 = new TCanvas("c1","c1",700,500);

  hBplusm12sq->Rebin(2);
  hCombplusm12sq->Rebin(2);
  hDpiplusm12sq->Rebin(2);

  TH1F* allplusm12sq = new TH1F("all","all",75,0.0,3.0);
  TH1F* bkgplusm12sq = new TH1F("all","all",75,0.0,3.0);
  
  for (Int_t ibin = 1; ibin<=hBplusm12sq->GetNbinsX();ibin++){
    Double_t allsum = hBplusm12sq->GetBinContent(ibin) +
      hCombplusm12sq->GetBinContent(ibin)+
      hDpiplusm12sq->GetBinContent(ibin);
    Double_t bkgsum = hCombplusm12sq->GetBinContent(ibin)+
      hDpiplusm12sq->GetBinContent(ibin);
    allplusm12sq->SetBinContent(ibin,allsum);
    bkgplusm12sq->SetBinContent(ibin,bkgsum);
  }
  
  
  allplusm12sq->SetTitle("B^{+} - m^{2}_{-}");
  

  allplusm12sq->Draw();
  bkgplusm12sq->SetFillColor(5);
  bkgplusm12sq->Draw("same");
  hDpiplusm12sq->SetFillColor(4);
  hDpiplusm12sq->Draw("same");
  
  c1->Print("m12plus.eps","eps");
  
  hBplusm23sq->Rebin(2);
  hCombplusm23sq->Rebin(2);
  hDpiplusm23sq->Rebin(2);

  TH1F* allplusm23sq = new TH1F("all","all",75,0.0,3.0);
  TH1F* bkgplusm23sq = new TH1F("all","all",75,0.0,3.0);
  
  for (Int_t ibin = 1; ibin<=hBplusm23sq->GetNbinsX();ibin++){
    Double_t allsum = hBplusm23sq->GetBinContent(ibin) +
      hCombplusm23sq->GetBinContent(ibin)+
      hDpiplusm23sq->GetBinContent(ibin);
    Double_t bkgsum = hCombplusm23sq->GetBinContent(ibin)+
      hDpiplusm23sq->GetBinContent(ibin);
    allplusm23sq->SetBinContent(ibin,allsum);
    bkgplusm23sq->SetBinContent(ibin,bkgsum);
  }
  
  
  allplusm23sq->SetTitle("B^{+} - m^{2}_{+}");
  

  allplusm23sq->Draw();
  bkgplusm23sq->SetFillColor(5);
  bkgplusm23sq->Draw("same");
  hDpiplusm23sq->SetFillColor(4);
  hDpiplusm23sq->Draw("same");
  
  c1->Print("m23plus.eps","eps");


  hBplusm13sq->Rebin(2);
  hCombplusm13sq->Rebin(2);
  hDpiplusm13sq->Rebin(2);

  TH1F* allplusm13sq = new TH1F("all","all",50,0.0,2.0);
  TH1F* bkgplusm13sq = new TH1F("all","all",50,0.0,2.0);
  
  for (Int_t ibin = 1; ibin<=hBplusm13sq->GetNbinsX();ibin++){
    Double_t allsum = hBplusm13sq->GetBinContent(ibin) +
      hCombplusm13sq->GetBinContent(ibin)+
      hDpiplusm13sq->GetBinContent(ibin);
    Double_t bkgsum = hCombplusm13sq->GetBinContent(ibin)+
      hDpiplusm13sq->GetBinContent(ibin);
    allplusm13sq->SetBinContent(ibin,allsum);
    bkgplusm13sq->SetBinContent(ibin,bkgsum);
  }
  
  
  allplusm13sq->SetTitle("B^{+} - m^{2}_{#pi#pi}");
  

  allplusm13sq->Draw();
  bkgplusm13sq->SetFillColor(5);
  bkgplusm13sq->Draw("same");
  hDpiplusm13sq->SetFillColor(4);
  hDpiplusm13sq->Draw("same");
  
  c1->Print("m13plus.eps","eps");
  
  hBminusm12sq->Rebin(2);
  hCombminusm12sq->Rebin(2);
  hDpiminusm12sq->Rebin(2);

  TH1F* allminusm12sq = new TH1F("all","all",75,0.0,3.0);
  TH1F* bkgminusm12sq = new TH1F("all","all",75,0.0,3.0);
  
  for (Int_t ibin = 1; ibin<=hBminusm12sq->GetNbinsX();ibin++){
    Double_t allsum = hBminusm12sq->GetBinContent(ibin) +
      hCombminusm12sq->GetBinContent(ibin)+
      hDpiminusm12sq->GetBinContent(ibin);
    Double_t bkgsum = hCombminusm12sq->GetBinContent(ibin)+
      hDpiminusm12sq->GetBinContent(ibin);
    allminusm12sq->SetBinContent(ibin,allsum);
    bkgminusm12sq->SetBinContent(ibin,bkgsum);
  }
  
  
  allminusm12sq->SetTitle("B^{-} - m^{2}_{-}");
  

  allminusm12sq->Draw();
  bkgminusm12sq->SetFillColor(5);
  bkgminusm12sq->Draw("same");
  hDpiminusm12sq->SetFillColor(4);
  hDpiminusm12sq->Draw("same");
  
  c1->Print("m12minus.eps","eps");
  
  hBminusm23sq->Rebin(2);
  hCombminusm23sq->Rebin(2);
  hDpiminusm23sq->Rebin(2);

  TH1F* allminusm23sq = new TH1F("all","all",75,0.0,3.0);
  TH1F* bkgminusm23sq = new TH1F("all","all",75,0.0,3.0);
  
  for (Int_t ibin = 1; ibin<=hBminusm23sq->GetNbinsX();ibin++){
    Double_t allsum = hBminusm23sq->GetBinContent(ibin) +
      hCombminusm23sq->GetBinContent(ibin)+
      hDpiminusm23sq->GetBinContent(ibin);
    Double_t bkgsum = hCombminusm23sq->GetBinContent(ibin)+
      hDpiminusm23sq->GetBinContent(ibin);
    allminusm23sq->SetBinContent(ibin,allsum);
    bkgminusm23sq->SetBinContent(ibin,bkgsum);
  }
  
  
  allminusm23sq->SetTitle("B^{-} - m^{2}_{+}");
  

  allminusm23sq->Draw();
  bkgminusm23sq->SetFillColor(5);
  bkgminusm23sq->Draw("same");
  hDpiminusm23sq->SetFillColor(4);
  hDpiminusm23sq->Draw("same");
  
  c1->Print("m23minus.eps","eps");


  hBminusm13sq->Rebin(2);
  hCombminusm13sq->Rebin(2);
  hDpiminusm13sq->Rebin(2);

  TH1F* allminusm13sq = new TH1F("all","all",50,0.0,2.0);
  TH1F* bkgminusm13sq = new TH1F("all","all",50,0.0,2.0);
  
  for (Int_t ibin = 1; ibin<=hBminusm13sq->GetNbinsX();ibin++){
    Double_t allsum = hBminusm13sq->GetBinContent(ibin) +
      hCombminusm13sq->GetBinContent(ibin)+
      hDpiminusm13sq->GetBinContent(ibin);
    Double_t bkgsum = hCombminusm13sq->GetBinContent(ibin)+
      hDpiminusm13sq->GetBinContent(ibin);
    allminusm13sq->SetBinContent(ibin,allsum);
    bkgminusm13sq->SetBinContent(ibin,bkgsum);
  }
  
  
  allminusm13sq->SetTitle("B^{-} - m^{2}_{#pi#pi}");
  

  allminusm13sq->Draw();
  bkgminusm13sq->SetFillColor(5);
  bkgminusm13sq->Draw("same");
  hDpiminusm13sq->SetFillColor(4);
  hDpiminusm13sq->Draw("same");
  
  c1->Print("m13minus.eps","eps");
  

  
  
  
}
