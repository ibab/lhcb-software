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
  return mumsFittableProperties().fitFlatte_gPi();
}
double Flatte::gK()const{
  return mumsFittableProperties().fitFlatte_gK_by_gPi() * gPi();
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

std::complex<double> Flatte::aSqrtTerm( const double& m0, const double& m ){
    const bool dbThis=false;
    
    const double a2 = 1.0 - (2*m0/m)*(2*m0/m);
    std::complex<double> a;
    
    if( a2 < 0 ){
        if(dbThis){
            std::cout << "WARNING in Flatte::aSqrtTerm()"
            << " sqrt( 1.0 - (2m0/m)^2 ) imaginary"
            << " (returning an imaginary number)."
            << std::endl;
        }
        const std::complex<double> i(0.0, 1.0);
        a = i * sqrt(fabs(a2));
    }else{
        const std::complex<double> unit(1.0, 0.0);
        a = ((std::complex<double>) sqrt(a2)) * unit;
    }
    return a;
}


std::complex<double> Flatte::complexGofM(){
    bool dbThis=false;
    
    double m  = BW_BW::mumsRecoMass();
    
    std::complex<double> Gpipi = (1./3.) * aSqrtTerm(    mPi0(), m )
    +            (2./3.) * aSqrtTerm( mPiPlus(), m );
    
    
    std::complex<double> GKK   = (1./2.) * aSqrtTerm(     mK0(), m )
    +            (1./2.) * aSqrtTerm(  mKPlus(), m );
    
    std::complex<double> FlatteWidth = gPi() * Gpipi + gK() * GKK;
    if(dbThis){
        cout << "Flatte: compare Flatte Width = " << FlatteWidth
        << " BW_BW::GofM() " << BW_BW::GofM()
        << endl;
    }
    return FlatteWidth;
}

double Flatte::GofM(){
    cout << "ERROR Flatte::GofM() called should not have happenned!" <<endl;
    std::complex<double> val = complexGofM();
    return abs(val);
}

std::complex<double> Flatte::BreitWigner() {
    double mpdg = mumsMass();
    std::complex<double> i(0.0, 1.0);
    std::complex<double> invBW = (((std::complex<double>) mpdg*mpdg -
                                   mumsRecoMass2()) -  i * mpdg * complexGofM());
    return 1.*GeV*GeV/invBW;  
    
}

