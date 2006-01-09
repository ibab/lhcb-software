// $Id: G4Etac.h,v 1.2 2006-01-09 20:52:22 robbep Exp $

#ifndef G4Etac_h
#define G4Etac_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         Etac                        ###
// ######################################################################

class G4Etac : public G4ParticleDefinition
{
 private:
  static G4Etac * theInstance ;
  G4Etac( ) { }
  ~G4Etac( ) { }


 public:
  static G4Etac * Definition() ;
  static G4Etac * EtacDefinition() ;
  static G4Etac * Etac() ;
};


#endif
