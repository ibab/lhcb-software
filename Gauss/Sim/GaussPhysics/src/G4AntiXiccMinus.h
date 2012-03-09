// $

#ifndef G4AntiXiccMinus_h
#define G4AntiXiccMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         AntiXiccMinus                        ###
// ######################################################################

class G4AntiXiccMinus : public G4ParticleDefinition
{
 private:
  static G4AntiXiccMinus * theInstance ;
  G4AntiXiccMinus( ) { }
  ~G4AntiXiccMinus( ) { }


 public:
  static G4AntiXiccMinus * Definition() ;
  static G4AntiXiccMinus * AntiXiccMinusDefinition() ;
  static G4AntiXiccMinus * AntiXiccMinus() ;
};


#endif
