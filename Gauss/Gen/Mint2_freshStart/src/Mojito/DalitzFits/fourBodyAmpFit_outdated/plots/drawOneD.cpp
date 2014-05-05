// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
void drawOneD(std::string a="s12"){

  gROOT->Macro("style.C");
  
  TFile* f = new TFile("AmpPlots.root");
  
  TNtuple* ntp = f->Get("DalitzEventList_sij");

  TCanvas* cow = new TCanvas;
  ntp->Draw((a + ">> h" + a).c_str());

  TH1F* h = gDirectory->Get(("h" + a).c_str());
  //  h->Rebin(2);

  cow->Print( (a + ".eps").c_str());
}
