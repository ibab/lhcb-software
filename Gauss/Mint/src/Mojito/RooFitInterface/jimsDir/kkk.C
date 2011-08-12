// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
void kkk()
{
  gROOT->LoadMacro("~/macros/rootlogon.C");
  rootlogon();
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);

  TFile f("fd.root");

  TCanvas* c1 = new TCanvas("c1","c1",600,600);
  c1->Divide(2,2);
  
  HD->SetTitle("");
  HDKK->SetTitle("");
  gStyle->SetOptStat(0);
  
  
  c1->cd(1);
  HD->Draw();

  c1->cd(2);
  
  HD->ProjectionX("m12");
  m12->Draw();
  
  c1->cd(3);
  
  HD->ProjectionY("m23");
  m23->Draw();

  c1->cd(4);
  
  HDKK->Draw();
  

  c1->Print("KKKcomp.eps");
  

  TCanvas* c2 = new TCanvas("c2","c2",600,300);
  
  c2->Divide(2,1);
  HBPlus->SetTitle("");
  HBMinus->SetTitle("");
  
  HBPlus->ProjectionX("m12plus");
  HBMinus->ProjectionX("m12minus");
  HBPlus->ProjectionY("m23plus");
  HBMinus->ProjectionY("m23minus");
  
  c2->cd(1);
  
  
  m12plus->Draw();
  m12minus->SetLineColor(2);
  m12minus->Draw("same");
  
  c2->cd(2);
  

  m23plus->Draw();
  m23minus->SetLineColor(2);
  m23minus->Draw("same");

  c2->Print("KKKb.eps","eps");
  

  return;
  

}
