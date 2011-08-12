// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
void YearComp()
{
  TCanvas* c1 = new TCanvas("c1","c1",600,400);
  
  TFile* f04 = new TFile("fd04.root");
  
  HDpipi->SetNormFactor(1);
  
  HDpipi->DrawCopy();
  
  f04->Close();
  
  TFile* f05 = new TFile("fd05.root");

  HDpipi->SetNormFactor(1);
  HDpipi->SetLineColor(2);
  HDpipi->DrawCopy("same");
  
  f05->Close();

  TFile* f06 = new TFile("fd06.root");

  HDpipi->SetNormFactor(1);
  HDpipi->SetLineColor(4);
  HDpipi->DrawCopy("same");
  
  f06->Close();
  
  c1->Print("YearComp.eps","eps");
  

}
