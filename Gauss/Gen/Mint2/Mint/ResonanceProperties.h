#ifndef RESONANCEPROPERTIES_HH
#define RESONANCEPROPERTIES_HH
// author: Philippe d'Argent (p.dargent@cern.ch)

#include <string>
#include <iostream>
#include "Mint/ParticleProperties.h"
#include "Mint/FitParameter.h"
#include "Mint/NamedParameter.h"
#include "Mint/Utils.h"
#include "Mint/MinuitParameterSet.h"

class ResonancePropertiesFitRef;

class ResonanceProperties{
  friend class ResonancePropertiesFitRef;
 protected:
  static double massInit(int pdg_id);
  static double widthInit(int pdg_id);
  static double radiusInit(int pdg_id);

  int _pid;
  MINT::FitParameter _fitMass;
  MINT::FitParameter _fitWidth;
  MINT::FitParameter _fitRadius;
  MINT::FitParameter _fitAlpha;
  MINT::FitParameter _fitFermiLambda;
  MINT::FitParameter _fitFermiS0;
  MINT::FitParameter _fitRhoOmegaDelta_Re;
  MINT::FitParameter _fitRhoOmegaDelta_Im;
  MINT::FitParameter _fitFlatte_gPi;
  MINT::FitParameter _fitFlatte_gK_by_gPi;
  MINT::FitParameter _fitBugg_M;
  MINT::FitParameter _fitBugg_b1;
  MINT::FitParameter _fitBugg_b2;
  MINT::FitParameter _fitBugg_A;
  MINT::FitParameter _fitBugg_g_4pi;
  MINT::FitParameter _fitBugg_g_KK;
  MINT::FitParameter _fitBugg_g_etaeta;
  MINT::FitParameter _fitBugg_alpha;
  MINT::FitParameter _fitBugg_sA;
  MINT::FitParameter _fitBugg_4pi_lambda;
  MINT::FitParameter _fitBugg_4pi_s0;
    
 public:
  static std::string nameFromPid(int pdg_id);
  static int pidFromName(const std::string& name);
  static std::string nameFromName(const std::string& name);
  ResonanceProperties(int pid, MINT::MinuitParameterSet* mps=0);
  ResonanceProperties(const std::string& name, MINT::MinuitParameterSet* mps=0);
  double mass() const{return _fitMass;}
  double width() const{return _fitWidth;}
  double radius() const{return _fitRadius;}
  double alpha() const{return _fitAlpha;}
  double fermiLambda() const{return _fitFermiLambda;}
  double fermiS0() const{return _fitFermiS0;}
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
  int pid() const{return _pid;}

  void changeMassForDebug(double newVal){
    _fitMass = newVal;
  }
  
};

#endif
//
