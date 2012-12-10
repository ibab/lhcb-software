// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/CLEO2012_Flatte.h"

#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"

#include <cmath>
#include <iostream>

using namespace std;
using namespace MINT;

double CLEO2012_Flatte::gPi()const{
  return _gPi;
}
double CLEO2012_Flatte::gK()const{
  return _gK_by_gPi * gPi();
}

double CLEO2012_Flatte::pdgMass(int PDG_ID) {
  return ParticlePropertiesList::mass(PDG_ID);
}
double CLEO2012_Flatte::mPi0(){
  if (_pi0Mass <= 0){
    _pi0Mass = pdgMass(111);
  }
  return _pi0Mass;
}
double CLEO2012_Flatte::mPiPlus(){
  if (_piPlusMass <= 0){
    _piPlusMass = pdgMass(211);
  }
  return _piPlusMass;
}
double CLEO2012_Flatte::mK0(){
  if (_K0Mass <= 0){
    _K0Mass = pdgMass(311);
  }
  return _K0Mass;
}
double CLEO2012_Flatte::mKPlus(){
  if (_KPlusMass <= 0){
    _KPlusMass = pdgMass(321);
  }
  return _KPlusMass;
}

double CLEO2012_Flatte::aSqrtTerm(double m0, double m){
  bool dbThis=false;
  double a2 = 1.0 - (2*m0/m)*(2*m0/m);
  double a=0;
  if(a2 < 0){
    if(dbThis){
      cout << "WARNING in CLEO2012_Flatte::aSqrtTerm()"
	   << " sqrt( 1.0 - (2m0/m)^2 ) imaginary"
	   << " (returning 0)."
	   << endl;
    }
  }else{
    a = sqrt(a2);
  }
  return a;
}

double CLEO2012_Flatte::GofM(){
  bool dbThis=false;

  double m  = CLEO2012_BW_BW::mumsRecoMass();

  double Gpipi = (1./3.) * aSqrtTerm(    mPi0(), m ) 
    +            (2./3.) * aSqrtTerm( mPiPlus(), m );


  double GKK   = (1./2.) * aSqrtTerm(     mK0(), m ) 
    +            (1./2.) * aSqrtTerm(  mKPlus(), m );

  double CLEO2012_FlatteWidth = gPi() * Gpipi + gK() * GKK;
  if(dbThis){
    cout << "CLEO2012_Flatte: compare CLEO2012_Flatte Width = " << CLEO2012_FlatteWidth
	 << " CLEO2012_BW_BW::GofM() " << CLEO2012_BW_BW::GofM()
	 << endl;
  }
  return CLEO2012_FlatteWidth;
}
