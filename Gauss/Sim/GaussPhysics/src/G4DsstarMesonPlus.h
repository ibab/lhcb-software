// $Id: G4DsstarMesonPlus.h,v 1.2 2006-01-09 20:52:22 robbep Exp $

#ifndef G4DsstarMesonPlus_h
#define G4DsstarMesonPlus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         DsstarMesonMinus                        ###
// ######################################################################

class G4DsstarMesonPlus : public G4ParticleDefinition
{
 private:
  static G4DsstarMesonPlus * theInstance ;
  G4DsstarMesonPlus( ) { }
  ~G4DsstarMesonPlus( ) { }


 public:
  static G4DsstarMesonPlus * Definition() ;
  static G4DsstarMesonPlus * DsstarMesonPlusDefinition() ;
  static G4DsstarMesonPlus * DsstarMesonPlus() ;
};


#endif
