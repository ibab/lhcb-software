// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
void pull()
{
  gROOT->LoadMacro("~/macros/rootlogon.C");
  rootlogon();
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);

  
  TChain* alltoys = new TChain("alltoys");

  char treename[128];
  

  for (int iexp=1003;iexp<=1050;iexp++){
    if (iexp != 1009 && iexp !=1010 && iexp !=1017 && iexp != 1022 
        && iexp != 1029 && iexp != 1036 && iexp != 1037 && iexp != 1038
        && iexp != 1042) {
            sprintf(treename,"kkknobkg_%d/KpipiResults.root/results",iexp);
      //      sprintf(treename,"/data/lhcb/lhcb2/libby/DalitzPullStudies/withbkg_%d/KpipiResults.root/results",iexp);
      alltoys->Add(treename);
      }
  }
  
  


  TCanvas* c1 = new TCanvas("c1","c1",900,300);
  c1->Divide(3,1);

  TCanvas* c2 = new TCanvas("c2","c2",900,300);
  c2->Divide(3,1);

  TCanvas* c3 = new TCanvas("c3","c3",900,300);
  c3->Divide(3,1);

  TText* dlab = new TText(0.1,0.9,"b)");
  dlab->SetTextSize(0.1);
  TText* elab = new TText(0.1,0.9,"d)");
  elab->SetTextSize(0.1);
  TText* flab = new TText(0.1,0.9,"f)");
  flab->SetTextSize(0.1);

  TH1F* hgam = new TH1F("hgam","",16,20.0,100.0);
  hgam->SetFillColor(5);
  hgam->SetXTitle("#gamma (#circ)");
  hgam->SetYTitle("Entries/5#circ");
  hgam->SetTitleSize(0.06,"X");
  hgam->SetTitleSize(0.06,"Y");
  

  
  TH1F* hegam = new TH1F("hegam","",30,0.0,30.0);
  hegam->SetFillColor(5);
  hegam->SetXTitle("#sigma_{#gamma} (#circ)");
  hegam->SetYTitle("Entries/1#circ");
  hegam->SetTitleSize(0.06,"X");
  hegam->SetTitleSize(0.06,"Y");

  TH1F* hpgam = new TH1F("hpgam","",14,-3.5,3.5);
  hpgam->SetFillColor(5);
  hpgam->SetXTitle("#gamma pull");
  hpgam->SetYTitle("Entries/0.5");
  hpgam->SetTitleSize(0.06,"X");
  hpgam->SetTitleSize(0.06,"Y");

  TH1F* hdel = new TH1F("hdel","",16,90.0,170.0);
  hdel->SetFillColor(5);
  hdel->SetXTitle("#delta_{B} (#circ)");
  hdel->SetYTitle("Entries/5#circ");
  hdel->SetTitleSize(0.06,"X");
  hdel->SetTitleSize(0.06,"Y");

  TH1F* hedel = new TH1F("hedel","",30,0.0,30.0);
  hedel->SetFillColor(5);
  hedel->SetXTitle("#sigma_{#delta_{B}} (#circ)");
  hedel->SetYTitle("Entries/1#circ");
  hedel->SetTitleSize(0.06,"X");
  hedel->SetTitleSize(0.06,"Y");

  TH1F* hpdel = new TH1F("hpdel","",14,-3.5,3.5);
  hpdel->SetFillColor(5);
  hpdel->SetXTitle("#delta_{B} pull");
  hpdel->SetYTitle("Entries/0.5");
  hpdel->SetTitleSize(0.06,"X");
  hpdel->SetTitleSize(0.06,"Y");

  TH1F* hrb = new TH1F("hrb","",20,0.00.0,20.0);
  hrb->SetFillColor(5);
  hrb->SetXTitle("r_{B} (#times10^{2})");
  hrb->SetYTitle("Entries/1.0");
  hrb->SetTitleSize(0.06,"X");
  hrb->SetTitleSize(0.06,"Y");

  TH1F* herb = new TH1F("herb","",40,2.0,4.0);
  herb->SetFillColor(5);
  herb->SetXTitle("#delta_{r_{B}} (#times10^{2})");
  herb->SetYTitle("Entries/0.05");
  herb->SetTitleSize(0.06,"X");
  herb->SetTitleSize(0.06,"Y");

  TH1F* hprb = new TH1F("hprb","",14,-3.5,3.5);
  hprb->SetFillColor(5);
  hprb->SetXTitle("r_{B} pull");
  hprb->SetYTitle("Entries/0.5");
  hprb->SetTitleSize(0.06,"X");
  hprb->SetTitleSize(0.06,"Y");
  

  gStyle->SetOptStat(1100);
  
  
  c1->cd(1);
  alltoys->Project("hgam","gam");
  hgam->Draw();

  dlab->DrawText(0.05*((hgam->GetXaxis()->GetXmax()-hgam->GetXaxis()->GetXmin()))+hgam->GetXaxis()->GetXmin(),hgam->GetMaximum()*0.9,"b)");

  c1->cd(2);
  alltoys->Project("hegam","egam");
  hegam->Draw();
  elab->DrawText(0.05*((hegam->GetXaxis()->GetXmax()-hegam->GetXaxis()->GetXmin()))+hegam->GetXaxis()->GetXmin(),hegam->GetMaximum()*0.9,"d)"); 
 
  c1->cd(3);
  alltoys->Project("hpgam","(gam-61.0)/egam");
  hpgam->Draw();
  elab->DrawText(0.05*((hpgam->GetXaxis()->GetXmax()-hpgam->GetXaxis()->GetXmin()))+hpgam->GetXaxis()->GetXmin(),hpgam->GetMaximum()*0.9,"f)");  


  c1->Print("fig11bdf.eps","eps");
  


  c2->cd(1);
  alltoys->Project("hrb","rb*100.0");
  hrb->Draw();
  dlab->DrawText(0.05*((hrb->GetXaxis()->GetXmax()-hrb->GetXaxis()->GetXmin()))+hrb->GetXaxis()->GetXmin(),hrb->GetMaximum()*0.9,"b)");

  
  
  c2->cd(2);
  alltoys->Project("herb","erb*100");
  herb->Draw();
  dlab->DrawText(0.05*((herb->GetXaxis()->GetXmax()-herb->GetXaxis()->GetXmin()))+herb->GetXaxis()->GetXmin(),herb->GetMaximum()*0.9,"d)");  
  
  c2->cd(3);
  alltoys->Project("hprb","(rb-0.1)/erb");
  hprb->Draw();
  dlab->DrawText(0.05*((hprb->GetXaxis()->GetXmax()-hprb->GetXaxis()->GetXmin()))+hprb->GetXaxis()->GetXmin(),hprb->GetMaximum()*0.9,"f)");  
  
  c2->Print("fig12bdf.eps","eps");

  
  c3->cd(1);
  alltoys->Project("hdel","del");
  
  hdel->Draw();
  dlab->DrawText(0.05*((hdel->GetXaxis()->GetXmax()-hdel->GetXaxis()->GetXmin()))+hdel->GetXaxis()->GetXmin(),hdel->GetMaximum()*0.9,"b)");  
  
  c3->cd(2);
  alltoys->Project("hedel","edel");
  hedel->Draw();
  dlab->DrawText(0.05*((hedel->GetXaxis()->GetXmax()-hedel->GetXaxis()->GetXmin()))+hedel->GetXaxis()->GetXmin(),hedel->GetMaximum()*0.9,"d)");  

  c3->cd(3);
  alltoys->Project("hpdel","(del-130.0)/edel");
  hpdel->Draw();
  dlab->DrawText(0.05*((hpdel->GetXaxis()->GetXmax()-hpdel->GetXaxis()->GetXmin()))+hpdel->GetXaxis()->GetXmin(),hpdel->GetMaximum()*0.9,"f)");  

  c3->Print("fig13bdf.eps","eps");


  return;
  
}
