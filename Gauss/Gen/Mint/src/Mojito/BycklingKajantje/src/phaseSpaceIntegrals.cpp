// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/phaseSpaceIntegrals.h"

#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/lambda.h"

#include <iostream>
#include <cmath>

#include "TF2.h"

using namespace std;

TF1* PhaseSpaceIntegral3body::_f = 0;
TF1* PhaseSpaceIntegral4body::_f = 0;

IGenFct* Global_s12Fct=0;
IGenFct* Global_s34Fct=0;
IGenFct* Global_s123Fct=0;

double d4body_by_ds12(Double_t* x, Double_t* p){
  bool dbThis=false;
  double s12 = x[0];
  if(s12 < 0) return 0;
  double m12 = sqrt(s12);
  double mum = p[0];
  double m1  = p[1];
  double m2  = p[2];
  double m3  = p[3];
  double m4  = p[4];

  if(dbThis) cout << " d4body_by_ds12 called with " 
		  << mum << ", "
		  << m1 << ", "
		  << m2 << ", "
		  << m3 << ", "
		  << m4 << endl;

  PhaseSpaceIntegral3body p3;

  double p3val = p3.getVal(mum, m12, m3, m4);
  double p2val = phaseSpaceIntegral2body(m12, m1, m2);
  if(dbThis){
    cout << " d4body_by_ds12: returning " << p2val << " * " << p3val
	 << " = " << p2val * p3val
	 << endl;
  }
  return p2val * p3val;
}

double d3body_by_ds12(Double_t* x, Double_t* p){
  bool dbThis=false;

  double s12 = x[0];
  if(s12 < 0) return 0;
  double m12 = sqrt(s12);
  double mum = p[0];
  double m1  = p[1];
  double m2  = p[2];
  double m3  = p[3];
  if(dbThis) cout << " d3body_by_ds12 called with " 
		  << mum << ", "
		  << m1 << ", "
		  << m2 << ", "
		  << m3 << endl;

  return phaseSpaceIntegral2body(mum, m12, m3)
    *    phaseSpaceIntegral2body(m12, m1, m2);
}

double d_ps4_by_ds12ds34_withFct(Double_t* x, Double_t* p) {
  //bool dbThis=false;

  double rho12 = x[0];
  double rho34 = x[1];
  double s12= Global_s12Fct->coordTransformToS(rho12);
  double s34= Global_s34Fct->coordTransformToS(rho34);

  
  if(s12 < 0) return 0;
  double m12 = sqrt(s12);
  if(m12 < p[1] + p[2]) return 0;

  if(s34 < 0) return 0;
  double m34 = sqrt(s34);
  if(m34 < p[3] + p[4]) return 0;
  if(m12 + m34 > p[0]) return 0;
  

  double ps1 = phaseSpaceIntegral2body(p[0]
				      , m12
				      , m34);

  if(ps1 <=0) return 0;
    
  double ps2 = phaseSpaceIntegral2body(m12
				       , p[1]
				       , p[2]);
  
  if(ps2 <=0) return 0;
    
  double ps3 = phaseSpaceIntegral2body(m34
				       , p[3]
				       , p[4]);
  if(ps3 <=0) return 0;


  double fct12 = Global_s12Fct->transformedFctValue(rho12);
  double fct34 = Global_s34Fct->transformedFctValue(rho34);

  return fct12 * fct34 * ps1*ps2*ps3;
}

double d_ps4_by_ds123ds12_withFct(Double_t* x, Double_t* p){


  double rho123 = x[0];
  double rho12  = x[1];

  double s123 = Global_s123Fct->coordTransformToS(rho123);
  double  s12 =  Global_s12Fct->coordTransformToS(rho12);


  
  if(s123 < 0) return 0;
  double m123 = sqrt(s123);
  if(m123 + p[4] > p[0]) return 0;
  if(m123 < p[1] + p[2] + p[3]) return 0;
  
  if(s12 < 0) return 0;
  double m12 = sqrt(s12);
  if(m12 + p[3] > m123)return 0;
  if(m12 < p[1] + p[2]) return 0;
  

  double ps1 = phaseSpaceIntegral2body(p[0]
				       , m123
				       , p[4]);

  if(ps1 <=0) return 0;

    
  double ps2 = phaseSpaceIntegral2body(m123
				       , sqrt(s12)
				       , p[3]);

  if(ps2 <=0) return 0;

  double ps3 = phaseSpaceIntegral2body(m12
				  , p[1]
				  , p[2]);

  if(ps3 <=0) return 0;

  double fct123 = Global_s123Fct->transformedFctValue(rho123);
  double fct12  = Global_s12Fct->transformedFctValue(rho12);

  return fct123*fct12 * ps1*ps2*ps3;

}

/* now inlined, see header.
double phaseSpaceIntegral2body(const DalitzEventPattern& _pat){
  if(_pat.size() != 3){
    cout << "phaseSpaceIntegral2body: wrong pattern " << _pat << endl;
  }
  double mum = _pat[0].mass();
  double d1  = _pat[1].mass();
  double d2  = _pat[2].mass();
  return phaseSpaceIntegral2body(mum, d1, d2);
}

double phaseSpaceIntegral2body(double mum, double d1, double d2){
  if(mum <=0 ) return 0;
  if(mum < d1 + d2) return 0;

  double la = lambda(mum*mum, d1*d1, d2*d2);
  if(la <= 0) return 0;

  return pi * sqrt(la)/(2*mum*mum);
  
}
*/


PhaseSpaceIntegral3body::PhaseSpaceIntegral3body(){
  if(0 == _f){
    _f = new TF1("d3body_by_ds12", d3body_by_ds12
		 , 0, 10*GeV*GeV
		 , 4
		 );
  }
  if(0 == _f){
    cout << "phaseSpaceIntegral3body::phaseSpaceIntegral3body()"
	 << " failed to get TF1!!!" << endl;
  }
}


double PhaseSpaceIntegral3body::getVal(const DalitzEventPattern& _pat){
  if(_pat.size() != 4){
    cout << "phaseSpaceIntegral3body: wrong pattern " << _pat << endl;
  }
  double mum = _pat[0].mass();
  double d1  = _pat[1].mass();
  double d2  = _pat[2].mass();
  double d3  = _pat[3].mass();

  return getVal(mum, d1, d2, d3);
}

double PhaseSpaceIntegral3body::getVal(double mum, double d1
				       , double d2, double d3){
  bool dbThis=false;
  double min_m12 = (d1 + d2);
  double max_m12 = (mum - d3);
  if(dbThis)cout << "PhaseSpaceIntegral3body::getVal : from, to " 
		 << min_m12 << ", " << max_m12 << endl;
  if(min_m12 >= max_m12) return 0;

  double min_s12 = min_m12*min_m12;
  double max_s12 = max_m12*max_m12;

  Double_t para[4]={mum, d1, d2, d3};

  _f->SetParameters(para);


  /*
  Int_t np = 1000;
  double x[np];
  double w[np];
  _f->CalcGaussLegendreSamplingPoints(np,x,w,1e-15);
  double returnVal = _f->IntegralFast(np,x,w, min_s12, max_s12, para);
  */

  double returnVal =  _f->Integral(min_s12, max_s12, para);
  if(dbThis) cout << "PhaseSpaceIntegral3body::getVal() returning " 
		  << returnVal << endl;
  return returnVal;
}



PhaseSpaceIntegral4body::PhaseSpaceIntegral4body(){
  if(0 == _f){
    _f = new TF1("d4body_by_ds12", d4body_by_ds12
		 , 0, 10*GeV*GeV
		 , 5
		 );
  }
  if(0 == _f){
    cout << "phaseSpaceIntegral4body::phaseSpaceIntegral3body()"
	 << " failed to get TF1!!!" << endl;
  }
}


double PhaseSpaceIntegral4body::getVal(const DalitzEventPattern& _pat){
  if(_pat.size() != 5){
    cout << "phaseSpaceIntegral4body: wrong pattern " << _pat << endl;
  }
  double mum = _pat[0].mass();
  double d1  = _pat[1].mass();
  double d2  = _pat[2].mass();
  double d3  = _pat[3].mass();
  double d4  = _pat[4].mass();
  return getVal(mum, d1, d2, d3, d4);
}
double PhaseSpaceIntegral4body::getVal(double mum
				       , double d1, double d2
				       , double d3, double d4){

  bool dbThis = false;

  double min_m12 = (d1 + d2);
  double max_m12 = (mum - d3 - d4);
  if(dbThis)cout << "PhaseSpaceIntegral4body::getVal : from, to " 
		 << min_m12 << ", " << max_m12 << endl;
  if(min_m12 >= max_m12) return 0;

  double min_s12 = min_m12*min_m12;
  double max_s12 = max_m12*max_m12;

  Double_t para[5]={mum, d1, d2, d3, d4};

  _f->SetParameters(para);

  Int_t np = 1000;
  double xpts[np];
  double wpts[np];
  _f->CalcGaussLegendreSamplingPoints(np,xpts,wpts,1e-15);
  return _f->IntegralFast(np,xpts,wpts, min_s12, max_s12, para);
  //  return _f->Integral(min_s12, max_s12, para);
}
double PhaseSpaceIntegral4body::getValCheck(const DalitzEventPattern& _pat){
  if(_pat.size() != 5){
    cout << "phaseSpaceIntegral4body: wrong pattern " << _pat << endl;
  }
  double mum = _pat[0].mass();
  double d1  = _pat[1].mass();
  double d2  = _pat[2].mass();
  double d3  = _pat[3].mass();
  double d4  = _pat[4].mass();
  return getValCheck(mum, d1, d2, d3, d4);
}
double PhaseSpaceIntegral4body::getValCheck(double mum, double d1
					    , double d2, double d3
					    , double d4){
  
  bool dbThis = false;

  double min_m12 = (d1 + d2);
  double max_m12 = (mum - d3 - d4);
  if(dbThis)cout << "PhaseSpaceIntegral4body::getVal : from, to " 
		 << min_m12 << ", " << max_m12 << endl;
  if(min_m12 >= max_m12) return 0;

  double min_s12 = min_m12*min_m12;
  double max_s12 = max_m12*max_m12;

  Double_t para[5]={mum, d1, d2, d3, d4};

  _f->SetParameters(para);

  int NSteps = 10000;
  double sum = 0;
  double dx = (max_s12 - min_s12)/((double)NSteps);
  
  for(int i=0; i< NSteps; i++){
    double dim = 0.5 + (double) i;
    double x = min_s12 + dx*dim;
    sum += _f->Eval(x);
  }
  return sum*dx;
}


PhaseSpaceIntegral4bodyWith_s12s34::
PhaseSpaceIntegral4bodyWith_s12s34(IGenFct* s12f
				   , IGenFct* s34f
				   , const DalitzEventPattern& pat
				   )
  : _s12Fct(s12f)
  , _s34Fct(s34f)
  , _pat(pat)
{
}

PhaseSpaceIntegral4bodyWith_s123s12::
PhaseSpaceIntegral4bodyWith_s123s12(IGenFct* s123f
				    , IGenFct* s12f
				    , const DalitzEventPattern& pat
				    )
  : _s123Fct(s123f)
  , _s12Fct(s12f)
  , _pat(pat)
{
}

double PhaseSpaceIntegral4bodyWith_s12s34::getVal()const{
  bool dbThis=true;

  double xmi = _s12Fct->getRhoMi();
  double xma = _s12Fct->getRhoMa();

  double ymi = _s34Fct->getRhoMi();
  double yma = _s34Fct->getRhoMa();

  const int numParticles=5;

  Global_s12Fct = _s12Fct;
  Global_s34Fct = _s34Fct;

  Double_t p[numParticles];
  for(int i=0; i < numParticles; i++){
    p[i] = _pat[i].mass();
  }

  TF2 intFunc("d_ps4_by_ds12ds34_withFct"
	      , d_ps4_by_ds12ds34_withFct
	      , xmi, xma
	      , ymi, yma
	      , numParticles
	      );
  

  intFunc.SetParameters(p);

  Double_t a[2];
  Double_t b[2];
  a[0]=xmi; b[0]=xma;
  a[1]=ymi; b[1]=yma;

  Int_t nDim    = 2;
  Int_t mipt    =    4000000;
  Int_t mapt    =  400000000;
  Double_t epsilon =  1.e-8;
  Double_t relerr  = -1;
  Int_t nfnevl = 9999;
  Int_t iFail = -9999;
  double returnVal = intFunc.IntegralMultiple(nDim, a, b
					      , mipt, mapt
					      , epsilon, relerr
					      , nfnevl, iFail);
  if(dbThis){
    cout << "12, 34:  just called : "
	 << " intFunc.IntegralMultiple( " << nDim 
	 << ", " << a[0] << "-" << b[0]
	 << ", " << a[1] << "-" << b[1] 
	 << ", " << mipt <<", " << mapt
	 << ", " << epsilon << ", " << relerr 
	 << ", " << nfnevl << ", " << iFail
	 << " );" 
	 << " = " << returnVal << endl;
    //    cout << " ..... compare with: " << intFunc.Integral(xmi, xma, ymi, yma) << endl;

  }
  return returnVal;

}

double PhaseSpaceIntegral4bodyWith_s123s12::getVal()const{
  bool dbThis=true;

  double xmi = _s123Fct->getRhoMi();
  double xma = _s123Fct->getRhoMa();

  double ymi = _s12Fct->getRhoMi();
  double yma = _s12Fct->getRhoMa();

  const int numParticles=5;

  Global_s123Fct = _s123Fct;
  Global_s12Fct  = _s12Fct;

  TF2 intFunc("d_ps4_by_ds13ds12_withFct"
	      , d_ps4_by_ds123ds12_withFct
	      , xmi, xma
	      , ymi, yma
	      , numParticles
	      );

  Double_t p[numParticles];
  for(int i=0; i < numParticles; i++){
    p[i] = _pat[i].mass();
  }

  intFunc.SetParameters(p);

  Double_t a[2];
  Double_t b[2];
  a[0]=xmi; b[0]=xma;
  a[1]=ymi; b[1]=yma;

  Int_t nDim    = 2;
  Int_t mipt    =    4000000;
  Int_t mapt    =  400000000;
  Double_t epsilon =  1.e-8;
  Double_t relerr  = -1;
  Int_t nfnevl =  9999;
  Int_t iFail  = -9999;
  double returnVal = intFunc.IntegralMultiple(nDim, a, b
					      , mipt, mapt
					      , epsilon, relerr
					      , nfnevl, iFail);
  
  if(dbThis){
    cout << "123, 12:  just called : "
	 << " intFunc.IntegralMultiple( " << nDim 
	 << ", " << a[0] << "-" << b[0]
	 << ", " << a[1] << "-" << b[1] 
	 << ", " << mipt <<", " << mapt
	 << ", " << epsilon << ", " << relerr 
	 << ", " << nfnevl << ", " << iFail
	 << " );" 
	 << " = " << returnVal << endl;
    //    cout << " ..... compare with: " << intFunc.Integral(xmi, xma, ymi, yma) << endl;
  }

  return returnVal;

  //  return intFunc.Integral(xmi, xma, ymi, yma);

}

double phaseSpaceIntegral_upTo4body(const DalitzEventPattern& pat){

  if(pat.numDaughters() < 2 ){
    return 0;
  }else if(pat.numDaughters() == 2 ){
    return phaseSpaceIntegral2body(pat);
  }else if(pat.numDaughters() == 3){
    PhaseSpaceIntegral3body p3;
    return p3.getVal(pat);
  }else if(pat.numDaughters() == 4){
    PhaseSpaceIntegral4body p4;
    return p4.getVal(pat);
  }else{
    return -9999;
  }

}

//
//


