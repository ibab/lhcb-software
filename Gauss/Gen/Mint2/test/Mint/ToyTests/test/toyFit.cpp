// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Minimiser.h"
#include "Mint/Neg2LL.h"
#include "Mint/EventList.h"

#include "Mint/PdfBase.h"

#include "TRandom.h"
#include "TCanvas.h"
#include "TH1F.h"

#include "TMath.h"

#include <iostream>

using namespace std;
using namespace MINT;

class evt{
  double _x;
  int _type;
public:
  evt(double x=0, int type=0) : _x(x), _type(type){}
  double& x(){return _x;}
  int& type(){return _type;}
};

class TwoGaussPdf : public PdfBase<evt>{
  FitParameter m1, m2, s1, s2, f1;
  double gauss(double mean, double sigma, double x){
    if(0 == sigma) return 0;
    double d=(x-mean)/sigma;
    double n=1./(sqrt(2.0*TMath::Pi())*sigma);
    return n  * exp(-0.5*d*d);
  }
public:
  TwoGaussPdf() : m1("m1"), m2("m2"), s1("s1"), s2("s2"), f1("f1"){}
  double getVal(evt& event){
    double x = event.x();
    double f=f1;
    return f*this->gauss(m1, s1, x) + (1.0-f)*this->gauss(m2, s2, x);
  }
};

class TwoGaussPdf2 : public PdfBase<evt>{
  FitParameter m1, m2, s1, s2, f1;
  double gauss(double mean, double sigma, double x){
    if(0 == sigma) return 0;
    double d=(x-mean)/sigma;
    double n=1./(sqrt(2.0*TMath::Pi())*sigma);
    return n  * exp(-0.5*d*d);
  }
public:
  TwoGaussPdf2() : m1("m1"), m2("m2"), s1("s1"), s2("s2"), f1("f1"){}
  double getVal(double x, int type){
    double f=f1;
    double a=f*this->gauss(m1, s1, x);
    double b=(1.0-f)*this->gauss(m2, s2, x);
    return (1==type? a/(a+b) : b/(a+b));
  }
  double getVal(evt& event){
    return getVal(event.x(), event.type());
  }
};


int toyFit(){

  NamedParameter<int> N("N");
  NamedParameter<int> RandomSeed("RandomSeed");
  EventList<evt> xvalues;
  NamedParameter<double> m1t("m1t"), s1t("s1t")
    , m2t("m2t"), s2t("s2t"), f1t("f1t");

  TH1F hdat("2gausdata", "2gausdata", 100, -5, 5);
  TH1F hfit("2gausfit", "2gausfit", 100, -5, 5);

  gRandom->SetSeed(RandomSeed);

  for(int i = 0; i < N; i++){
    double m, s;
    int t;
    if(gRandom->Rndm() < f1t){
      m=m1t; s=s1t; t=1;
    }else{
      m=m2t; s=s2t; t=2;
    }
    double x=gRandom->Gaus(m, s);
    xvalues.Add( evt(x, t));
    hdat.Fill(x);
  }

  //TwoGaussPdf my2GaussPdf;
  TwoGaussPdf2 my2GaussPdf;

  Neg2LL<evt> fcn(&my2GaussPdf, &xvalues);

  Minimiser mini(&fcn);
  mini.doFit();


  for(int i=1; i <= hfit.GetNbinsX(); i++){
    double x =  hfit.GetBinCenter(i);
    double dx = hfit.GetBinWidth(i);
    double P  = 0;//my2GaussPdf.getVal(x);
    hfit.Fill(x, P*dx*N);
  }

  TCanvas* tcan = new TCanvas("can");
  hdat.Draw();
  hfit.SetLineColor(2);
  hfit.Draw("Same L");
  tcan->Print("plots.eps");

  cout << "Done. Fitted " << N << " events"
       << " generated with"
       << m1t << s1t << m2t << s2t << f1t
       << ". Result above."
       << endl;

  return 0;
}

int main(){
  return toyFit();
}
//
