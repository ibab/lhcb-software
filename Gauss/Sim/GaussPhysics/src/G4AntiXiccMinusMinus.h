// $

#ifndef G4AntiXiccMinusMinus_h
#define G4AntiXiccMinusMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                      AntiXiccMinusMinus                        ###
// ######################################################################

class G4AntiXiccMinusMinus : public G4ParticleDefinition
{
 private:
  static G4AntiXiccMinusMinus * theInstance ;
  G4AntiXiccMinusMinus( ) { }
  ~G4AntiXiccMinusMinus( ) { }


 public:
  static G4AntiXiccMinusMinus * Definition() ;
  static G4AntiXiccMinusMinus * AntiXiccMinusMinusDefinition() ;
  static G4AntiXiccMinusMinus * AntiXiccMinusMinus() ;
};


#endif
