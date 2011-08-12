// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
void dalitz_for_pparc()
{
  gROOT->LoadMacro("~/rootlogon.C");
  rootlogon();
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1,0);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);
  
  TFile *_file0 = TFile::Open("fd.root");
  
  TCanvas* c1 = new TCanvas("c1","c1",600,600);
  
  HD->SetTitle("");

  HD->GetXaxis()->SetTitleColor(1);
  HD->SetTitleSize(0.05,"X");
  HD->SetLabelSize(0.05,"X");
  HD->SetTitleOffset(1.2,"X");
  HD->SetXTitle("m^{2}_{K^{0}_{S}#pi^{+}} (GeV^{2}/c^{4})");

  HD->GetYaxis()->SetTitleColor(1);
  HD->SetTitleSize(0.05,"Y");
  HD->SetLabelSize(0.05,"Y");
  HD->SetTitleOffset(1.4,"Y");
  
  HD->SetYTitle("m^{2}_{K^{0}_{S}#pi^{-}} (GeV^{2}/c^{4})");
  

  HD->Draw();
  
  c1->Print("D0.gif","gif");
  
  
  HDbar->SetTitle("");

  HDbar->GetXaxis()->SetTitleColor(1);
  HDbar->SetTitleSize(0.05,"X");
  HDbar->SetLabelSize(0.05,"X");
  HDbar->SetTitleOffset(1.2,"X");
  HDbar->SetXTitle("m^{2}_{K^{0}_{S}#pi^{+}} (GeV^{2}/c^{4})");

  HDbar->GetYaxis()->SetTitleColor(1);
  HDbar->SetTitleSize(0.05,"Y");
  HDbar->SetLabelSize(0.05,"Y");
  HDbar->SetTitleOffset(1.4,"Y");
  
  HDbar->SetYTitle("m^{2}_{K^{0}_{S}#pi^{-}} (GeV^{2}/c^{4})");
  

  HDbar->Draw();
  
  c1->Print("D0bar.gif","gif");

  HDCP_E->SetTitle("");

  HDCP_E->GetXaxis()->SetTitleColor(1);
  HDCP_E->SetTitleSize(0.05,"X");
  HDCP_E->SetLabelSize(0.05,"X");
  HDCP_E->SetTitleOffset(1.2,"X");
  HDCP_E->SetXTitle("m^{2}_{K^{0}_{S}#pi^{+}} (GeV^{2}/c^{4})");

  HDCP_E->GetYaxis()->SetTitleColor(1);
  HDCP_E->SetTitleSize(0.05,"Y");
  HDCP_E->SetLabelSize(0.05,"Y");
  HDCP_E->SetTitleOffset(1.4,"Y");
  
  HDCP_E->SetYTitle("m^{2}_{K^{0}_{S}#pi^{-}} (GeV^{2}/c^{4})");
  

  HDCP_E->Draw();
  
  c1->Print("D0CP_E.gif","gif");

  HDCP_O->SetTitle("");

  HDCP_O->GetXaxis()->SetTitleColor(1);
  HDCP_O->SetTitleSize(0.05,"X");
  HDCP_O->SetLabelSize(0.05,"X");
  HDCP_O->SetTitleOffset(1.2,"X");
  HDCP_O->SetXTitle("m^{2}_{K^{0}_{S}#pi^{+}} (GeV^{2}/c^{4})");

  HDCP_O->GetYaxis()->SetTitleColor(1);
  HDCP_O->SetTitleSize(0.05,"Y");
  HDCP_O->SetLabelSize(0.05,"Y");
  HDCP_O->SetTitleOffset(1.4,"Y");
  
  HDCP_O->SetYTitle("m^{2}_{K^{0}_{S}#pi^{-}} (GeV^{2}/c^{4})");
  

  HDCP_O->Draw();
  
  c1->Print("D0CP_O.gif","gif");

  HBPlus->SetTitle("");

  HBPlus->GetXaxis()->SetTitleColor(1);
  HBPlus->SetTitleSize(0.05,"X");
  HBPlus->SetLabelSize(0.05,"X");
  HBPlus->SetTitleOffset(1.2,"X");
  HBPlus->SetXTitle("m^{2}_{K^{0}_{S}#pi^{+}} (GeV^{2}/c^{4})");

  HBPlus->GetYaxis()->SetTitleColor(1);
  HBPlus->SetTitleSize(0.05,"Y");
  HBPlus->SetLabelSize(0.05,"Y");
  HBPlus->SetTitleOffset(1.4,"Y");
  
  HBPlus->SetYTitle("m^{2}_{K^{0}_{S}#pi^{-}} (GeV^{2}/c^{4})");
  

  HBPlus->Draw();
  
  c1->Print("BPlus.gif","gif");

  HBMinus->SetTitle("");

  HBMinus->GetXaxis()->SetTitleColor(1);
  HBMinus->SetTitleSize(0.05,"X");
  HBMinus->SetLabelSize(0.05,"X");
  HBMinus->SetTitleOffset(1.2,"X");
  HBMinus->SetXTitle("m^{2}_{K^{0}_{S}#pi^{-}} (GeV^{2}/c^{4})");

  HBMinus->GetYaxis()->SetTitleColor(1);
  HBMinus->SetTitleSize(0.05,"Y");
  HBMinus->SetLabelSize(0.05,"Y");
  HBMinus->SetTitleOffset(1.4,"Y");
  
  HBMinus->SetYTitle("m^{2}_{K^{0}_{S}#pi^{+}} (GeV^{2}/c^{4})");
  

  HBMinus->Draw();
  
  c1->Print("BMinus.gif","gif");


}

