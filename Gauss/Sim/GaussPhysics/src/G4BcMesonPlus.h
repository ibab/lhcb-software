// $Id: G4BcMesonPlus.h,v 1.2 2006-01-09 20:52:22 robbep Exp $

#ifndef G4BcMesonPlus_h
#define G4BcMesonPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         BcMesonPlus                        ###
// ######################################################################

class G4BcMesonPlus : public G4ParticleDefinition
{
 private:
  static G4BcMesonPlus * theInstance ;
  G4BcMesonPlus( ) { }
  ~G4BcMesonPlus( ) { }


 public:
  static G4BcMesonPlus * Definition() ;
  static G4BcMesonPlus * BcMesonPlusDefinition() ;
  static G4BcMesonPlus * BcMesonPlus() ;
};


#endif
