// $

#ifndef G4AntiXiccStarMinus_h
#define G4AntiXiccStarMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         AntiXiccStarMinus                        ###
// ######################################################################

class G4AntiXiccStarMinus : public G4ParticleDefinition
{
 private:
  static G4AntiXiccStarMinus * theInstance ;
  G4AntiXiccStarMinus( ) { }
  ~G4AntiXiccStarMinus( ) { }


 public:
  static G4AntiXiccStarMinus * Definition() ;
  static G4AntiXiccStarMinus * AntiXiccStarMinusDefinition() ;
  static G4AntiXiccStarMinus * AntiXiccStarMinus() ;
};


#endif
