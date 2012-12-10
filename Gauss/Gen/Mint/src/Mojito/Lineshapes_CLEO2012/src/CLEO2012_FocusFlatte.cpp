// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/CLEO2012_FocusFlatte.h"

#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"

#include <cmath>
#include <iostream>

using namespace std;
using namespace MINT;

double CLEO2012_FocusFlatte::gPi()const{
  return _gPi;
}
double CLEO2012_FocusFlatte::gK()const{
  return _gK_by_gPi * gPi();
}

double CLEO2012_FocusFlatte::pdgMass(int PDG_ID) {
  return ParticlePropertiesList::mass(PDG_ID);
}
double CLEO2012_FocusFlatte::mPi0(){
  if (_pi0Mass <= 0){
    _pi0Mass = pdgMass(111);
  }
  return _pi0Mass;
}
double CLEO2012_FocusFlatte::mPiPlus(){
  if (_piPlusMass <= 0){
    _piPlusMass = pdgMass(211);
  }
  return _piPlusMass;
}
double CLEO2012_FocusFlatte::mK0(){
  if (_K0Mass <= 0){
    _K0Mass = pdgMass(311);
  }
  return _K0Mass;
}
double CLEO2012_FocusFlatte::mKPlus(){
  if (_KPlusMass <= 0){
    _KPlusMass = pdgMass(321);
  }
  return _KPlusMass;
}

std::complex<double> CLEO2012_FocusFlatte::aSqrtTerm(double m0, double m){
  bool dbThis=false;
  double a2 = 1.0 - (2*m0/m)*(2*m0/m);
  std::complex<double> a;
  
  if(a2 < 0){
    if(dbThis){
      cout << "WARNING in CLEO2012_FocusFlatte::aSqrtTerm()"
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

std::complex<double> CLEO2012_FocusFlatte::complexGofM(){
  bool dbThis=false;

  double m  = CLEO2012_BW_BW::mumsRecoMass();

  std::complex<double> Gpipi = (1./3.) * aSqrtTerm(    mPi0(), m ) 
    +            (2./3.) * aSqrtTerm( mPiPlus(), m );


  std::complex<double> GKK   = (1./2.) * aSqrtTerm(     mK0(), m ) 
    +            (1./2.) * aSqrtTerm(  mKPlus(), m );

  std::complex<double> FlatteWidth = gPi() * Gpipi + gK() * GKK;
  if(dbThis){
    cout << "LAURENS CLASS CLEO2012_FocusFlatte: compare Flatte Width = " << FlatteWidth
	 << " CLEO2012_BW_BW::GofM() " << CLEO2012_BW_BW::GofM()
	 << endl;
  }
  return FlatteWidth;
}

double CLEO2012_FocusFlatte::GofM(){
  cout << "ERROR CLEO2012_FocusFlatte::GofM() called should not have happenned!" <<endl;
  std::complex<double> val = complexGofM();
  return abs(val);
}

std::complex<double> CLEO2012_FocusFlatte::BreitWigner() {
   double mpdg = mumsPDGMass();
  std::complex<double> i(0.0, 1.0);
  std::complex<double> invBW = (((std::complex<double>) mpdg*mpdg -
  mumsRecoMass2()) -  i * mpdg * complexGofM());
  return 1.*GeV*GeV/invBW;  

}
