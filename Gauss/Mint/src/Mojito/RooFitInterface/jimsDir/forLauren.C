// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
void forLauren()
{
  gROOT->LoadMacro("/userdisk/libby/macros/rootlogon.C");
  rootlogon();
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetOptStat(0);
  

  TFile f("fd.root");

  HDpipi->SetTitle("");
  HDpipi->GetXaxis()->SetTitleColor(1);
  
  HDpipi->SetXTitle("m_{#pi#pi}^{2} (GeV^{2}/c^{4})");
  HDpipi->SetYTitle("|Sum(BW)|");
  
  HDpipi->DrawCopy("c");
  c1->Print("AmpBWSwaveAndNonRes.eps","eps");
  HDpipi2->SetTitle("");
  HDpipi2->GetXaxis()->SetTitleColor(1);
  
  HDpipi2->SetXTitle("m_{#pi#pi}^{2} (GeV^{2}/c^{4})");
  HDpipi2->SetYTitle("Arg(Sum(BW)) (rad)");
  
  HDpipi2->DrawCopy("c");
  c1->Print("PhaseBWSwaveAndNonRes.eps","eps");
  
}

  
  
