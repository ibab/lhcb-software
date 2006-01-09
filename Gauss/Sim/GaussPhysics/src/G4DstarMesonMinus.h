// $Id: G4DstarMesonMinus.h,v 1.2 2006-01-09 20:52:22 robbep Exp $

#ifndef G4DstarMesonMinus_h
#define G4DstarMesonMinus_h 1

#include "globals.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"

// ######################################################################
// ###                         DstarMesonMinus                        ###
// ######################################################################

class G4DstarMesonMinus : public G4ParticleDefinition
{
 private:
  static G4DstarMesonMinus * theInstance ;
  G4DstarMesonMinus( ) { }
  ~G4DstarMesonMinus( ) { }


 public:
  static G4DstarMesonMinus * Definition() ;
  static G4DstarMesonMinus * DstarMesonMinusDefinition() ;
  static G4DstarMesonMinus * DstarMesonMinus() ;
};


#endif
