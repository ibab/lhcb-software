// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
void dalitzforpaper()
{

  gROOT->LoadMacro("~/macros/rootlogon.C");
  rootlogon();
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);

  gStyle->SetOptStat(0);
  
  TCanvas* c1 = new TCanvas("c1","c1",800,400);
  
  TFile f("DalitzHisto.root");
  
  c1->Divide(2,1);
  
  c1->cd(1);
  
  m23sqVSm12sqPlus__m23sq_m12sq->SetTitle("");
  m23sqVSm12sqPlus__m23sq_m12sq->SetXTitle("m^{2}_{-} (GeV^{2})");
  m23sqVSm12sqPlus__m23sq_m12sq->SetYTitle("m^{2}_{+} (GeV^{2})");
  m23sqVSm12sqMinus__m23sq_m12sq->SetTitle("");
  m23sqVSm12sqMinus__m23sq_m12sq->SetYTitle("m^{2}_{-} (GeV^{2})");
  m23sqVSm12sqMinus__m23sq_m12sq->SetXTitle("m^{2}_{+} (GeV^{2})");
  
  m23sqVSm12sqPlus__m23sq_m12sq->SetMarkerStyle(20);
  m23sqVSm12sqPlus__m23sq_m12sq->SetMarkerSize(0.3);
  m23sqVSm12sqMinus__m23sq_m12sq->SetMarkerStyle(20);
  m23sqVSm12sqMinus__m23sq_m12sq->SetMarkerSize(0.3);

  m23sqVSm12sqMinus__m23sq_m12sq->GetXaxis()->SetTitleColor(1);
  m23sqVSm12sqPlus__m23sq_m12sq->GetXaxis()->SetTitleColor(1);
  m23sqVSm12sqMinus__m23sq_m12sq->GetYaxis()->SetTitleOffset(1.2);
  m23sqVSm12sqPlus__m23sq_m12sq->GetYaxis()->SetTitleOffset(1.2);
  

  TText* label = new TText(0.0,0.0,"a)");
  label->SetTextSize(0.075) ;
  
  
  m23sqVSm12sqPlus__m23sq_m12sq->DrawCopy();
  label->DrawText(3.0,3.0,"a)");
  
  c1->cd(2);
  
  m23sqVSm12sqMinus__m23sq_m12sq->DrawCopy();
  label->DrawText(3.0,3.0,"b)");


  
  

  return;
  

}
