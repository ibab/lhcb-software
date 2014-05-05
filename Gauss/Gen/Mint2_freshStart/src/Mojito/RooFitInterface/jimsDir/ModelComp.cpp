// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
void ModelComp()
{
  gROOT->LoadMacro("/userdisk/libby/macros/rootlogon.C");
  rootlogon();
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);
 gStyle->SetOptStat(0);
  
  TFile f1("hbook_rb0_modelplot.root");
  
  TCanvas* c1 = new TCanvas("c1","c1",700,900);
  
  c1->Divide(1,3);
  
  c1->cd(1);
  
  h1010->SetNormFactor(1.0);
  
  h1010->SetTitle("");
  h1010->SetXTitle("m^{2}_{+} (GeV^{2})");
  h1010->SetYTitle("|f(m^{2}_{+},m^{2}_{-})|^{2}/60 MeV^{2} (Arb. scale)");
  h1010->SetTitleSize(0.06,"X");
  h1010->SetTitleSize(0.06,"Y");
  h1010->SetLabelSize(0.06,"X");
  h1010->SetLabelSize(0.06,"Y");

  double nent = h1010->GetEntries();
  
  h1010->SetMaximum(0.06*nent);
  
  h1010->DrawCopy();
  

  c1->cd(2);

  h1020->SetNormFactor(1.0);
  
  h1020->SetTitle("");
  h1020->SetXTitle("m^{2}_{-} (GeV^{2})");
  h1020->SetYTitle("|f(m^{2}_{+},m^{2}_{-})|^{2}/60 MeV^{2} (Arb. scale)");
  h1020->SetTitleSize(0.06,"X");
  h1020->SetTitleSize(0.06,"Y");
  h1020->SetLabelSize(0.06,"X");
  h1020->SetLabelSize(0.06,"Y");
  
  h1020->DrawCopy();

  c1->cd(3);
  h1030->SetNormFactor(1.0);

  h1030->SetAxisRange(0.0,2.0);
  h1030->SetTitle("");
  h1030->SetXTitle("m^{2}_{#pi#pi} (GeV^{2})");
  h1030->SetYTitle("|f(m^{2}_{+},m^{2}_{-})|^{2}/60 MeV^{2} (Arb. scale)");
  h1030->SetTitleSize(0.06,"X");
  h1030->SetTitleSize(0.06,"Y");
  h1030->SetLabelSize(0.06,"X");
  h1030->SetLabelSize(0.06,"Y");

  nent = h1030->GetEntries();
  
  h1030->SetMaximum(0.07*nent);
  h1030->DrawCopy();
  
  TFile f2("DalitzHistoModelComp.root");

  c1->cd(1);


  
  
  hBminusm23sq->SetLineStyle(2);
  hBminusm23sq->SetNormFactor(1.0);
  hBminusm23sq->DrawCopy("same");
 
  TLine* line1 = new TLine(1.5,0.055,1.7,0.055);
  line1->Draw();
  TLine* line2 = new TLine(1.5,0.05,1.7,0.05);
  line2->SetLineStyle(2);
  line2->Draw();
  TText* lab = new TText(0.0,0.0,"");
  lab->SetTextAlign(12);
  lab->SetTextSize(0.06);
  

  lab->DrawText(1.75,0.055,"Belle");
  lab->DrawText(1.75,0.05,"BABAR");
  
  

  
  c1->cd(2);
  
  hBminusm12sq->SetLineStyle(2);
  hBminusm12sq->SetNormFactor(1.0);
  hBminusm12sq->DrawCopy("same");

  c1->cd(3);
  hBminusm13sq->SetAxisRange(0.0,2.0);
  hBminusm13sq->SetLineStyle(2);
  hBminusm13sq->SetNormFactor(1.0);
  hBminusm13sq->DrawCopy("same");
  
  


}
