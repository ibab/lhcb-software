#include "Mint/Bugg_BW.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "Mint/Utils.h"
#include "Mint/DalitzEventList.h"
#include "Mint/phaseSpaceIntegrals.h"
#include <cmath>
#include <complex>

using namespace std;
using namespace MINT;

std::complex<double> Bugg_BW::BreitWigner(){
    
    double s= mumsRecoMass2()/(GeV*GeV);
    
    double num = M() * Gamma_2pi(s);
    complex<double> den = M() * M() - s 
    - g_1_square(s) * (s - sA() * _m_pi * _m_pi)/(M() * M() - sA() * _m_pi * _m_pi)*z(s)- complex<double>(0,1) * M() * Gamma_tot(s);    
    
    return num/den;
}

double Bugg_BW::Gamma_2pi(double s){
    return g_1_square(s) * (s -sA()*_m_pi*_m_pi)/(M()*M() -sA()*_m_pi*_m_pi)*rho_2(s,_m_pi).real();
}

double Bugg_BW::g_1_square(double s){
    return (b1()+b2()*s)*exp(-(s-M()*M())/A());
}

double Bugg_BW::j1(double s){
    
    double rho_pipi = rho_2(s,_m_pi).real();
    double returnVal = 2.;
    if(rho_pipi>0.)returnVal += rho_pipi * log((1.-rho_pipi)/(1.+rho_pipi));
    
    return returnVal/TMath::Pi();
}

double Bugg_BW::z(double s){
    return j1(s)-j1(M()*M());
}


std::complex<double> Bugg_BW::Gamma_2K(double s){
    return g_KK()*g_1_square(s)*s/(M()*M())*exp(-alpha()*sqrt((s-4.*_m_K*_m_K)*(s-4.*_m_K*_m_K))) * rho_2(s,_m_K);
}

std::complex<double> Bugg_BW::Gamma_2eta(double s){
  return g_etaeta() *g_1_square(s)*s/(M() *M())*exp(-alpha()*sqrt((s-4.*_m_eta*_m_eta)*(s-4.*_m_eta*_m_eta))) * rho_2(s,_m_eta);
}

double Bugg_BW::Gamma_4pi(double s){
    if(s < 16. * _m_pi*_m_pi) return 0;
    return g_4pi() * rho_4(s)/rho_4(M()*M());
}

std::complex<double> Bugg_BW::rho_2(double s, double m){
    double rho_squared = 1.- 4. * m*m /s;
    if(rho_squared >= 0)return sqrt(rho_squared);
    else return complex<double>(0,1)*sqrt(-rho_squared);
}

double Bugg_BW::rho_4(double s){
    return 1./(1.+exp(lambda()*(s0()-s)));
}


//
