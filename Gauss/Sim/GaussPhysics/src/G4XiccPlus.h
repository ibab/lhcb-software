// $

#ifndef G4XiccPlus_h
#define G4XiccPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         XiccPlus                        ###
// ######################################################################

class G4XiccPlus : public G4ParticleDefinition
{
 private:
  static G4XiccPlus * theInstance ;
  G4XiccPlus( ) { }
  ~G4XiccPlus( ) { }


 public:
  static G4XiccPlus * Definition() ;
  static G4XiccPlus * XiccPlusDefinition() ;
  static G4XiccPlus * XiccPlus() ;
};


#endif
