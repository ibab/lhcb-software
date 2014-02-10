// $

#ifndef G4XiccPlusPlus_h
#define G4XiccPlusPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         XiccPlusPlus                        ###
// ######################################################################

class G4XiccPlusPlus : public G4ParticleDefinition
{
 private:
  static G4XiccPlusPlus * theInstance ;
  G4XiccPlusPlus( ) { }
  ~G4XiccPlusPlus( ) { }


 public:
  static G4XiccPlusPlus * Definition() ;
  static G4XiccPlusPlus * XiccPlusPlusDefinition() ;
  static G4XiccPlusPlus * XiccPlusPlus() ;
};


#endif
