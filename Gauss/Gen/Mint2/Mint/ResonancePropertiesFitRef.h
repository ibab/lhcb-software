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
 public:
    ResonancePropertiesFitRef(const ResonanceProperties& repo, IFitParRegister* daddy=0);
    double mass() const{return _fitMass;}
    double width() const{return _fitWidth;}
    double radius() const{return _fitRadius;}
    double alpha() const{return _fitAlpha;}
    int pid() const{return _pid;}
};

#endif
//
