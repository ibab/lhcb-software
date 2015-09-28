#ifndef RESONANCEPROPERTIES_FITREF_HH
#define RESONANCEPROPERTIES_FITREF_HH
// author: Philippe d'Argent (p.dargent@cern.ch) & Jonas Rademacker

#include "Mint/FitParRef.h"
#include "Mint/FitParDependent.h"

class ResonanceProperties;

class ResonancePropertiesFitRef : public MINT::FitParDependent{
 protected:
    int _pid;
    MINT::FitParRef _fitMass;
    MINT::FitParRef _fitWidth;
    MINT::FitParRef _fitRadius;
    MINT::FitParRef _fitAlpha;
    MINT::FitParRef _fitFermiLambda;
    MINT::FitParRef _fitFermiS0;
    MINT::FitParRef _fitRhoOmegaDelta_Re;
    MINT::FitParRef _fitRhoOmegaDelta_Im;
    MINT::FitParRef _fitFlatte_gPi;
    MINT::FitParRef _fitFlatte_gK_by_gPi;
    MINT::FitParRef _fitBugg_M;
    MINT::FitParRef _fitBugg_b1;
    MINT::FitParRef _fitBugg_b2;
    MINT::FitParRef _fitBugg_A;
    MINT::FitParRef _fitBugg_g_4pi;
    MINT::FitParRef _fitBugg_g_KK;
    MINT::FitParRef _fitBugg_g_etaeta;
    MINT::FitParRef _fitBugg_alpha;
    MINT::FitParRef _fitBugg_sA;
    MINT::FitParRef _fitBugg_4pi_lambda;
    MINT::FitParRef _fitBugg_4pi_s0;
    
 public:
    ResonancePropertiesFitRef(const ResonanceProperties& repo
			      , IFitParRegister* daddy=0);

    double mass()   const{return _fitMass;}
    double width()  const{return _fitWidth;}
    double radius() const{return _fitRadius;}
    double alpha()  const{return _fitAlpha;}
    double fermiLambda() const{return _fitFermiLambda;}
    double fermiS0() const{return _fitFermiS0;}
    double rhoOmegaDelta_Re() const{return _fitRhoOmegaDelta_Re;}
    double rhoOmegaDelta_Im() const{return _fitRhoOmegaDelta_Im;}
    double fitFlatte_gPi() const{return _fitFlatte_gPi;}  
    double fitFlatte_gK_by_gPi() const{return _fitFlatte_gK_by_gPi;}  
    double fitBugg_M() const{return _fitBugg_M;}
    double fitBugg_b1() const{return _fitBugg_b1;}
    double fitBugg_b2() const{return _fitBugg_b2;}
    double fitBugg_A() const{return _fitBugg_A;}
    double fitBugg_g_4pi() const{return _fitBugg_g_4pi;}
    double fitBugg_g_KK() const{return _fitBugg_g_KK;}
    double fitBugg_g_etaeta() const{return _fitBugg_g_etaeta;}
    double fitBugg_alpha() const{return _fitBugg_alpha;}
    double fitBugg_sA() const{return _fitBugg_sA;}
    double fitBugg_4pi_lambda() const{return _fitBugg_4pi_lambda;}
    double fitBugg_4pi_s0() const{return _fitBugg_4pi_s0;}

    int pid()       const{return _pid;}

    virtual ~ResonancePropertiesFitRef(){}
};

#endif
//
