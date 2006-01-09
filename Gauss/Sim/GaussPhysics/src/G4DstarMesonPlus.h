// $Id: G4DstarMesonPlus.h,v 1.2 2006-01-09 20:52:22 robbep Exp $

#ifndef G4DstarMesonPlus_h
#define G4DstarMesonPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         DstarMesonPlus                        ###
// ######################################################################

class G4DstarMesonPlus : public G4ParticleDefinition
{
 private:
  static G4DstarMesonPlus * theInstance ;
  G4DstarMesonPlus( ) { }
  ~G4DstarMesonPlus( ) { }


 public:
  static G4DstarMesonPlus * Definition() ;
  static G4DstarMesonPlus * DstarMesonPlusDefinition() ;
  static G4DstarMesonPlus * DstarMesonPlus() ;
};


#endif
