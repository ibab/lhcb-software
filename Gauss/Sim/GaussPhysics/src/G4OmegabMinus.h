// $

#ifndef G4OmegabMinus_h
#define G4OmegabMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         Omegab minus                        ###
// ######################################################################

class G4OmegabMinus : public G4ParticleDefinition
{
 private:
  static G4OmegabMinus * theInstance ;
  G4OmegabMinus( ) { }
  ~G4OmegabMinus( ) { }


 public:
  static G4OmegabMinus
 * Definition() ;
  static G4OmegabMinus * OmegabMinusDefinition() ;
  static G4OmegabMinus * OmegabMinus() ;
};


#endif
