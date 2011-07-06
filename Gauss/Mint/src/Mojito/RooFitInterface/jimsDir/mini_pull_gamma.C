// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
void mini_pull_gamma()
{
  gROOT->LoadMacro("~/rootlogon.C");
  rootlogon();

  const int NTOYS = 44;
  
  
  double gamma[NTOYS] = 
    {
      63.77, 64.28, 61.03, 70.95, 64.48, 42.84, 45.58, 78.64, 74.70, 46.35,
      40.08, 28.01, 38.01, 26.36, 61.53, 83.75, 63.09, 62.47, 51.79, 66.01,
      57.78, 63.29, 85.10, 75.23, 80.08, 57.31, 78.64, 65.32, 62.02, 53.17,
      57.65, 86.98, 67.96, 40.71, 45.69, 77.91, 68.13, 77.26, 58.18, 67.01,
      49.03, 62.30, 37.31, 56.54
    };
  
  double egamma[NTOYS] = 
    {
      10.76,  6.84,  7.96, 11.69, 10.76, 16.56, 14.17, 12.26, 11.08, 10.33,
      16.81,  8.91, 11.14, 12.24, 15.32,  8.89, 10.90,  8.76, 11.49, 11.36,
      8.10, 18.89, 11.17,  9.70,  6.87,  8.06, 10.20, 10.62, 13.44, 10.71,
      9.12, 12.09,  9.58,  9.96, 10.66,  9.46, 11.97, 12.17,  9.31, 11.91,
      13.47, 8.73,  10.92, 11.32
  };

  double rB[NTOYS] = 
    { 
      0.0959, 0.1512, 0.1370, 0.1005, 0.1173, 0.0643, 0.0791, 0.0996, 0.0989, 0.1032, 
      0.0658, 0.1134, 0.1037, 0.1037, 0.1000, 0.1180, 0.0967, 0.1165, 0.0980, 0.0864,
      0.1174, 0.0663, 0.0963, 0.1167, 0.1505, 0.1336, 0.1135, 0.1034, 0.0868, 0.1011,
      0.1202, 0.0803, 0.1107, 0.1053, 0.1027, 0.1165, 0.0854, 0.0937, 0.1228  0.0978,
      0.0807, 0.1246, 0.1045, 0.0949   
   };
  
  double erB[NTOYS] = 
    {
      0.0179, 0.0191, 0.0186, 0.0180, 0.0180, 0.0174, 0.0185, 0.0200, 0.0193, 0.0177,
      0.0189, 0.0182, 0.0183, 0.0195, 0.0197, 0.0192, 0.0184, 0.0176, 0.0186, 0.0195,
      0.0165, 0.0174, 0.0209, 0.0174, 0.0195, 0.0179, 0.0174, 0.0174, 0.0185, 0.0175,
      0.0187, 0.0209, 0.0175, 0.0182, 0.0178, 0.0188, 0.0201, 0.0188, 0.0182, 0.0170,
      0.0178, 0.0173, 0.0185, 0.0191
 };

  double delB[NTOYS] = 
    { 
      131.76, 145.44, 137.51, 117.71, 122.67, 128.28, 150.04, 130.93, 135.71, 119.53, 
      146.53, 121.86, 129.24, 123.12, 132.79, 138.30, 134.96, 136.12, 145.48, 153.88,
      136.89, 100.29, 144.02, 117.48, 147.90, 136.40, 120.96, 118.88, 128.51, 129.85,
      132.28, 159.28, 129.11, 135.14, 135.86, 130.45, 150.05, 127.00, 133.29, 107.23,
      127.06, 123.00, 115.92, 141.19
   };
  
  double edelB[NTOYS] = 
    {
       10.93,  6.94,    8.07,  11.88,  10.86,  16.51,  14.02,  12.70,  11.42,  10.24,
       16.72,  8.91,   11.12,  12.24,  15.57,   9.20,  11.06,  8.88,   11.48,  11.43,
        8.15, 18.56,   11.64,   9.87,   7.06,   8.13,  10.44,  10.67,  13.63,  10.74,
       9.22, 12.26,    9.73,   9.95,  10.66,    9.83,  12.12,  12.59,   9.43,  11.89,
       13.42, 8.81,  10.77, 11.40
 };

  TH1D* hgamma = new TH1D("hgamma","",20,10.0,110.0);
  TH1D* hegamma = new TH1D("hegamma","",20,0.0,20.0);
  TH1D* hgammapull = new TH1D("hgammapull","",30,-5.5,9.5);

  TH1D* hdelB = new TH1D("hdelB","",20,80.0,180.0);
  TH1D* hedelB = new TH1D("hedelB","",20,0.0,20.0);
  TH1D* hdelBpull = new TH1D("hdelBpull","",30,-5.5,9.5);
 
  TH1D* hrB = new TH1D("hrB","",20,0.0,0.20*100);
  TH1D* herB = new TH1D("herB","",10,0.015*100,0.025*100);
  TH1D* hrBpull = new TH1D("hrBpull","",30,-5.5,9.5);

  double meangam=0.0;
  double rmsgam=0.0;
  double meanegam=0.0;
  double rmsegam=0.0;
  double meangampull=0.0;
  double rmsgampull=0.0;
  

  
  for (int itoy = 0; itoy<NTOYS; itoy++){
    
    cout << egamma[itoy] << endl;
    
   meangam+=gamma[itoy]/double(NTOYS);
   meanegam+=egamma[itoy]/double(NTOYS);
   meangampull+=(gamma[itoy]-60.0)/egamma[itoy]/double(NTOYS);
   rmsgam+=pow(gamma[itoy],2)/double(NTOYS);
   rmsegam+=pow(egamma[itoy],2)/double(NTOYS);
   rmsgampull+=pow((gamma[itoy]-60.0)/egamma[itoy],2)/double(NTOYS);
   

    hgamma->Fill(gamma[itoy]);
    hegamma->Fill(egamma[itoy]);
    hgammapull->Fill((gamma[itoy]-60.0)/egamma[itoy]);
    hdelB->Fill(delB[itoy]);
    hedelB->Fill(edelB[itoy]);
    hdelBpull->Fill((delB[itoy]-130.0)/edelB[itoy]);
    hrB->Fill(rB[itoy]*100);
    herB->Fill(erB[itoy]*100);
    hrBpull->Fill((rB[itoy]-0.1)/erB[itoy]);
  }

  cout << meangam << " " << meanegam << " " << meangampull << endl;
  cout << sqrt(rmsgam-pow(meangam,2)) << endl;
  cout << sqrt(rmsegam-pow(meanegam,2)) << endl;
  cout << sqrt(rmsgampull-pow(meangampull,2)) << endl;
  

  
  
  gStyle->SetOptStat(1100);
  gStyle->SetOptFit(111);
  
  gStyle->SetStatW(0.325); 
  gStyle->SetStatH(0.3);
 

  TCanvas* c1 = new TCanvas("c1","c1",900, 300);
  c1->Divide(3,1);
  c1->cd(1);
  
  hgamma->SetTitle("#gamma");

  hgamma->Draw();
  c1->cd(2);
  hegamma->SetTitle("#sigma_{#gamma}");
  
  hegamma->Draw();
  c1->cd(3);
  hgammapull->SetTitle("#gamma pull");
  
  hgammapull->Fit("gaus","","",-2.0,2.0);

  c1->Print("GammaPull.eps","eps");
  

  TCanvas* c2 = new TCanvas("c2","c2",900, 300);
  c2->Divide(3,1);
  
  c2->cd(1);
  
  hrB->SetTitle("r_{B}");
  hrB->Draw();
  c2->cd(2);
  herB->SetTitle("#sigma_{r_{B}}");
  herB->Draw();
  c2->cd(3);
  
  hrBpull->SetTitle("r_{B} pull");
  hrBpull->Fit("gaus");

  c2->Print("rBPull.eps","eps");
  
  TCanvas* c3 = new TCanvas("c3","c3",900, 300);
  c3->Divide(3,1);

  c3->cd(1);
  

  hdelB->SetTitle("#delta_{B}");
  hdelB->Draw();
  c3->cd(2);
  hedelB->SetTitle("#sigma_{#delta_{B}}");
  hedelB->Draw();
  c3->cd(3);
  
  hdelBpull->SetTitle("#delta_{B} pull");
  hdelBpull->Fit("gaus");
 

  c3->Print("delBPull.eps","eps");

  return;
  

}
