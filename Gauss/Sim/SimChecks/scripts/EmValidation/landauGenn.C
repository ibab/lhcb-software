#include <vector>;
#include <numeric>;
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include <iostream.h>
#include "TF1.h"
#include <math.h>

void landauGenn(double pval){

 //  p of the particle
//Nigel Watson 20110914  double p = 33.8 ; // GeV

//Nigel Watson 20110914 Get momenta as arg
double p=pval;
  
//Nigel Watson 20110914 Add masses in one place
// assume pions
//Nigel Watson 20110914  double pmass=0.140;
// assume muons
 double pmass=0.106;
// assume electrons
//Nigel Watson 20110914  double pmass=0.000511;
 
 double E = sqrt((p*p)+(pmass*pmass));
//Nigel Watson 20110914  double E = sqrt((p*p)+(0.140*0.140));
 double beta = p/E;
 double gamma = E/pmass;

 // 500 micron thick Silicon
 double t =300.;

//Nigel Watson 20110922  cout <<" Generate with p = " <<p <<" and m=" <<pmass<< endl;

//Nigel Watson 20110922  cout << "betagamma " << beta*gamma << "p "<<p<<endl; 
//Nigel Watson 20110922  cout << beta << " " << gamma << endl; 

 // landau parameters
 double b = landauWidth(beta,t);
 std::cout << "landau scale is " << b << std::endl; 
 double a = landauMPV(b,beta,gamma)+(0.226*b);

 cout << "betagamma " << beta*gamma << "p "<<p<<" landaumpv " << landauMPV(b,beta,gamma)<<endl; 

//Nigel Watson 20110922  cout << "landaumpv " << landauMPV(b,beta,gamma) << endl; 

 // landau generator
 TF1* fLandau= new TF1("fLandau","landau",0.,
                      600.);
 fLandau->SetParameters(100000,a,b);

 // gaussian smear for atomic binding
 TF1* fGaussGenerator = new TF1("m_gaussGenerator","gaus",-50.,
                                             50.);
 double binding = atomicBinding(t);
 cout << "binding " << binding << binding << std::endl;
 fGaussGenerator->SetParameters(1,0.,binding);

 TF1* fNoiseGenerator = new TF1("m_noiseGenerator","gaus",-50.,
                                            50.);
 fNoiseGenerator->SetParameters(1,0.,3.8);

 fLandauHisto = new TH1F("genCharge","GenCharge",1000,0.,500.);

 // generate random numbers 
 for (int iGen = 0; iGen < 1000000; ++iGen){
   float x = fLandau->GetRandom() + fGaussGenerator->GetRandom();
   //   + fNoiseGenerator->GetRandom();
   //+ fGaussGenerator->GetRandom();
  //     // fNoiseGenerator->GetRandom();
   fLandauHisto->Fill(x);
 } // iG

 fLandauHisto->Draw();

 // get the maximum and FWHM
 // former by parabola fit around max, latter by linear interpolation
 // note FWHM/MPV is related to the Si thickness

 TF1* f1 = new TF1("f1","pol2",0,200);
 
 // max
 float x1, val;
 estimateMax(fLandauHisto,f1,x1,val);
//Nigel Watson 20110922  cout << "max is " << x1 << " " << val<< endl;

 float upper, lower;
 estimateFWHM(fLandauHisto,lower, upper,val);  
 float scale = (upper-lower);
//Nigel Watson 20110922  cout << " fwhm is " << scale << endl;

 cout << "betagam " << beta*gamma << "p "<<p<<" lanmpv " << landauMPV(b,beta,gamma)<<" estMPV "<<x1<<" estFWMH "<<scale<<endl; 


 TFile* outFile = new TFile("landau.root","RECREATE","output ROOT file");
 fLandauHisto->Write();
 outFile->Write();
 outFile->Close();



 //fLandau->Draw();

}
void estimateMax(TH1F* aHisto,TF1* f1, float& x, float& maxValue){

  // parabola fit to max
  int iBin = aHisto->GetMaximumBin();
  x = aHisto->GetBinCenter(iBin);

  for (int i=0 ; i< 3 ; ++i ){
    aHisto->Fit(f1, "Q","",x-3.5, x+3.5);
    x = findMax(f1, x-4.5,x+4.5);
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


estimateFWHM(TH1F* aHisto, float& left, float& right, float val){

  int iMax = aHisto->GetMaximumBin();
  

  int reqLevel =   0.5*val; 
  
  int iLeft = iMax;
  while (((int) iLeft > 0)&&(aHisto->GetBinContent(iLeft)>reqLevel)){
    --iLeft;
  }
  left = interpolate(iLeft,reqLevel,-1 ,aHisto);
  //left = aHisto->GetBinCenter(iLeft);


  int iRight = iMax;
  while (((int) iRight < aHisto->GetNbinsX())&&(aHisto->GetBinContent(iRight)>reqLevel)){
    ++iRight;
  }
  
  // right = aHisto->GetBinCenter(iRight);
  right = interpolate(iRight,reqLevel,1,aHisto);

}

float interpolate(int iBin, float req, int off, TH1F* aHisto){
 
  float slope = (aHisto->GetBinContent(iBin) - aHisto->GetBinContent(iBin+off))/
         (aHisto->GetBinCenter(iBin) - aHisto->GetBinCenter(iBin+off));

  float constant = aHisto->GetBinContent(iBin)-(slope*aHisto->GetBinCenter(iBin));
  return ((req-constant)/slope);

}

double landauWidth(double beta, double t){

  return (0.017825*t/pow(beta,2.));
}

double densityEffect(double beta, double gamma){

  // See Sternheimer et al 1982
  
  double x = log10(beta*gamma); 

  double dEffect = 0.;
  if (x < 0.09666){
    // 0
  }  
  else if ((x > 0.09666)&&(x<2.5)){
    dEffect = 4.606*x - 4.435 +(0.3755*(pow(2.5-x,2.72)));
  }
  else {
    dEffect = 4.606*x - 4.435;
  }
  return dEffect;
}



double landauMPV(double width, double beta, double gamma){

  double mpv = width * (
			log(2.0*511.0*pow(beta*gamma,2.0)/0.174)
                        +log(width/0.174)
                        + 0.2
                        - pow(beta,2.)
                        - densityEffect(beta,gamma)); 

  return mpv;
}

double atomicBinding(double t){
 
  double delta2 = 0.18;
  return sqrt(delta2*t);
}
