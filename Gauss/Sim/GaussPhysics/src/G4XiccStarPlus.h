// $

#ifndef G4XiccStarPlus_h
#define G4XiccStarPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         XiccStarPlus                        ###
// ######################################################################

class G4XiccStarPlus : public G4ParticleDefinition
{
 private:
  static G4XiccStarPlus * theInstance ;
  G4XiccStarPlus( ) { }
  ~G4XiccStarPlus( ) { }


 public:
  static G4XiccStarPlus * Definition() ;
  static G4XiccStarPlus * XiccStarPlusDefinition() ;
  static G4XiccStarPlus * XiccStarPlus() ;
};


#endif
