#include <vector>;
#include <numeric>;
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include <iostream.h>
#include <fstream.h>
#include "TF1.h"
#include "TString.h"
#include <math.h>

void landau4new(const TString& hfile="dummy.root")
{
  // TFile* aFile = new TFile("fast-dray-0.1-10GeV.root");
  //TFile* aFile = new TFile("Gauss-5ev-20100716-histos.root");
  //TFile* aFile = new TFile("GaussRead_on_274sim-readVeloMatt.root");
//Nigel Watson 20110914  TFile* aFile = new TFile("bugfix_Gauss-50K-mu-50GeV.root");
//Nigel Watson 20110915  TFile* aFile = new TFile("Gauss-1000000ev-20110913-histos.root");
 TFile* aFile = new TFile(hfile);
 TH1F* fLandauHisto = (TH1F*)aFile->Get("/EMGaussMoniVeloGaussMoni/1003;1");
 //TH1F* fLandauHisto = (TH1F*)aFile->Get("/VeloGaussMoniVeloGaussMoni/1002;1");
 //TH1F* fLandauHisto = (TH1F*)aFile->Get("/VeloGaussMoniVeloGaussMoni/2002;1");
 fLandauHisto->Rebin(2);
 fLandauHisto->Draw("HISTO");

 TF1* f1 = new TF1("f1","pol2",0,200);
 
 // max
 float x1, val;
 estimateMax(fLandauHisto,f1,x1,val);
 cout << "max is " << x1 << " " << val<< endl;

 float upper, lower;
 estimateFWHM(fLandauHisto,lower, upper,val);  
 float scale = (upper-lower);
 cout << " fwhm is " << scale << endl;

//Nigel Watson 20110916  Re-activate this code.

//Nigel Watson 20110916 TString dirname = ".";
//Nigel Watson 20110916 TString rootname = "./" + hfile 
//Nigel Watson 20110916 rootname = dirname + "/" + rootname;

 TString ofile="output/landau_output.root";
 //Nigel Watson 20110916  strcat(ofile,"landau4new.root");

 cout << "new file is called " << ofile << endl;

//Peter Griffith. Adding Branches for FWHM and MPV to add to root files

TFile* outFile = new TFile( ofile,"RECREATE","output ROOT file");
TTree *tree = new TTree();

double MPV[] = {x1};
double FWHM[] = {scale};

TGraph *fitval = new TGraph(1,MPV,FWHM);
fitval->SetTitle("fitval");
fitval->Write();
fLandauHisto->Write();
outFile->Write();
outFile->Close();

fLandauHisto->Draw("HISTO");

ofstream myfile;
 myfile.open("output/landau_output.txt",ios::app);
/* 
 if (myfile.is_open())
 {
   myfile <<"From file " << hfile << endl;
   myfile<<" MPV = " << x1 << ", FWHM = " << scale << endl;
   myfile.close();
   cout<<" MPV = " << x1 << ", FWHM = " << scale << endl;
 }
 */

 if (myfile.is_open())
 {
   myfile<< x1 <<"\t" << scale << endl;
   myfile.close();
   cout<<" MPV = " << x1 << ", FWHM = " << scale << endl;
 }
 else cout << "Unable to open file";
 
}


void estimateMax(TH1F* aHisto,TF1* f1, float& x, float& maxValue){

  // parabola fit to max
  int iBin = aHisto->GetMaximumBin();
  x = aHisto->GetBinCenter(iBin);

  for (int i=0 ; i< 3 ; ++i ){
    aHisto->Fit(f1,"Q","",x-3.5, x+3.5);
    Double_t sigma = f1->GetParameter(2); //value of 2nd parameter
    Double_t esigma = f1->GetParError(2); //error on 2nd parameter
    cout << "Sigma: " << sigma<<" Err: "<<esigma<< endl;

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

 std::cout << "upperT " << upperT<< std::endl;
 std::cout << "lowerT " << lowerT<< std::endl;
 std::cout << "upperT - lowerT " << upperT-lowerT<< std::endl;
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
/*
double densityEffect(double beta, double gamma){


  double x = log10(beta*gamma);

  double dEffect = 0.;
  if (x < -0.05){
    // 0
  }
  else if ((x > -0.05)&&(x<3.)){
    dEffect = 4.606*x - 4.447 +(0.162*(pow(3.0-x,3.0)));
  }
  else {
    dEffect = 4.606*x - 4.44;
  }
  return dEffect;
}
*/

double densityEffect(double beta, double gamma){

  
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

  std::cout << "first term " << log(2.0*511.0*pow(beta*gamma,2.0)/0.174) << std::endl;
  
 std::cout << "second term " << log(width/0.174) << std::endl;
 std::cout << "third term " <<  pow(beta,2.) << std::endl;
 std::cout << "density effect" <<  densityEffect(beta,gamma) << std::endl;

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
