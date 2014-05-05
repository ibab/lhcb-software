// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/Flatte.h"

#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"

#include <cmath>
#include <iostream>

using namespace std;
using namespace MINT;

double Flatte::gPi()const{
  return _gPi;
}
double Flatte::gK()const{
  return _gK_by_gPi * gPi();
}

double Flatte::pdgMass(int PDG_ID) {
  return ParticlePropertiesList::mass(PDG_ID);
}
double Flatte::mPi0(){
  if (_pi0Mass <= 0){
    _pi0Mass = pdgMass(111);
  }
  return _pi0Mass;
}
double Flatte::mPiPlus(){
  if (_piPlusMass <= 0){
    _piPlusMass = pdgMass(211);
  }
  return _piPlusMass;
}
double Flatte::mK0(){
  if (_K0Mass <= 0){
    _K0Mass = pdgMass(311);
  }
  return _K0Mass;
}
double Flatte::mKPlus(){
  if (_KPlusMass <= 0){
    _KPlusMass = pdgMass(321);
  }
  return _KPlusMass;
}

double Flatte::aSqrtTerm(double m0, double m){
  bool dbThis=false;
  double a2 = 1.0 - (2*m0/m)*(2*m0/m);
  double a=0;
  if(a2 < 0){
    if(dbThis){
      cout << "WARNING in Flatte::aSqrtTerm()"
	   << " sqrt( 1.0 - (2m0/m)^2 ) imaginary"
	   << " (returning 0)."
	   << endl;
    }
  }else{
    a = sqrt(a2);
  }
  return a;
}

double Flatte::GofM(){
  bool dbThis=false;

  double m  = BW_BW::mumsRecoMass();

  double Gpipi = (1./3.) * aSqrtTerm(    mPi0(), m ) 
    +            (2./3.) * aSqrtTerm( mPiPlus(), m );


  double GKK   = (1./2.) * aSqrtTerm(     mK0(), m ) 
    +            (1./2.) * aSqrtTerm(  mKPlus(), m );

  double FlatteWidth = gPi() * Gpipi + gK() * GKK;
  if(dbThis){
    cout << "Flatte: compare Flatte Width = " << FlatteWidth
	 << " BW_BW::GofM() " << BW_BW::GofM()
	 << endl;
  }
  return FlatteWidth;
}
