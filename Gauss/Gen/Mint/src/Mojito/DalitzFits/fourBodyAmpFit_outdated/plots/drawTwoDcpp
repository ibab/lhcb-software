// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
void drawTwoD(std::string a="s12", std::string b="s23"){

  gROOT->Macro("style.C");
  
  TFile* f = new TFile("AmpPlots.root");
  
  TNtuple* ntp = f->Get("DalitzEventList_sij");

  TCanvas* cow = new TCanvas;
  ntp->Draw((a+":"+b).c_str());

  cow->Print( (a + "_vs_" + b + ".eps").c_str());
}
