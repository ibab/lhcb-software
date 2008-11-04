/// Fit to Landau convolved with Gaussian from testBeam


#include <vector>;
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include <iostream.h>

#define scaleOverMPV 0.062; // for 500 micron Silicon
//#define scaleOverMPV 0.063; // for 400 micron Silicon
//#define scaleOverMPV 0.064; // for 300 micron Silicon

void landauAna(){

  TFile aFile = TFile("myFile.root");
  aFile->ls();
  TH1F* aHisto = (TH1F*)gDirectory.Get("/IT/ITMonitor/Long/1;1");
  aHisto->SetTitle(""); 

  float x1, val;
  TF1* f1 = new TF1("f1","pol2",0,200);
  estimateMax(aHisto,f1,x1,val);
  cout << "max is " << x1 << " " << val<< endl;


   // four parameter fit with scale free. It is better to 
   // to fix the scale to be "0.06" of the MPV. This can be 
   // be done by changing the number of fit parameters to 3  
   // and commenting out a line in the landau function 
   TF1* langau = new TF1("langau",clangaufun,
                            10.,
                            200.,
                            4);
                            
  langau->SetParName(0,"MPV");
  langau->SetParName(1,"integral");
  langau->SetParName(2,"sigma of gauss");
  langau->SetParName(3,"scale");

  langau->SetParameter(0,x1);
  langau->SetParameter(1,aHisto->GetEntries());
  langau->SetParameter(2,2.0);
  langau->SetParameter(3,2.0);

  langau->SetParLimits(3,1.,10.0); 
  langau->SetParLimits(0,x1-10.,x1+10.); 
  langau->SetLineColor(2);
  langau->SetLineStyle(2);  

  aHisto->Fit("langau","" ,"",10.,100.);
  aHisto->DrawCopy("HISTO");
  langau->Draw("LSAME");
 

}


Double_t clangaufun(Double_t *x, Double_t *par) {

  // by Helge!

//     Fit parameters:
//     par[0]=Width (scale) parameter of Landau density
//     par[1]=Most Probable (MP, location) parameter of Landau density
//     par[2]=Total area (integral -inf to inf, normalization constant)
//     par[3]=Width (sigma) of convoluted Gaussian function
//
//     In the Landau distribution (represented by the CERLIB approximation),
//     the maximum is located at x=-0.22278298 with the location parameter=0.
//     This shift is corrected within this function, so that the actual
//     maximum is identical to the MP parameter.

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift  = -0.22278298;       // Landau maximum location

  // Control constants
  Double_t np = 200.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;

  Double_t fitfun;

  Double_t scale = scaleOverMPV*par[0];
  scale = par[3]; // comment this line to fix the scale to MPV

  // MP shift correction

  mpc = par[0] - mpshift * scale;

  // Range of convolution integral
  xlow = x[0] - sc * par[2];
  xupp = x[0] + sc * par[2];

  step = (xupp-xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for(i=1.0; i<=np/2; i++)

    {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc, scale) / scale;
      sum += fland * TMath::Gaus(x[0],xx,par[2]);

      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,scale) / scale;
      sum += fland * TMath::Gaus(x[0],xx,par[2]);
    }

  fitfun = (par[1] * step * sum * invsq2pi / par[2]);

// now just add another gaussion for the noise peak centered at zero
  // fitfun = fitfun + par[4]*exp(-x[0]*x[0]/par[5]/par[5]);

  return fitfun;

}

void estimateMax(TH1F* aHisto,TF1* f1, float& x, float& maxValue){

  // parabola fit to max
  int iBin = aHisto->GetMaximumBin();
  x = aHisto->GetBinCenter(iBin);


  for (int i=0 ; i< 3 ; ++i ){
    aHisto->Fit(f1, "Q","",x-2., x+2.);
    x = findMax(f1,x-4.,x+4.);
  }

  maxValue = f1->Eval(x);

}

double findMax(TF1* f1, float min, float max){

  int i;
  float lowerT = min;
  float upperT = max;

  for (i=0;i<5000;i++){
    if (f1->Eval(lowerT)<f1->Eval(upperT)){
      lowerT = 0.5*(lowerT+upperT);
    }
    else {
      upperT = 0.5*(lowerT+upperT);
    }
  } // i

  return lowerT;
}












