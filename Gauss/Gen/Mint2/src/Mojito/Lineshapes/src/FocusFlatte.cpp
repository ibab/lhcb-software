// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/FocusFlatte.h"

#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"

#include <cmath>
#include <iostream>

using namespace std;
using namespace MINT;

double FocusFlatte::gPi()const{
  return _gPi;
}
double FocusFlatte::gK()const{
  return _gK_by_gPi * gPi();
}

double FocusFlatte::pdgMass(int PDG_ID) {
  return ParticlePropertiesList::mass(PDG_ID);
}
double FocusFlatte::mPi0(){
  if (_pi0Mass <= 0){
    _pi0Mass = pdgMass(111);
  }
  return _pi0Mass;
}
double FocusFlatte::mPiPlus(){
  if (_piPlusMass <= 0){
    _piPlusMass = pdgMass(211);
  }
  return _piPlusMass;
}
double FocusFlatte::mK0(){
  if (_K0Mass <= 0){
    _K0Mass = pdgMass(311);
  }
  return _K0Mass;
}
double FocusFlatte::mKPlus(){
  if (_KPlusMass <= 0){
    _KPlusMass = pdgMass(321);
  }
  return _KPlusMass;
}

std::complex<double> FocusFlatte::aSqrtTerm(double m0, double m){
  bool dbThis=false;
  double a2 = 1.0 - (2*m0/m)*(2*m0/m);
  std::complex<double> a;
  
  if(a2 < 0){
    if(dbThis){
      cout << "WARNING in FocusFlatte::aSqrtTerm()"
	   << " sqrt( 1.0 - (2m0/m)^2 ) imaginary"
	   << " (returning an imaginary number)."
	   << endl;
    }
   std::complex<double> i(0.0, 1.0); 
   a = i * a2;
   }else{
    std::complex<double> unit(1.0, 0.0);
    a = ((std::complex<double>) sqrt(a2)) * unit;
  }
  return a;
}

std::complex<double> FocusFlatte::complexGofM(){
  bool dbThis=false;

  double m  = BW_BW::mumsRecoMass();

  std::complex<double> Gpipi = (1./3.) * aSqrtTerm(    mPi0(), m ) 
    +            (2./3.) * aSqrtTerm( mPiPlus(), m );


  std::complex<double> GKK   = (1./2.) * aSqrtTerm(     mK0(), m ) 
    +            (1./2.) * aSqrtTerm(  mKPlus(), m );

  std::complex<double> FlatteWidth = gPi() * Gpipi + gK() * GKK;
  if(dbThis){
    cout << "LAURENS CLASS FocusFlatte: compare Flatte Width = " << FlatteWidth
	 << " BW_BW::GofM() " << BW_BW::GofM()
	 << endl;
  }
  return FlatteWidth;
}

double FocusFlatte::GofM(){
  cout << "ERROR FocusFlatte::GofM() called should not have happenned!" <<endl;
  std::complex<double> val = complexGofM();
  return abs(val);
}

std::complex<double> FocusFlatte::BreitWigner() {
  //  double mpdg = mumsPDGMass();
   double mpdg = mumsMass();
   std::complex<double> i(0.0, 1.0);
   std::complex<double> invBW = (((std::complex<double>) mpdg*mpdg -
				  mumsRecoMass2()) -  i * mpdg * complexGofM());
   return 1.*GeV*GeV/invBW;  

}
