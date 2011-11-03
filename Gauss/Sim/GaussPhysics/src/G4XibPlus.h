

#ifndef G4XibPlus_h
#define G4XibPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         Xib plus                        ###
// ######################################################################

class G4XibPlus : public G4ParticleDefinition
{
 private:
  static G4XibPlus * theInstance ;
  G4XibPlus( ) { }
  ~G4XibPlus( ) { }


 public:
  static G4XibPlus
 * Definition() ;
  static G4XibPlus * XibPlusDefinition() ;
  static G4XibPlus * XibPlus() ;
};


#endif
