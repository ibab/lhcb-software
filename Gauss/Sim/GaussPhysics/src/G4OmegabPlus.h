

#ifndef G4OmegabPlus_h
#define G4OmegabPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         Omegab plus                        ###
// ######################################################################

class G4OmegabPlus : public G4ParticleDefinition
{
 private:
  static G4OmegabPlus * theInstance ;
  G4OmegabPlus( ) { }
  ~G4OmegabPlus( ) { }


 public:
  static G4OmegabPlus
 * Definition() ;
  static G4OmegabPlus * OmegabPlusDefinition() ;
  static G4OmegabPlus * OmegabPlus() ;
};


#endif
