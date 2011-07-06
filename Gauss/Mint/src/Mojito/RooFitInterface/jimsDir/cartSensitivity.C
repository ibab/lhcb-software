// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include "TMath.h"
void cartSensitivity()
{

  TRandom2* myRandom = new TRandom2(123456);

  TH1D* hgam = new TH1D("hgam","",80,40.0,80.0);
  TH1D* hdel = new TH1D("hdel","",80,110.0,150.0);
  TH1D* hrb = new TH1D("hrb","",80,0.08,0.12);
  

 
  for (Int_t i=0;i<10000;i++){
    Double_t xp = myRandom->Gaus(-0.0977,0.0160);
    Double_t yp = myRandom->Gaus(-0.0224,0.0144);
    Double_t xm = myRandom->Gaus(0.0339,0.01138);
    Double_t ym = myRandom->Gaus(0.0934,0.01639);

    Double_t rb=0.5*(sqrt(pow(xp,2)+pow(yp,2))+sqrt(pow(xm,2)+pow(ym,2)));
    
    TComplex zp(xp,yp);
    TComplex zm(xm,ym);

    Double_t xip = zp.Theta()*(180/TMath::Pi());
    if (xip<0.0) xip+=360.0;
    Double_t xim = zm.Theta()*(180/TMath::Pi());
    if (xim<0.0) xim+=360.0;
    

    
    Double_t gamma = 0.5*(xip - xim);
    
    Double_t deltab = 0.5*(xip + xim);
    
    hgam->Fill(gamma);
    hrb->Fill(rb);

    hdel->Fill(deltab);
    


  }
  
  TCanvas* c1 = new TCanvas("c1","c1",800,200);
  c1->Divide(3,1);
  c1->cd(1);
  hgam->Draw();
  c1->cd(2);
  hdel->Draw();
  c1->cd(3);
  hrb->Draw();
  

  return;
  
}
