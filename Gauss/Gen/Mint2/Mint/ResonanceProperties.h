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

  int pid() const{return _pid;}

  void changeMassForDebug(double newVal){
    _fitMass = newVal;
  }
  
};

#endif
//
